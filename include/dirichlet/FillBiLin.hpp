/// \file       include/dirichlet/FillBiLin.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::FillBiLin.

#ifndef DIRICHLET_FILL_BILIN_HPP
#define DIRICHLET_FILL_BILIN_HPP

#include "eigen3/Eigen/Dense" // ArrayXXi

namespace dirichlet {


using Eigen::ArrayXXi;


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
  int w_; ///< Width  of image.
  int h_; ///< Height of image.

  /// Extend mask so that it is power of two along each dimension.
  /// \tparam P       Type of each pixel-value in mask.
  /// \param  msk     Pointer to first pixel of row-major mask-image.
  /// \param  stride  Pointer-increments between consecutive pixels.
  /// \return         Extended mask.
  template<typename P> ArrayXXi extendMask(P const *msk, int stride);

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
  int w() const { return w_; }

  /// Height of image.
  /// \return  Height of image.
  int h() const { return h_; }
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


template<typename P> ArrayXXi FillBiLin::extendMask(P const *msk, int stride) {
  using impl::pow2;
  // Allocate space for extended mask.
  ArrayXXi extendedMask= ArrayXXi::Zero(pow2(h_), pow2(w_));
  // Map mask to logical image.
  impl::ImageMap<P const> mask(msk, h_, w_, impl::Stride(stride));
  // Initialize range of rows and columns for copying.
  auto const rseq= seq(0, h_ - 1);
  auto const cseq= seq(0, w_ - 1);
  // Copy original mask into extended mask, and convert to ones and zeros.
  extendedMask(rseq, cseq)= (mask != P(0)).template cast<int>();
  // Return extended mask (copy of handle elided by C++-17).
  return extendedMask;
}


template<typename P>
FillBiLin::FillBiLin(P const *msk, int w, int h, int stride): w_(w), h_(h) {
  ArrayXXi const m0= extendMask(msk, stride);
  std::cout << "w=" << w << " "
            << "h=" << h << " "
            << "w0=" << m0.cols() << " "
            << "h0=" << m0.rows() << std::endl;
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_BILIN_HPP

// EOF
