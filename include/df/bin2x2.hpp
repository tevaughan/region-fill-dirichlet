/// \file       bin2x2.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of df::bin2x2().

#pragma once
#include <eigen3/Eigen/Dense> // seq

namespace df {


using Eigen::seq;


/// Perform 2x2-binning on array or array-expression `a`.
///
/// Each superpixel in returned, binned array-expression `b` will contain sum
/// of corresponding 2x2 pixels in `a`.
///
/// Throw exception if `a` have fewer than two rows or two columns.
/// Throw exception if `a` have odd number of rows or columns.
///
/// \tparam A  Type of array to bin.
/// \param  a  Array to bin.
/// \return    Binned array `b`.
///
template<typename A> auto bin2x2(A const &a) {
  auto const nr= a.rows();
  auto const nc= a.cols();
  if(nr < 2) throw "too few rows";
  if(nc < 2) throw "too few columns";
  if(nr != nr / 2 * 2) throw "odd number of rows";
  if(nc != nc / 2 * 2) throw "odd number of columns";
  auto const lft= seq(0, nc - 2, 2);
  auto const rgt= seq(1, nc - 1, 2);
  auto const top= seq(0, nr - 2, 2);
  auto const bot= seq(1, nr - 1, 2);
  return a(top, lft) + a(bot, lft) + a(top, rgt) + a(bot, rgt);
}


} // namespace df

// EOF
