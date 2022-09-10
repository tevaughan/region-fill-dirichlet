/// \file       include/DirichletFill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Declaration of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include <eigen3/Eigen/Dense> // ArrayX2i, ArrayXf, ArrayXXi, ArrayXi, Map

/// Namespace for code solving Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayX2i; // For list of coordinates.
using Eigen::ArrayXf;  // For solution.
using Eigen::ArrayXXi; // For coordinates-map.


/// Solution to Dirichlet-problem for zero-valued Laplacian across specified
/// pixels in image.
class Fill {
  /// Solution to linear system.  See documentation for x().
  ArrayXf x_;

  /// Map from rectangular coordinates of pixel in solution to offset in array
  /// returned by x().  See documentation for coordsMap().
  ArrayXXi coordsMap_;

  /// Function called by constructor to initialize value returned by
  /// coordsMap().
  ///
  /// \param coords  Row-column pairs, each pair holding rectangular
  ///                coordinates of pixel to be filled according to
  ///                Dirichlet-problem.  `coords(i,0)` holds image-row-offset
  ///                of `i`th pixel to be filled, and `coords(i,1)` holds
  ///                image-column-offset.
  ///
  /// \param width   Number of columns in image.
  /// \param height  Number of rows    in image.
  ///
  /// \return        Initializer for array returned by coordsMap().
  ///
  static ArrayXXi
  initCoords(ArrayX2i const coords, unsigned width, unsigned height);

public:
  /// Analyze image, and, for each pixel whose location is specified in
  /// `coords`, calculate value with which pixel should be filled, according to
  /// Dirichlet-problem for `coords`.
  ///
  /// If pixel specified in `coords` lie on edge of image, then only component
  /// of Laplacian parallel to edge is set to zero in constraining pixel's
  /// value.  Component of Laplacian perpendicular to edge is ignored if pixel
  /// be on edge of image.
  ///
  /// Every pixel specified in `coords` must lie in image and *not* at corner
  /// of image.  Otherwise, behavior is undefined.
  ///
  /// \tparam C        Type of each component of each pixel pointed to by
  ///                  `image`.  If `C` be non-const type, then solution is
  ///                  (converted to `C` if necessary and) copied back into
  ///                  image.
  ///
  /// \param coords    Row-column pairs, each pair holding rectangular
  ///                  coordinates of pixel to be filled according to
  ///                  Dirichlet-problem.  `coords(i,0)` holds image-row-offset
  ///                  of `i`th pixel to be filled, and `coords(i,1)` holds
  ///                  image-column-offset.
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
  Fill(ArrayX2i const &coords,
       C              *image,
       unsigned        width,
       unsigned        height,
       unsigned        numComps= 1);

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
  /// \return  Solution to linear system.
  ///
  ArrayXf const &x() const { return x_; }

  /// Map from rectangular coordinates of pixel in solution to offset of row in
  /// matrix returned by x().
  ///
  /// Returned matrix has `height` rows and `width` columns.  Each element has
  /// value -1 except at coordinates in solution.  If coordinates be in
  /// solution, then element contains offset, both of corresponding row in
  /// matrix returned by x() and of corresponding row in matrix `coords` as
  /// specified in call to constructor.
  ///
  /// \return  Map from rectangular coordinates of pixel in solution to offset
  ///          of row in matrix returned by x().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }
};


} // namespace dirichlet

// Implementation below.

namespace dirichlet {


using Eigen::ArrayXi;
using Eigen::Map;


inline ArrayXXi
Fill::initCoords(ArrayX2i const coords, unsigned width, unsigned height) {
  ArrayXXi      cmap(ArrayXXi::Constant(height, width, -1));
  ArrayXi const lin= coords.col(0) + coords.col(1) * height;
  Map<ArrayXi>  m(&cmap(0, 0), height * width, 1);
  m(lin)= ArrayXi::LinSpaced(coords.rows(), 0, coords.rows() - 1);
  return cmap;
}


template<typename C>
Fill::Fill(
      ArrayX2i const &coords,
      C              *image,
      unsigned        width,
      unsigned        height,
      unsigned        numComps):
    coordsMap_(initCoords(coords, width, height)) {
  // TBS
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
