/// \file       interpolate.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of df::interpolate().

#pragma once
#include <eigen3/Eigen/Dense> // VectorXf

/// Code for fast, multi-grid solution to Dirichlet-fill problem.
namespace df {


using Eigen::VectorXf;
using std::is_integral_v;
using std::is_same_v;


/// Bilinearly interpolate corner-values over rectangular array of pixels.
///
/// Value given in `crn` is value at outer corner of each corner-pixel.
/// Value returned in `cen` is value at center of each pixel in array.
///
/// \tparam     T    Type of corner-value array; throw if not 2x2.
/// \tparam     U    Type of rectangular array (may of course be square!).
///
/// \param[in]  crn  Value at outer corner of each corner-pixel of rectangular
///                  pixel-array.
///
/// \param[out] cen  Interpolated from corner-values, value at center of each
///                  pixel in array.
///
template<typename T, typename U> void interpolate(T const &crn, U &cen) {
  if(crn.rows() != 2 || crn.cols() != 2) throw "crnVals not 2x2";
  auto const nrow  = cen.rows();
  auto const ncol  = cen.cols();
  auto const rowCrd= VectorXf::LinSpaced(nrow, 0.5f, nrow - 0.5f);
  auto const colCrd= VectorXf::LinSpaced(ncol, 0.5f, ncol - 0.5f).transpose();
  auto const ml    = (crn(1, 0) - crn(0, 0)) * 1.0f / nrow;
  auto const mr    = (crn(1, 1) - crn(0, 1)) * 1.0f / nrow;
  auto const lft   = crn(0, 0) * VectorXf::Ones(nrow) + ml * rowCrd;
  auto const rgt   = crn(0, 1) * VectorXf::Ones(nrow) + mr * rowCrd;
  auto const m     = (rgt - lft) * 1.0f / ncol;
  auto const i     = lft.replicate(1, ncol) + m * colCrd;
  /**/ using S     = typename U::Scalar;
  if constexpr(is_same_v<S, float>) { // float
    cen= i;
  } else if constexpr(is_integral_v<S>) { // int8_t,  int16_t, etc.
    auto const negativ= (i.array() < 0.0f).template cast<int>();
    auto const positiv= 1 - negativ;
    auto const roundDn= (i.array() - 0.5f).template cast<S>();
    auto const roundUp= (i.array() + 0.5f).template cast<S>();
    /*      */ cen    = negativ * roundDn + positiv * roundUp;
  } else { // double, long double
    cen= i.template cast<S>();
  }
}


} // namespace df

// EOF
