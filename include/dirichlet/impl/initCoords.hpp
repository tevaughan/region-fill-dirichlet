/// \file       include/dirichlet/impl/initCoords.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::initCoords().

#ifndef DIRICHLET_IMPL_INIT_COORDS_HPP
#define DIRICHLET_IMPL_INIT_COORDS_HPP

#include "../Coords.hpp" // Coords and classes in Eigen/Dense

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet::impl {


using Eigen::ArrayXi;
using Eigen::ArrayXXi;
using Eigen::Map;


/// Compute initializer for value returned by Fill::coordsMap().
///
/// \param coords  Coordinates of each pixel to be Dirichlet-filled.
/// \param width   Number of columns in image.
/// \param height  Number of rows    in image.
/// \return        Initializer for array returned by Fill::coordsMap().
///
inline ArrayXXi
initCoords(Coords const &coords, unsigned width, unsigned height) {
  ArrayXXi     cmap= ArrayXXi::Constant(height, width, -1);
  auto const   lin = coords.col(0) + coords.col(1) * height;
  Map<ArrayXi> m(&cmap(0, 0), height * width, 1);
  m(lin)= ArrayXi::LinSpaced(coords.rows(), 0, coords.rows() - 1);
  return cmap;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_INIT_COORDS_HPP

// EOF
