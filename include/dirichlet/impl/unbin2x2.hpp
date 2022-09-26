/// \file       include/dirichlet/impl/unbin2x2.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::unbin2x2().

#ifndef DIRICHLET_FILL_IMPL_UNBIN2X2_HPP
#define DIRICHLET_FILL_IMPL_UNBIN2X2_HPP

#include <eigen3/Eigen/Dense> // ArrayXi, seq

namespace dirichlet::impl {


using Eigen::ArrayXi;
using Eigen::ArrayXX;
using Eigen::seq;


/// Perform logical 2x2 unbinning on array `a`.
///
/// Each element of `a` is boolean, and every element in 2x2 block of elements
/// in returned array is true whenever corresponding element in `a` be true.
///
/// \param  a  Array to unbin.
/// \return    Unbinned array.
///
auto unbin2x2(ArrayXX<bool> const &a) {
  int const  nr= a.rows();
  int const  nc= a.cols();
  auto const r = ArrayXi::LinSpaced(2 * nr, 0, nr - 1);
  auto const c = ArrayXi::LinSpaced(2 * nc, 0, nc - 1);
  return a(r, c);
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_UNBIN2X2_HPP

// EOF
