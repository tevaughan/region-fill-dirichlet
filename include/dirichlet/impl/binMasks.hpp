/// \file       include/dirichlet/impl/binMasks.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
///
/// \brief      Definition of
///               dirichlet::impl::binMask().
///               dirichlet::impl::binMasks().

#ifndef DIRICHLET_IMPL_BIN_MASKS_HPP
#define DIRICHLET_IMPL_BIN_MASKS_HPP

#include "eigen3/Eigen/Dense" // ArrayXXi, seq
#include <vector>             // vector

namespace dirichlet::impl {


using Eigen::ArrayXX;
using Eigen::seq;
using std::vector;


/// Perform binning on higher-resolution, power-of-two-sized mask `hi` so that
/// each pixel in lower-resolution returned mask is logical AND of four
/// corresponding pixels in `hi`.
///
/// \param hi  Higher-resolution mask.
///
/// \return    Expression-template for lower-resolution mask (half resolution
///            in each dimension).
template<typename T> auto binMask(T const &hi) {
  int const r= hi.rows();
  int const c= hi.cols();
  if(r != r / 2 * 2) throw "number of rows not even";
  if(c != c / 2 * 2) throw "number of cols not even";
  auto const lft= seq(0, hi.cols() - 2, 2);
  auto const rgt= seq(1, hi.cols() - 1, 2);
  auto const top= seq(0, hi.rows() - 2, 2);
  auto const bot= seq(1, hi.rows() - 1, 2);
  return hi(lft, top) && hi(lft, bot) && hi(rgt, top) && hi(rgt, bot);
}


/// Perform binning on power-of-two mask `m0` so that, at each level of
/// binning, each pixel is logical AND of four corresponding pixels at next
/// higher level of resolution.
///
/// First element is highest-resolution (4x4-binned) mask in list, then lower
/// resolution (8x8), even lower resolution (16x16), etc.  Last element is at
/// least four pixels across in each dimension.
///
/// 2x2-binned mask is not in list because it is not used.
///
/// \param m0  Original, power-of-two-extended, unbinned mask.
/// \return    List of power-of-two-binned versions of `m0`.
vector<ArrayXX<bool>> binMasks(ArrayXX<bool> const &m0) {
  vector<ArrayXX<bool>> r;
  if(m0.rows() < 8 || m0.cols() < 8) return r;
  auto const m1= binMask(m0);
  if(m1.rows() < 8 || m1.cols() < 8) return r;
  r.push_back(binMask(m1)); // m2
  while(true) {
    ArrayXX<bool> const &last= r[r.size() - 1];
    if(last.rows() < 8 || last.cols() < 8) break;
    r.push_back(binMask(last)); // mi
  }
  return r;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_BIN_MASKS_HPP

// EOF
