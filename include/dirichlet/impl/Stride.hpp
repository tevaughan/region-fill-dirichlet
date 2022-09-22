/// \file       include/dirichlet/impl/Stride.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::Stride.

#ifndef DIRICHLET_IMPL_STRIDE_HPP
#define DIRICHLET_IMPL_STRIDE_HPP

#include <eigen3/Eigen/Dense> // Stride

namespace dirichlet::impl {


using Eigen::Dynamic;


/// Allow dynamic inner stride.
struct Stride: public Eigen::Stride<1, Dynamic> {
  /// Initialize inner stride.
  /// \param s  Inner stride.
  Stride(int s): Eigen::Stride<1, Dynamic>(1, s) {}
};


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_STRIDE_HPP

// EOF

