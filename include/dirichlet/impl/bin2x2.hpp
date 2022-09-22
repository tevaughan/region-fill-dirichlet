/// \file       include/dirichlet/impl/bin2x2.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::bin2x2().

#ifndef DIRICHLET_FILL_IMPL_BIN2X2_HPP
#define DIRICHLET_FILL_IMPL_BIN2X2_HPP

#include <eigen3/Eigen/Dense> // seq

namespace dirichlet::impl {


using Eigen::seq;


/// Perform logical 2x2 binning on array `a`.
///
/// Each element of `a` is boolean, and each element in returned array is true
/// only if every element in corresponding 2x2 in `a` be true.
///
/// Throw exception if `a` have not both positive, even number of rows and
/// positive, even number of columns.
///
/// \tparam T  Type of either array of bool or equivalent expression-template.
/// \param  a  Array to bin.
/// \return    Expression-template for binned array.
///
template<typename T> auto bin2x2(T const &a) {
  int const nr= a.rows();
  int const nc= a.cols();
  if(nr < 2) throw "too few rows";
  if(nc < 2) throw "too few cols";
  if(nr != nr / 2 * 2) throw "number of rows not even";
  if(nc != nc / 2 * 2) throw "number of cols not even";
  auto const lft= seq(0, nc - 2, 2);
  auto const rgt= seq(1, nc - 1, 2);
  auto const top= seq(0, nr - 2, 2);
  auto const bot= seq(1, nr - 1, 2);
  return a(top, lft) && a(bot, lft) && a(top, rgt) && a(bot, rgt);
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_BIN2X2_HPP

// EOF
