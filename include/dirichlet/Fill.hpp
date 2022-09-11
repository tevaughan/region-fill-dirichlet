/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include "Coords.hpp" // Coords, Eigen/Dense

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayXXf; // For solution.
using Eigen::ArrayXXi; // For coordinates-map.


/// Fill one or more holes of any size in image by solving Dirichlet-problem
/// for zero-valued Laplacian across specified hole-pixels in image.  Every
/// pixel specified as to be filled must be in interior of image; no such pixel
/// may be at edge of image.  See documentation for Coords.
class Fill {
  /// Solution to linear system.  See documentation for x().
  ArrayXXf x_;

  /// Map from rectangular coordinates of filled pixel to offset of row in
  /// matrix returned by x().  See documentation for coordsMap().
  ArrayXXi coordsMap_;

  /// For each coordinate-pair indicating pixel to be filled, value for each of
  /// left-, right-, top-, and bottom-neighbors in matrix returned by
  /// coordsMap().  See documentation for lrtb().
  ArrayXXi lrtb_;

public:
  /// Analyze image, and, for each pixel whose location is specified in
  /// `coords`, calculate value with which pixel should be filled.
  ///
  /// Values to fill with are calculated according to Dirichlet-problem for
  /// filling pixels P, specified by `coords`, on basis of pixels B on boundary
  /// of P.
  ///
  /// Every pixel specified in `coords` must lie in interior of image; every
  /// pixel on edge of image can serve as boundary-condition but cannot be
  /// filled.  If any pixel specified in `coords` be out of bounds, then no
  /// solution is computed.  See documentation for Coords.
  ///
  /// \tparam C        Type of each component of each pixel pointed to by
  ///                  `image`.  If `C` be non-const type, then solution is
  ///                  (converted to `C` if necessary and) copied back into
  ///                  image.
  ///
  /// \param coords    Coordinates of each pixel to be Dirichlet-filled.
  ///
  /// \param image     Pointer to image to be analyzed.  Number of components
  ///                  per pixel must be same as `numComps`, which is one by
  ///                  default.  Pixels should be arranged in memory so that,
  ///                  except at end of row, columns advance for subsequent
  ///                  elements while row stays constant (row-major order).
  ///                  Components within pixel advance fastest of all.  Number
  ///                  of elements pointed to by `image` should be
  ///                  `width*height*numComps`.
  ///
  /// \param width     Number of columns in image.  Number of elements pointed
  ///                  to by `image` should be `width*height*numComps`.
  ///
  /// \param height    Number of rows in image.  Number of elements pointed to
  ///                  by `image` should be `width*height*numComps`.
  ///
  /// \param numComps  Number of components per pixel.  By default, one.
  ///
  template<typename C>
  Fill(Coords const &coords,
       C            *image,
       unsigned      width,
       unsigned      height,
       unsigned      numComps= 1);

  /// Solution to linear system.
  ///
  /// Each column in solution corresponds to one color-component of pixel.
  /// Order of columns in solution is same as order of components within pixel
  /// of `image` as specified in call to constructor.  If image be gray, then
  /// solution has only one column.
  ///
  /// Each row in solution corresponds to different pixel.  Order of rows in
  /// solution is same as order of rows in `coords` as specified in call to
  /// constructor.
  ///
  /// What is stored in each element of solution is value of pixel's component
  /// satisfying zero value for Laplacian of component at location of
  /// component.  Each column in solution corresponds to different component of
  /// pixel, and each row in solution corresponds to different pixel.
  ///
  /// If template-parameter `C`, as specified in constructor, be non-const
  /// type, then solution is not just stored in instance but also (converted to
  /// `C` if necessary and) copied back into image specified in constructor.
  ///
  /// Solution is empty if any specified coordinates be out of bounds or in
  /// corder of image.
  ///
  /// \return  Solution to linear system.
  ///
  ArrayXXf const &x() const { return x_; }

  /// Map from rectangular coordinates of filled pixel to offset of row in
  /// matrix returned by x().
  ///
  /// Returned matrix has `height` rows and `width` columns.  Each element has
  /// value -1 except at coordinates in solution.  If coordinates be in
  /// solution, then element contains offset, both of corresponding row in
  /// matrix returned by x() and of corresponding row in matrix `coords` as
  /// specified in call to constructor.
  ///
  /// Array is empty if any specified coordinates be out of bounds or in corder
  /// of image.
  ///
  /// \return  Map from rectangular coordinates of filled pixel to offset of
  ///          row in matrix returned by x().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }

  /// For each coordinate-pair indicating pixel to be filled, value for each of
  /// left-, right-, top-, and bottom-neighbors in matrix returned by
  /// coordsMap().
  ///
  /// Matrix returned by %lrtb() has four columns.  In matrix returned by
  /// coordsMap(), if element at `(r,c)` contain value `i` (rather than -1),
  /// then columns in row `i` of matrix returned by %lrtb() contain values,
  /// respectively, of elements at `(r,c-1)`, `(r,c+1)`, `(r-1,c)`, and
  /// `(r+1,c)` in matrix returned by coordsMap().
  ///
  /// If value in some element of matrix returned by %lrtb be -1, then
  /// corresponding pixel is on boundary of hole.
  ///
  /// \return  Value for each of left-, right-, top-, and bottom-neighbors in
  ///          matrix returned by coordsMap().
  ArrayXXi const &lrtb() const { return lrtb_; }
};


} // namespace dirichlet

// Implementation below.

#include "impl/coordsGood.hpp" // coordsGood()
#include "impl/initCoords.hpp" // initCoords()
#include "impl/initLrtb.hpp"   // initLrtb()


namespace dirichlet {


using impl::coordsGood;
using impl::initCoords;
using impl::initLrtb;


template<typename C>
Fill::Fill(
      Coords const &coords,
      C            *image,
      unsigned      width,
      unsigned      height,
      unsigned      numComps) {
  if(!coordsGood(coords, width, height)) return;
  coordsMap_= initCoords(coords, width, height);
  lrtb_     = initLrtb(coords, coordsMap_);
  // TBS
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
