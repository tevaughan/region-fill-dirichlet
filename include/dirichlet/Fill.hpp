/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include "Coords.hpp" // Coords, Eigen/Dense

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayXi;
using Eigen::ArrayXXf;
using Eigen::ArrayXXi;
using Eigen::Map;
using Eigen::VectorXf;


/// Fill one or more holes of any size in image by solving Dirichlet-problem
/// for zero-valued Laplacian across specified hole-pixels in image.  Every
/// pixel specified as to be filled must be in interior of image; no such pixel
/// may be at edge of image.  See documentation for Coords.
class Fill {
  Coords const &coords_; ///< Coordinates of each filled pixel.
  unsigned      wdth_;   ///< Number of columns in image.
  unsigned      hght_;   ///< Number of rows    in image.

  /// Map from rectangular coordinates of filled pixel to offset of same
  /// coordinates in value returned by coords().  See documentation for
  /// coordsMap().
  ArrayXXi coordsMap_;

  /// coordsMap_ as single column.
  /// \return  coordsMap_ as single column.
  auto cmCol() const {
    return Map<ArrayXi const>(&coordsMap_(0, 0), wdth_ * hght_, 1);
  }

public:
  /// Prepare to fill one or more single-component images of size
  /// `width*height`.
  ///
  /// Values to fill with are calculated according to Dirichlet-problem for
  /// filling pixels `P`, specified by `coords`, on basis of pixels `B` on
  /// boundary of P.
  ///
  /// Every pixel specified in `coords` must lie in interior of image; every
  /// pixel on edge of image can serve as boundary-condition but cannot be
  /// filled.  If any pixel specified in `coords` be out of bounds, then no
  /// solution is computed.  See documentation for Coords.
  ///
  /// \param coords  Coordinates of each pixel to be Dirichlet-filled.
  /// \param width   Number of columns in image.
  /// \param height  Number of rows in image.
  ///
  Fill(Coords const &coords, unsigned width, unsigned height);

  /// Coordinates of each filled pixel.
  /// \return  Coordinates of each filled pixel.
  Coords const &coords() const { return coords_; }

  /// Solution to linear system for one component of image; if image be gray
  /// (and so have only one component), the solution is complete solution for
  /// image.
  ///
  /// Each row in solution corresponds to different pixel.  Order of rows in
  /// solution is same as order of rows in `coords` as specified in call to
  /// constructor.
  ///
  /// What is stored in each element of solution is value of pixel's component
  /// satisfying zero value for Laplacian of component at location of
  /// component.
  ///
  /// If template-parameter `Comp` be non-const type, then solution is not just
  /// returned but also (converted to `Comp` if necessary and) copied back into
  /// image.
  ///
  /// Solution is empty if any specified coordinates be out of bounds or in
  /// corder of image.
  ///
  /// \tparam Comp   Type of each component in image.
  ///
  /// \param image   Pointer to image with holes.  One component is separated
  ///                from next in memory by `stride`, which is 1 by
  ///                default. `stride` might usually be same as number of
  ///                components per pixel.  Image must be arranged in memory so
  ///                that, except at end of row, columns advance for subsequent
  ///                components while row stays constant (row-major order).
  ///                Number of elements pointed to by `image` should be at
  ///                least `width*height*stride`.
  ///
  /// \param stride  Number of instances of type Comp between component of one
  ///                pixel and corresponding component of next pixel.
  ///
  /// \return        Solution to linear system.
  ///
  template<typename Comp>
  VectorXf operator()(Comp *image, unsigned stride= 1) const;

  /// Map from rectangular coordinates of filled pixel to offset of row in
  /// value returned by coords().
  ///
  /// Returned matrix has `height` rows and `width` columns.  Each element has
  /// value -1 except at coordinates in solution.  If coordinates be in
  /// solution, then element contains offset coordinates in value returned by
  /// coords().
  ///
  /// Array is empty if any specified coordinates be out of bounds or in corder
  /// of image.
  ///
  /// \return  Map from rectangular coordinates of filled pixel to offset of
  ///          same coordinates in value returned by coords().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }

  /// Expression-template for column containing value of left neighbor in
  /// array returned by coordsMap().
  ///
  /// Element `e` at offset `i` in returned column corresponds to filled
  /// pixel's coordinates at same offset `i` in `coords`.  If `e = -1`, then
  /// neighbor is on boundary of hole; otherwise, neighboring filled pixel has
  /// coordinates at offset `e` in `coords`.
  ///
  /// \return  Column whose height is same as height of `coords`.
  ///
  auto nLft() const {
    return cmCol()(coords_.col(0) + (coords_.col(1) - 1) * hght_);
  }

  /// Expression-template for column containing value of right neighbor in
  /// array returned by coordsMap().
  ///
  /// Element `e` at offset `i` in returned column corresponds to filled
  /// pixel's coordinates at same offset `i` in `coords`.  If `e = -1`, then
  /// neighbor is on boundary of hole; otherwise, neighboring filled pixel has
  /// coordinates at offset `e` in `coords`.
  ///
  /// \return  Column whose height is same as height of `coords`.
  ///
  auto nRgt() const {
    return cmCol()(coords_.col(0) + (coords_.col(1) + 1) * hght_);
  }

  /// Expression-template for column containing value of top neighbor in
  /// array returned by coordsMap().
  ///
  /// Element `e` at offset `i` in returned column corresponds to filled
  /// pixel's coordinates at same offset `i` in `coords`.  If `e = -1`, then
  /// neighbor is on boundary of hole; otherwise, neighboring filled pixel has
  /// coordinates at offset `e` in `coords`.
  ///
  /// \return  Column whose height is same as height of `coords`.
  ///
  auto nTop() const {
    return cmCol()(coords_.col(0) - 1 + coords_.col(1) * hght_);
  }

  /// Expression-template for column containing value of bottom neighbor in
  /// array returned by coordsMap().
  ///
  /// Element `e` at offset `i` in returned column corresponds to filled
  /// pixel's coordinates at same offset `i` in `coords`.  If `e = -1`, then
  /// neighbor is on boundary of hole; otherwise, neighboring filled pixel has
  /// coordinates at offset `e` in `coords`.
  ///
  /// \return  Column whose height is same as height of `coords`.
  ///
  auto nBot() const {
    return cmCol()(coords_.col(0) + 1 + coords_.col(1) * hght_);
  }
};


} // namespace dirichlet

// Implementation below.

#include "impl/coordsGood.hpp" // coordsGood()
#include "impl/initCoords.hpp" // initCoords()

namespace dirichlet {


using impl::coordsGood;
using impl::initCoords;


Fill::Fill(Coords const &coords, unsigned width, unsigned height):
    coords_(coords), wdth_(width), hght_(height) {
  if(!coordsGood(coords, width, height)) return;
  coordsMap_= initCoords(coords, width, height);
  // TBS
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
