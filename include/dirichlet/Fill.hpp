/// \file       include/DirichletFill.hpp
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
  /// \param coords    Row-column pairs, each pair holding rectangular
  ///                  coordinates of pixel to be filled according to
  ///                  Dirichlet-problem.  `coords(i,0)` holds row-offset of
  ///                  `i`th pixel to be filled, and `coords(i,1)` holds
  ///                  column-offset.
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
  ///                  pointed to by `image` should be `width*height*numComps`.
  ///
  /// \param height    Number of rows in source-image.  Number of elements
  ///                  pointed to by `image` should be `width*height*numComps`.
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
  /// Each column in solution corresponds to one color-component of pixel.
  /// Order of columns in solution is same as order of components within pixel
  /// of `image` as specified in call to constructor.  If image be gray, then
  /// solution has only one column.
  ///
  /// Each row in solution specifies coordinates of filled pixel. Order of rows
  /// in solution is same as order of rows in `coords` as specified in call to
  /// constructor.
  ///
  /// What is stored in each element of solution is value of pixel's component
  /// satisfying zero value for Laplacian of component at location of
  /// component.  Each column in solution corresponds to different component of
  /// pixel, and each row in solution corresponds to different location on
  /// pixel-grid.
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

#include <iostream> // cout, cerr, endl

namespace dirichlet {


using Eigen::ArrayXi;
using std::cerr;
using std::endl;


template<typename C>
Fill::Fill(
      ArrayX2i const &coords,
      C              *image,
      unsigned        width,
      unsigned        height,
      unsigned        numComps) {
  ArrayXi map(height, width);
  for(unsigned r= 0; r < coords.rows(); ++r) {
    int const row= coords(r, 0);
    int const col= coords(r, 1);
    if(col < 0 || col >= width) {
      cerr << "Fill::Fill: col=" << col << " out of bounds" << endl;
      continue;
    }
    if(row < 0 || row >= height) {
      cerr << "Fill::Fill: row=" << row << " out of bounds" << endl;
      continue;
    }
    // TBS
  }
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
