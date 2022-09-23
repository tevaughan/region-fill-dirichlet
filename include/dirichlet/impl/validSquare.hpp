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
  int const     nr= a.rows();
  int const     nc= a.cols();
  ArrayXX<bool> b = ArrayXX<bool>::Zero(nr, nc);
  if(nr < 2 || nc < 2) throw "a too small";
  auto const    tr= seq(0, nr - 2); // Top     row.
  auto const    br= seq(1, nr - 1); // Bottom  row.
  auto const    lc= seq(0, nc - 2); // Left    column.
  auto const    rc= seq(1, nc - 1); // Right   column.
  auto const    cr= seq(1, nr - 2); // Central row.
  auto const    cc= seq(1, nc - 2); // Central column.
  b(cr, cc)= a(tr, cc) && a(br, cc) && a(cr, lc) && a(cr, rc) && a(cr, cc);
  return b;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_VALID_SQUARE_HPP

// EOF

