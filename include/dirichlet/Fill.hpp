/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Declaration of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include <eigen3/Eigen/Dense> // ArrayX2i, ArrayXf

/// Namespace for code solving Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayX2i;
using Eigen::ArrayXf;


/// Solve Dirichlet-problem for zero-valued Laplacian across specified pixels
/// in image.
class Fill {
  ArrayXf x_; ///< Solution to linear system.  See documentation for x().

public:
  /// Analyze source-image, pointed to by `image`, and calculate for each
  /// pixel, whose location is specified in `coords`, value with which pixel
  /// should be filled in, according to Dirichlet-problem.
  ///
  /// If pixel to be filled lie on edge of source-image, then only component of
  /// Laplacian parallel to edge is set to zero in constraining pixel's value.
  /// Component of Laplacian perpendicular to edge is ignored.
  ///
  /// If pixel to be filled lie at corner of source-image, then zero is used as
  /// boundary-value at corner.
  ///
  /// \tparam C        Type of each component of each pixel pointed to by
  ///                  `image`.  If `C` be non-const type, then solution is
  ///                  (converted to `C` if necessary and) copied back into
  ///                  source-image.
  ///
  /// \param coords    Column-row pairs, each pair holding rectangular
  ///                  coordinates of pixel to be filled according to
  ///                  Dirichlet-problem. `coords(i,0)` holds column-offset of
  ///                  `i`th pixel to be filled, and `coords(i,1)` holds
  ///                  row-offset.
  ///
  /// \param image     Pointer to source-image to be analyzed.  Number of
  ///                  components per pixel must be same as `numComps`, which
  ///                  is one by default. Pixels should be arranged in memory
  ///                  so that, except at end of row, columns advance for
  ///                  subsequent elements while row stays constant (row-major
  ///                  order).  Components within pixel advance fastest of all.
  ///                  Number of elements pointed to by `image` should be
  ///                  `imageWidth*imageHeight*numComps`.
  ///
  /// \param width     Number of columns in source-image.  Number of elements
  ///                  pointed to by `image` should be
  ///                  `imageWidth*imageHeight*numComps`.
  ///
  /// \param height    Number of rows in source-image.  Number of elements
  ///                  pointed to by `image` should be
  ///                  `imageWidth*imageHeight*numComps`.
  ///
  /// \param numComps  Number of components per pixel.  By default, one.
  template<typename C>
  Fill(ArrayX2i const &coords,
       C              *image,
       unsigned        width,
       unsigned        height,
       unsigned        numComps= 1);

  /// Solution to linear system.
  ///
  /// Each column in solution corresponds to one color-component of pixel, and
  /// order of columns in solution is same as order of components within pixel
  /// of `image` as specified in call to constructor.  If image be gray, then
  /// solution has only one column.
  ///
  /// Each row in solution specifies coordinates of filled pixel, and order of
  /// rows in solution is same as order of rows in `coords` as specified in
  /// call to constructor.
  ///
  /// What is stored in each element of solution is value of component
  /// satisfying zero value for Laplacian across pixels to be filled.
  /// Component of pixel corresponds to column in solution, and coordinates of
  /// pixel correspond to row in solution.
  ///
  /// If template-parameter `C`, as specified in constructor, be non-const
  /// type, then solution is not just stored in instance but also (converted to
  /// `C` if necessary and) copied back into source-image specified in
  /// constructor.
  ///
  /// \return  Solution to linear system.
  ArrayXf const &x() const { return x_; }
};


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
