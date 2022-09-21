/// \file       include/dirichlet/impl/ImageHelper.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
///
/// \brief      Definition of
///               dirichlet::impl::ImageHelper,
///               dirichlet::impl::Image,
///               dirichlet::impl::Stride,
///               dirichlet::impl::ImageMap.

#ifndef DIRICHLET_IMPL_IMAGE_HPP
#define DIRICHLET_IMPL_IMAGE_HPP

#include <eigen3/Eigen/Dense> // Array, Dynamic, Map, RowMajor, Unaligned

namespace dirichlet::impl {


using Eigen::Array;
using Eigen::Dynamic;
using Eigen::Map;
using Eigen::RowMajor;
using Eigen::Unaligned;


/// Define type for Image when `P` pixel-value type is non-const type.
/// \tparam P  Type of each pixel-value in image.
template<typename P> struct ImageHelper {
  /// Ordinary Array containing pixel-values of type `P`.
  using Type= Array<P, Dynamic, Dynamic, RowMajor>;
};


/// Specialization of ImageHelper for const type of pixel-value.
/// \tparam P  Non-const version of pixel-value type.
template<typename P> struct ImageHelper<P const> {
  /// Const Array containing pixel-values of type `P`.
  using Type= typename ImageHelper<P>::Type const;
};


/// Type of logical, single-component image.
///
/// "Logical" is used because a single-component image might have
/// a non-compact layout in memory, with non-unit stride.
///
/// When `P` is a const type, Image is a const Array<P>; otherwise, Image is
/// just an Array<P>.
///
/// \tparam P  Type of each pixel-value in image.
///
template<typename P> using Image= typename ImageHelper<P>::Type;


/// Allow dynamic inner stride.
struct Stride: public Eigen::Stride<1, Dynamic> {
  /// Initialize inner stride.
  /// \param s  Inner stride.
  Stride(int s): Eigen::Stride<1, Dynamic>(1, s) {}
};


/// Map used to present image as Image<P>.
/// \tparam P  Type of each pixel-value in image.
template<typename P> using ImageMap= Map<Image<P>, Unaligned, Stride>;


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_IMAGE_HPP

// EOF
