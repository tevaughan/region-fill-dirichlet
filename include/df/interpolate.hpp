/// \file       interpolate.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of df::interpolate().

#pragma once

#include <eigen3/Eigen/Dense> // ArrayXf
#include <iostream>           // cout

/// Code for fast, multi-grid solution to Dirichlet-fill problem.
namespace df {


using Eigen::VectorXf;
using std::is_same_v;


/// Bilinearly interpolate corner-values over rectangular array of pixels.
///
/// Value given in `crn` is value at outer corner of each corner-pixel.
/// Value returned in `cen` is value at center of each pixel in array.
///
/// \tparam     T    Type of corner-value array; must be 2x2.
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
  auto const  nrow  = cen.rows();
  auto const  ncol  = cen.cols();
  auto const  rowCrd= VectorXf::LinSpaced(nrow, 0.5f, nrow - 0.5f);
  auto const  colCrd= VectorXf::LinSpaced(ncol, 0.5f, ncol - 0.5f).transpose();
  std::cout << "rowCrd=\n"
            << rowCrd.transpose() << "\n"
            << "colCrd=\n"
            << colCrd << std::endl;
  float const tl    = crn(0, 0);
  float const bl    = crn(1, 0);
  float const tr    = crn(0, 1);
  float const br    = crn(1, 1);
  float const inrow = 1.0f / nrow;
  float const incol = 1.0f / ncol;
  float const ml    = (bl - tl) * inrow; // Slope down left  edge.
  float const mr    = (br - tr) * inrow; // Slope down right edge.
  auto const  ones  = VectorXf::Ones(nrow);
  auto const  lft= tl * ones + ml * rowCrd; // Lft edg val at cen of each row.
  auto const  rgt= tr * ones + mr * rowCrd; // Rgt edg val at cen of each row.
  std::cout << "lft=\n"
            << lft.transpose() << "\n"
            << "rgt=\n"
            << rgt.transpose() << std::endl;
  auto const  m  = (rgt - lft) * incol;     // Different slope across each row.
  auto const  i  = lft.replicate(1, ncol) + m * colCrd; // Interpolant.
  if constexpr(is_same_v<typename U::Scalar, float>) {
    cen= i;
  } else {
    cen= i.template cast<typename U::Scalar>();
  }
}


} // namespace df

// EOF
