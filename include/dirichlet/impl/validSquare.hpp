/// \file       include/dirichlet/impl/validSquare.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::validSquare().

#ifndef DIRICHLET_FILL_IMPL_VALID_SQUARE_HPP
#define DIRICHLET_FILL_IMPL_VALID_SQUARE_HPP

#include <eigen3/Eigen/Dense> // ArrayXX, seq

namespace dirichlet::impl {


using Eigen::ArrayXX;
using Eigen::seq;


/// Look for valid squares in binned boolean array `a`.
///
/// Each element of `a` is boolean, and each element in returned array is true
/// only if every one of the four neighbors of the element be true.
///
/// \param  a  Array to test.
/// \return    Result of test.
///
inline ArrayXX<bool> validSquare(ArrayXX<bool> const &a) {
  auto const    nr  = a.rows();
  auto const    nc  = a.cols();
  ArrayXX<bool> b   = ArrayXX<bool>::Zero(nr + 2, nc + 2);
  auto const    cenr= seq(1, nr);
  auto const    cenc= seq(1, nc);
  b(cenr, cenc)     = a;
  auto const topr   = seq(0, nr - 1);
  auto const botr   = seq(2, nr + 1);
  auto const lftc   = seq(0, nc - 1);
  auto const rgtc   = seq(2, nc + 1);
  auto const top    = b(topr, cenc);
  auto const bot    = b(botr, cenc);
  auto const lft    = b(cenr, lftc);
  auto const rgt    = b(cenr, rgtc);
  auto const cen    = b(cenr, cenc);
  return top && bot && lft && rgt && cen;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_VALID_SQUARE_HPP

// EOF

