/// \file       include/dirichlet/FillBiLin.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::FillBiLin.

#ifndef DIRICHLET_FILL_BILIN_HPP
#define DIRICHLET_FILL_BILIN_HPP

#include "impl/Weights.hpp"   // Weights
#include <eigen3/Eigen/Dense> // ArrayX3, ArrayXX, seq

namespace dirichlet {


using Eigen::ArrayX3;
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

  /// Recursive function that performs binning on higher-resolution mask `hi`,
  /// detects valid squares at current binning level, calls itself (if enough
  /// pixels at current binning), modifies `weights_` for corners and edges of
  /// squares to interpolate, increments `numSquares_` as needed, modifies
  /// `corners_` as needed, and returns result of next-lower-resolution
  /// binning.
  ///
  /// \tparam T   Type of array or expression-template for array.
  /// \param  hi  Higher-resolution mask.
  /// \return     Expression-template for end-result at next lower resolution.
  ///
  template<typename T> auto binMask(T const &hi) {
    int const r= hi.rows();
    int const c= hi.cols();
    if(r != r / 2 * 2) throw "number of rows not even";
    if(c != c / 2 * 2) throw "number of cols not even";
    auto const lft= seq(0, c - 2, 2);
    auto const rgt= seq(1, c - 1, 2);
    auto const top= seq(0, r - 2, 2);
    auto const bot= seq(1, r - 1, 2);
    auto lo= hi(lft, top) && hi(lft, bot) && hi(rgt, top) && hi(rgt, bot);
    // TBS:
    // - Define array loValid, with each element set true for corresponding
    //   square that is valid at current level of binning.
    if(r >= 8 && c >= 8 /* && at least one element true in loValid */) {
      auto const lowerValid= binMask(lo);
      // TBS:
      // - In loValid, set false each element with corresponding square
      //   overlapped by valid square in lowerValid.
    }
    // TBS:
    // - For each true element in loValid, mark corners and edges appropriately
    //   in weights_, increment numSquares_, and add row to corners_.
    // - Return loValid.
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

#include "impl/Image.hpp"    // Image, ImageMap
#include "impl/binMasks.hpp" // binMasks(), vector
#include "impl/pow2.hpp"     // pow2()
#include <iostream>          // cout, endl

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


// Maximum number of corners is h*w/16 because smallest square has 16 pixels.
template<typename P>
FillBiLin::FillBiLin(P const *msk, int w, int h, int stride):
    weights_(h, w), corners_(h * w / 16, 3) {
  ArrayXX<bool> const   m0= extendMask(msk, stride);
  vector<ArrayXX<bool>> m = impl::binMasks(m0);
  std::cout << "w=" << w << " "
            << "h=" << h << " "
            << "w0=" << m0.cols() << " "
            << "h0=" << m0.rows() << " "
            << "numMasks=" << m.size() << std::endl;
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_BILIN_HPP

// EOF
