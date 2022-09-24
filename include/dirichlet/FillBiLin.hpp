/// \file       include/dirichlet/FillBiLin.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::FillBiLin.

#ifndef DIRICHLET_FILL_BILIN_HPP
#define DIRICHLET_FILL_BILIN_HPP

#include "impl/Weights.hpp"     // Weights
#include "impl/bin2x2.hpp"      // bin2x2()
#include "impl/unbin2x2.hpp"    // unbin2x2()
#include "impl/validSquare.hpp" // validSquare()
#include <eigen3/Eigen/Sparse>  // SparseMatrix

namespace dirichlet {


using Eigen::Array3;
using Eigen::ArrayX2i;
using Eigen::ArrayX3;
using Eigen::ArrayXi;
using Eigen::ArrayXX;
using Eigen::ArrayXXi;
using Eigen::seq;
using Eigen::SimplicialCholesky;
using Eigen::SparseMatrix;


/// Fill holes in image by approximately solving Dirichlet-problem for
/// zero-valued Laplacian across hole-pixels.
///
/// Solution is approximate because deeper in interior of any hole are larger,
/// square regions, across each of which bilinear interpolant is used.  Corners
/// of each square region are included in global solution to Dirichlet-problem,
/// but interior of each square region is excluded from global solution in
/// order drastically to reduce size of linear problem whenever there be a deep
/// region to be filled.  Approximation is good because exact solution is
/// nearly linear deep in interior away from boundary.
///
/// Constructor sets up linear problem by analyzing mask indicating size of
/// image and pixels to be filled.
///
/// Instance is function-object that solves linear problem by analyzing
/// image-data.
class FillBiLin {
  /// Left, right, top, bottom, and center weights for each pixel.
  impl::Weights weights_;

  /// Number of squares over which to interpolate.
  int numSquares_= 0;

  /// Number of pixels, each of whose values is solved for in linear problem.
  int numSolvePixels_= 0;

  /// Corner-coordinates and side-length for each square over which to
  /// interpolate.
  ArrayX3<int16_t> corners_;

  /// Mask that has been extended if necessary so that each of the number of
  /// rows and the number of columns is a power of two.  When first
  /// initialized, each element is true only if corresponding pixel should be
  /// filled.  However, after the initial call to (recursive) binMask() return,
  /// an element is true only if corresponding pixel both be not involved in
  /// any square interpolant and should be solved for.
  ArrayXX<bool> extendedMask_;

  /// Coordinates of each pixel whose value is to be solved for.  First column
  /// in `coordMap_` is offset of pixel's row; second, of pixel's column.
  ArrayX2i coords_;

  // Map from rectangular coordinates of pixel in linear problem to
  // corresponding row-offset of same coordinates in `coords_`.
  ArrayXXi coordsMap_;

  /// Square matrix for linear problem.
  SparseMatrix<float> a_;

  /// Pointer to Cholesky-decomposition of square matrix for linear problem.
  SimplicialCholesky<SparseMatrix<float>> *A_= nullptr;

  /// Extend mask with with zeros so that it is power of two along each
  /// dimension, and convert to array of boolean.
  /// \tparam P       Type of each pixel-value in mask.
  /// \param  msk     Pointer to first pixel of row-major mask-image.
  /// \param  stride  Pointer-increments between consecutive pixels.
  template<typename P> void extendMask(P const *msk, int stride);

  /// Modify `weights_` for corners and edges of square to interpolate, add
  /// entry to `corners_`, increment `numSquares_`, and set false in
  /// `extendedMask_` every pixel corresponding to square.
  /// \param r   Row of valid pixel at binning-factor bf in binMask().
  /// \param c   Col of valid pixel at binning-vactor bf in binMask().
  /// \param bf  Absolute binning factor relative to unbinned mask.
  void registerSquare(int r, int c, int bf);

  /// Modify `weights_` for corners and edges of square to interpolate.
  /// \param top  Top    unbinned row.
  /// \param lft  Left   unbinned column.
  /// \param bot  Bottom unbinned row.
  /// \param rgt  Right  unbinned column.
  void registerSquareWeights(int top, int lft, int bot, int rgt);

  /// Set false in `extendedMask_` every pixel corresponding to square.
  /// \param top  Top    unbinned row.
  /// \param lft  Left   unbinned column.
  /// \param bot  Bottom unbinned row.
  /// \param rgt  Right  unbinned column.
  void eliminateSquareFromMask(int top, int lft, int bot, int rgt);

  /// Recursive function that performs binning on higher-resolution mask `hi`,
  /// detects valid squares at current binning level, calls itself (if enough
  /// pixels at current binning), modifies `weights_` for corners and edges of
  /// squares to interpolate, increments `numSquares_` as needed, modifies
  /// `corners_` as needed, and returns result of next-lower-resolution
  /// binning.
  ///
  /// \param  hi  Higher-resolution mask.
  /// \param  bf  Absolute binning factor of lo relative to unbinned mask.
  /// \return     Expression-template for end-result at next lower resolution.
  ///
  ArrayXX<bool> binMask(ArrayXX<bool> const &hi, int bf) {
    // Bin to current level.
    ArrayXX<bool> const lo= impl::bin2x2(hi);
    int const           nr= lo.rows();
    int const           nc= lo.cols();
    // Identify interpolable squares at current level.
    if(nr < 3 || nc < 3) return ArrayXX<bool>::Zero(nr, nc);
    ArrayXX<bool> loValid= impl::validSquare(lo);
    // Count number of interpolable squares at current level.
    int const sum= loValid.template cast<int>().sum();
    // Make recursive call only if enough interpolable squares.
    if(nr >= 8 && nc >= 8 && sum >= 4) {
      // Make recursive call.
      ArrayXX<bool> const lowerValid= binMask(lo, bf * 2);
      // Eliminate from loValid any overlap in lowerValid.
      loValid= loValid && !impl::unbin2x2(lowerValid);
    }
    for(int c= 0; c < loValid.cols(); ++c) {
      for(int r= 0; r < loValid.rows(); ++r) {
        if(loValid(r, c)) registerSquare(r, c, bf);
      }
    }
    return loValid;
  }

  /// After binMask() is done, constructor calls this to set, if necessary,
  /// weights for each image-corner pixel whose value is to be solved for.
  /// \param h  Height of image.
  /// \param w  Width  of image.
  void populateCornerWeights(int h, int w);

  /// After binMask() is done, constructor calls this to set, if necessary,
  /// weights for each image-edge pixel whose value is to be solved for.
  /// \param h  Height of image.
  /// \param w  Width  of image.
  void populateEdgeWeights(int h, int w);

  /// After binMask() is done, constructor calls this to set, if necessary,
  /// weights for each interior image-pixel whose value is to be solved for and
  /// which has not already had its weights set by way of its assocation with
  /// the perimter of a square marked for interpolation.
  /// \param h  Height of image.
  /// \param w  Width  of image.
  void populateInteriorWeights(int h, int w);

public:
  /// Set up linear problem by analyzing mask.
  /// \tparam P       Type of each pixel-value in mask.
  /// \param  msk     Pointer to first pixel of row-major mask-image.
  /// \param  w       Width of image.
  /// \param  h       Height of image.
  /// \param  stride  Pointer-increments between consecutive pixels.
  template<typename P> FillBiLin(P const *msk, int w, int h, int stride= 1);

  /// Deallocate Cholesky-docomposition.
  virtual ~FillBiLin() {
    if(A_ != nullptr) {
      delete A_;
      A_= nullptr;
    }
  }

  /// Width of image.
  /// \return  Width of image.
  int w() const { return weights_.w(); }

  /// Height of image.
  /// \return  Height of image.
  int h() const { return weights_.h(); }

  /// Left, right, top, bottom, and center weights for each pixel.
  ///
  /// For each pixel not to be filled, each weight is zero.  For each pixel to
  /// be filled by interpolant, each weight is zero.  For each pixel along edge
  /// of square to interpolate, weights represent one-dimensional Laplacian,
  /// with nonzero weights only along edge (and zero weights perpendicular to
  /// edge).  For each pixel along edge of image, weights correspond to mean of
  /// available neighbors.  Otherwise, for pixel whose value is to be solved
  /// for in region to be filled, weights correspond to mean of pixel.
  ///
  /// \return  Left, right, top, bottom, and center weights for each pixel.
  impl::Weights const &weights() const { return weights_; }

  /// Number of squares over which to interpolate.
  /// \return  Number of squares over which to interpolate.
  int numSquares() const { return numSquares_; }

  /// Array with three columns, two for coordinates of top-left pixel of each
  /// square over which to interpolate and one for number of pixels along side
  /// of square.
  ///
  /// Size is set initially (in constructor) to h()*w()/16 rows, which
  /// corresponds to the limiting number of squares.  Size is adjusted downward
  /// after actual number of squares is computed.
  ///
  /// \return  Corner-coordinates and side-length for each square over which to
  ///          interpolate.
  ArrayX3<int16_t> const &corners() const { return corners_; }

  void initMatrix();
};


} // namespace dirichlet

// ---------------
// Implementation.
// ---------------

#include "impl/Image.hpp" // Image, ImageMap
#include "impl/pow2.hpp"  // pow2()
#include <iostream>       // cout, endl

namespace dirichlet {


using Eigen::seq;
using Eigen::Stride;
using Eigen::Triplet;
using std::cerr;
using std::endl;
using std::vector;


template<typename P> void FillBiLin::extendMask(P const *msk, int stride) {
  using impl::pow2;
  // Allocate space for extended mask.
  extendedMask_= ArrayXX<bool>::Zero(pow2(h()), pow2(w()));
  // Map mask to logical image.
  impl::ImageMap<P const> mask(msk, h(), w(), stride);
  // Initialize range of rows and columns for copying.
  auto const rseq= seq(0, h() - 1);
  auto const cseq= seq(0, w() - 1);
  // Copy original mask into extended mask, and convert to ones and zeros.
  extendedMask_(rseq, cseq)= (mask != P(0));
}


void FillBiLin::registerSquare(int r, int c, int bf) {
  int const top= r * bf;       // Top    unbinned row.
  int const lft= c * bf;       // Left   unbinned column.
  int const bot= top + bf - 1; // Bottom unbinned row.
  int const rgt= lft + bf - 1; // Right  unbinned column.
  registerSquareWeights(top, lft, bot, rgt);
  eliminateSquareFromMask(top, lft, bot, rgt);
  // Add corner and size for current square.
  corners_.row(numSquares_) << top, lft, bf;
  ++numSquares_;
}


void FillBiLin::registerSquareWeights(int top, int lft, int bot, int rgt) {
  auto const crnrRows= seq(top, bot, bot - top);
  auto const crnrCols= seq(lft, rgt, rgt - lft);
  auto const edgeRows= seq(top + 1, bot - 1);
  auto const edgeCols= seq(lft + 1, rgt - 1);
  // Write weights for corners.
  weights_.top()(crnrRows, crnrCols)= +1;
  weights_.bot()(crnrRows, crnrCols)= +1;
  weights_.lft()(crnrRows, crnrCols)= +1;
  weights_.rgt()(crnrRows, crnrCols)= +1;
  weights_.cen()(crnrRows, crnrCols)= -4;
  // Write weights for vertical edges.
  weights_.top()(edgeRows, crnrCols)= +1;
  weights_.bot()(edgeRows, crnrCols)= +1;
  weights_.lft()(edgeRows, crnrCols)= +0;
  weights_.rgt()(edgeRows, crnrCols)= +0;
  weights_.cen()(edgeRows, crnrCols)= -2;
  // Write weights for horizontal edges.
  weights_.top()(crnrRows, edgeCols)= +0;
  weights_.bot()(crnrRows, edgeCols)= +0;
  weights_.lft()(crnrRows, edgeCols)= +1;
  weights_.rgt()(crnrRows, edgeCols)= +1;
  weights_.cen()(crnrRows, edgeCols)= -2;
}


void FillBiLin::eliminateSquareFromMask(int top, int lft, int bot, int rgt) {
  auto const rows          = seq(top, bot);
  auto const cols          = seq(lft, rgt);
  extendedMask_(rows, cols)= false;
}


void FillBiLin::populateCornerWeights(int h, int w) {
  if(extendedMask_(0, 0)) {
    weights_.bot()(0, 0)= +1;
    weights_.rgt()(0, 0)= +1;
    weights_.cen()(0, 0)= -2;
  }
  if(extendedMask_(0, w - 1)) {
    weights_.bot()(0, w - 1)= +1;
    weights_.lft()(0, w - 1)= +1;
    weights_.cen()(0, w - 1)= -2;
  }
  if(extendedMask_(h - 1, w - 1)) {
    weights_.top()(h - 1, w - 1)= +1;
    weights_.lft()(h - 1, w - 1)= +1;
    weights_.cen()(h - 1, w - 1)= -2;
  }
  if(extendedMask_(h - 1, 0)) {
    weights_.top()(h - 1, 0)= +1;
    weights_.rgt()(h - 1, 0)= +1;
    weights_.cen()(h - 1, 0)= -2;
  }
}


void FillBiLin::populateEdgeWeights(int h, int w) {
  auto const rows            = seq(1, h - 2);
  auto const cols            = seq(1, w - 2);
  auto const lftMask         = extendedMask_(rows, 0);
  auto const topMask         = extendedMask_(0, cols);
  auto const rgtMask         = extendedMask_(rows, w - 1);
  auto const botMask         = extendedMask_(h - 1, cols);
  weights_.top()(rows, 0)    = lftMask.cast<int8_t>() * (+1);
  weights_.rgt()(rows, 0)    = lftMask.cast<int8_t>() * (+1);
  weights_.bot()(rows, 0)    = lftMask.cast<int8_t>() * (+1);
  weights_.cen()(rows, 0)    = lftMask.cast<int8_t>() * (-3);
  weights_.lft()(0, cols)    = topMask.cast<int8_t>() * (+1);
  weights_.rgt()(0, cols)    = topMask.cast<int8_t>() * (+1);
  weights_.bot()(0, cols)    = topMask.cast<int8_t>() * (+1);
  weights_.cen()(0, cols)    = topMask.cast<int8_t>() * (-3);
  weights_.top()(rows, w - 1)= rgtMask.cast<int8_t>() * (+1);
  weights_.lft()(rows, w - 1)= rgtMask.cast<int8_t>() * (+1);
  weights_.bot()(rows, w - 1)= rgtMask.cast<int8_t>() * (+1);
  weights_.cen()(rows, w - 1)= rgtMask.cast<int8_t>() * (-3);
  weights_.lft()(h - 1, cols)= botMask.cast<int8_t>() * (+1);
  weights_.rgt()(h - 1, cols)= botMask.cast<int8_t>() * (+1);
  weights_.top()(h - 1, cols)= botMask.cast<int8_t>() * (+1);
  weights_.cen()(h - 1, cols)= botMask.cast<int8_t>() * (-3);
}


void FillBiLin::populateInteriorWeights(int h, int w) {
  auto const rows= seq(1, h - 2);
  auto const cols= seq(1, w - 2);
  auto const mask= extendedMask_(rows, cols).cast<int8_t>();
  weights_.top()(rows, cols)+= mask * (+1);
  weights_.bot()(rows, cols)+= mask * (+1);
  weights_.lft()(rows, cols)+= mask * (+1);
  weights_.rgt()(rows, cols)+= mask * (+1);
  weights_.cen()(rows, cols)+= mask * (-4);
}


void FillBiLin::initMatrix() {
  numSolvePixels_= (weights_.cen() != 0).cast<int>().sum();
  coords_        = ArrayX2i(numSolvePixels_, 2);
  unsigned i     = 0;
  for(int c= 0; c < w(); ++c) {
    for(int r= 0; r < h(); ++r) {
      if(weights_.cen()(r, c) != 0) {
        coords_(i, 0)= r;
        coords_(i, 1)= c;
        ++i;
      }
    }
  }
  // TBS
}


// Maximum number of corners is h*w/16 because smallest square has 16 pixels.
template<typename P>
FillBiLin::FillBiLin(P const *msk, int w, int h, int stride):
    weights_(h, w),   //
    coords_(h * w, 2) //
{
  extendMask(msk, stride);
  if(extendedMask_.rows() < 2 || extendedMask_.cols() < 2) {
    cerr << "FillBilLin: ERROR: m0 too small" << std::endl;
    return;
  }
  ArrayXX<bool> const m1= impl::bin2x2(extendedMask_);
  if(m1.rows() < 2 || m1.cols() < 2) {
    cerr << "FillBilLin: ERROR: m1 too small" << endl;
    return;
  }
  // Smallest square has 16 pixels.
  corners_.resize(h * w / 16, 3);
  binMask(m1, 4);
  corners_.conservativeResize(numSquares_, 3);
  populateCornerWeights(h, w);
  populateEdgeWeights(h, w);
  populateInteriorWeights(h, w);
  initMatrix();
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_BILIN_HPP

// EOF
