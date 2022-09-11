/// \file       include/dirichlet/Coords.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Coords.

#ifndef DIRICHLET_COORDS_HPP
#define DIRICHLET_COORDS_HPP

#include <eigen3/Eigen/Dense> // ArrayX2i

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


/// Type used by Fill for receiving row-column pairs, each holding rectangular
/// coordinates of pixel to be filled in (as hole might be filled in) according
/// to Dirichlet-problem.  If `coords` be instance of type `%Coords`, then
///
/// - `coords(i,0)` holds offset of row    of `i`th pixel to be filled, and
/// - `coords(i,1)` holds offset of column of `i`th pixel to be filled.
///
/// Every pixel specified by `coords` must be in interior of image; no pixel
/// specified in `coords` may be at edge of image.  If region that must be
/// filled in extend to edge of image, then pixels along edge must first be
/// filled in by other means, and, after that, Fill may be instantiated to fill
/// in interior holes.
using Coords= Eigen::ArrayX2i;


} // namespace dirichlet

#endif // ndef DIRICHLET_COORDS_HPP

// EOF

