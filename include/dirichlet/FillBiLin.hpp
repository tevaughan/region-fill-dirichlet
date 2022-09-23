/// \file       include/dirichlet/FillBiLin.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::FillBiLin.

#ifndef DIRICHLET_FILL_BILIN_HPP
#define DIRICHLET_FILL_BILIN_HPP

#include "impl/Weights.hpp"     // Weights
#include "impl/bin2x2.hpp"      // bin2x2()
#include "impl/unbin2x2.hpp"    // unbin2x2()
#include "impl/validSquare.hpp" // validSquare()

namespace dirichlet {


using Eigen::Array3;
using Eigen::ArrayX3;
using Eigen::ArrayXi;
using Eigen::ArrayXX;
using Eigen::seq;


/// Fill holes in image by approximately solving Dirichlet-problem for
/// zero-valued Laplacian across hole-pixels.
///
/// Solution is approximate because deeper in interior of any hole are larger,
/// square regions, across each of which bilinear interpolant is used.  Corners
/// of each square region are included in global solution to Dirichlet-problem,
/// but interior of each square region is excluded from global solution in
/// order drastically to reduce size of linear problem.  Approximation is good
/// because exact solution is nearly linear deep in interior away from
/// boundary.
///
/// Constructor sets up linear problem by analyzing mask indicating size of
/// image and pixels to be filled.
///
/// Instance is function-object that solves linear problem by analyzing
/// image-data.
class FillBiLin {
  /// Left, right, top, bottom, and center weights for each pixel.  For each
  /// pixel not to be filled, each weight is zero.  For each pixel to be filled
  /// by interpolant, each weight is zero.  For each pixel along edge of square
  /// to interpolate, weights represent one-dimensional Laplacian, with nonzero
  /// weights only along edge (and zero weights perpendicular to edge).  For
  /// each pixel along edge of image, weights correspond to mean of available
  /// neighbors.  Otherwise, for pixel whose value is to be solved for in
  /// region to be filled, weights correspond to mean of pixel.
  impl::Weights weights_;

  /// Number of squares over which to interpolate.
  int numSquares_= 0;

  /// Array with three columns, two for coordinates of top-left pixel of square
  /// to interpolate and one for number of pixels along side of square.  Size
  /// is set initially (in constructor) to h()*w()/16 rows, which corresponds
  /// to the limiting number of squares.  Size is adjusted downward after
  /// initial call to binMask() returns.
  ArrayX3<int16_t> corners_;

  /// Extend mask with with zeros so that it is power of two along each
  /// dimension, and convert to array of boolean.
  /// \tparam P       Type of each pixel-value in mask.
  /// \param  msk     Pointer to first pixel of row-major mask-image.
  /// \param  stride  Pointer-increments between consecutive pixels.
  /// \return         Extended mask.
  template<typename P> ArrayXX<bool> extendMask(P const *msk, int stride);

  /// Modify `weights_` for corners and edges of square to interpolate, add
  /// entry to `corners_`, and increment `numSquares_`.
  /// \param r   Row of valid pixel at binning-factor bf in binMask().
  /// \param c   Col of valid pixel at binning-vactor bf in binMask().
  /// \param bf  Absolute binning factor relative to unbinned mask.
  void registerSquare(int r, int c, int bf);

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

public:
  /// Set up linear problem by analyzing mask.
  /// \tparam P       Type of each pixel-value in mask.
  /// \param  msk     Pointer to first pixel of row-major mask-image.
  /// \param  w       Width of image.
  /// \param  h       Height of image.
  /// \param  stride  Pointer-increments between consecutive pixels.
  template<typename P> FillBiLin(P const *msk, int w, int h, int stride= 1);

  /// Width of image.
  /// \return  Width of image.
  int w() const { return weights_.w(); }

  /// Height of image.
  /// \return  Height of image.
  int h() const { return weights_.h(); }
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
using std::vector;


template<typename P>
ArrayXX<bool> FillBiLin::extendMask(P const *msk, int stride) {
  using impl::pow2;
  // Allocate space for extended mask.
  ArrayXX<bool> extendedMask= ArrayXX<bool>::Zero(pow2(h()), pow2(w()));
  // Map mask to logical image.
  impl::ImageMap<P const> mask(msk, h(), w(), impl::Stride(stride));
  // Initialize range of rows and columns for copying.
  auto const rseq= seq(0, h() - 1);
  auto const cseq= seq(0, w() - 1);
  // Copy original mask into extended mask, and convert to ones and zeros.
  extendedMask(rseq, cseq)= (mask != P(0));
  // Return extended mask (copy of handle elided by C++-17).
  return extendedMask;
}


void FillBiLin::registerSquare(int r, int c, int bf) {
  int const  top     = r * bf;       // Top    unbinned row.
  int const  lft     = c * bf;       // Left   unbinned column.
  int const  bot     = top + bf - 1; // Bottom unbinned row.
  int const  rgt     = lft + bf - 1; // Right  unbinned column.
  auto const crnrRows= seq(top, bot, bf - 1);
  auto const crnrCols= seq(lft, rgt, bf - 1);
  auto const edgeRows= seq(top + 1, bot - 1);
  auto const edgeCols= seq(lft + 1, rgt - 1);
  // Write weights for corners.
  weights_.top()(crnrRows, crnrCols)= +1;
  weights_.bot()(crnrRows, crnrCols)= +1;
  weights_.lft()(crnrRows, crnrCols)= +1;
  weights_.rgt()(crnrRows, crnrCols)= +1;
  weights_.cen()(crnrRows, crnrCols)= -4;
  // Write weights for vertical edges.
  weights_.top()(edgeRows, crnrCols);
  weights_.bot()(edgeRows, crnrCols);
  weights_.lft()(edgeRows, crnrCols);
  weights_.rgt()(edgeRows, crnrCols);
  weights_.cen()(edgeRows, crnrCols);
  // Write weights for horizontal edges.
  weights_.top()(crnrRows, edgeCols);
  weights_.bot()(crnrRows, edgeCols);
  weights_.lft()(crnrRows, edgeCols);
  weights_.rgt()(crnrRows, edgeCols);
  weights_.cen()(crnrRows, edgeCols);
  // Add corner and size for current square.
  corners_.row(numSquares_) << top, lft, bf;
  ++numSquares_;
}


// Maximum number of corners is h*w/16 because smallest square has 16 pixels.
template<typename P>
FillBiLin::FillBiLin(P const *msk, int w, int h, int stride):
    weights_(h, w), corners_(h * w / 16, 3) {

  ArrayXX<bool> const m0= extendMask(msk, stride);
  std::cout << "w=" << w << " "
            << "h=" << h << " "
            << "w0=" << m0.cols() << " "
            << "h0=" << m0.rows() << " " << std::endl;
  if(m0.rows() < 2 || m0.cols() < 2) {
    std::cerr << "FillBilLin: ERROR: m0 too small" << std::endl;
    return;
  }

  ArrayXX<bool> const m1= impl::bin2x2(m0);
  std::cout << "w1=" << m1.cols() << " "
            << "h1=" << m1.rows() << " " << std::endl;
  if(m1.rows() < 2 || m1.cols() < 2) {
    std::cerr << "FillBilLin: ERROR: m1 too small" << std::endl;
    return;
  }

  ArrayXX<bool> const mv2= binMask(m1, 4);
  std::cout << "mv2=\n" << mv2 << std::endl;
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_BILIN_HPP

// EOF
