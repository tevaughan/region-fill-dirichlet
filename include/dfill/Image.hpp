/// \file       include/dfill/Image.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dfill::Image.

#ifndef DFILL_IMAGE_HPP
#define DFILL_IMAGE_HPP

#include <eigen3/Eigen/Dense> // ArrayX2i, ArrayXf

/// Namespace for code implementing Dirichlet-filling of certain pixels
/// specified in image.
namespace dfill {


using Eigen::ArrayX2i;
using Eigen::ArrayXf;


/// Solve Dirichlet-problem for specified pixels in image.
class Image {
  /// Solution to linear system.
  ///
  /// Each column in solution specifies component of filled pixel, and order of
  /// columns in solution is same as order of components within pixel of
  /// `image` as specified in call to constructor.
  ///
  /// Each row in solution specifies coordinates of filled pixel, and order of
  /// rows in solution is same as order of rows in `coords` as specified in
  /// call to constructor.
  ///
  /// Stored in element of solution is filled value of component corresponding
  /// to column and at coordinates corresponding to row.
  ///
  /// If template-parameter `C` specified in constructor be non-const type,
  /// then solution is not just stored in `x_` but also (converted to `C` if
  /// necessary and) copied back into source-image specified in constructor.
  ArrayXf x_;

public:
  /// Analyze source-image, pointed to by `image`, and calculate for each
  /// pixel, whose location is specified in `coords`, value with which pixel
  /// should be filled in, according to Dirichlet-problem.
  ///
  /// \tparam C           Type of each component of each pixel pointed to by
  ///                     `image`.  If `C` be non-const type, then solution is
  ///                     (converted to `C` if necessary and) copied back into
  ///                     source-image.
  ///
  /// \param coords       Column-row pairs, each pair holding rectangular
  ///                     coordinates of pixel to be filled according to
  ///                     Dirichlet-problem.  `coords(i,0)` holds column-offset
  ///                     of `i`th pixel to be filled, and `coords(i,1)` holds
  ///                     row-offset.
  ///
  /// \param image        Pointer to source-image to be analyzed.  Number of
  ///                     components per pixel must be same as `numComps`,
  ///                     which is one by default.  Pixels should be arranged
  ///                     in memory so that, except at end of row, columns
  ///                     advance for subsequent elements while row stays
  ///                     constant (row-major order).  Components within pixel
  ///                     advance fastest of all.  Number of elements pointed
  ///                     to by `image` should be
  ///                     `imageWidth*imageHeight*numComps`.
  ///
  /// \param imageWidth   Number of columns in source-image.  Number of
  ///                     elements pointed to by `image` should be
  ///                     `imageWidth*imageHeight*numComps`.
  ///
  /// \param imageHeight  Number of rows in source-image.  Number of elements
  ///                     pointed to by `image` should be
  ///                     `imageWidth*imageHeight*numComps`.
  ///
  /// \param numComps     Number of components per pixel.  By default, one.
  template<typename C>
  Image(ArrayX2i const &coords,
        C              *image,
        unsigned        imageWidth,
        unsigned        imageHeight,
        unsigned        numComps= 1);

  /// Solution to linear system.
  ///
  /// Each column in solution specifies component of filled pixel, and order of
  /// columns in solution is same as order of components within pixel of
  /// `image` as specified in call to constructor.
  ///
  /// Each row in solution specifies coordinates of filled pixel, and order of
  /// rows in solution is same as order of rows in `coords` as specified in
  /// call to constructor.
  ///
  /// Stored in element of solution is filled value of component corresponding
  /// to column and at coordinates corresponding to row.
  ///
  /// If template-parameter `C` specified in constructor be non-const type,
  /// then solution is not just stored in `x_` but also (converted to `C` if
  /// necessary and) copied back into source-image specified in constructor.
  ///
  /// \return  Solution to linear system.
  ArrayXf const &x() const { return x_; }
};


} // namespace dfill

#endif // ndef DFILL_IMAGE_HPP

// EOF
