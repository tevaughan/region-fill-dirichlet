/// \file       include/dfill/Image.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dfill::Image.

#ifndef DFILL_IMAGE_HPP
#define DFILL_IMAGE_HPP

#include <memory> // unique_ptr

namespace dfill {


using std::unique_ptr;


/// Solve Dirichlet-problem for specified pixels in image.
///
/// Solution is image with pixel-component of type `float`, regardless of type
/// of pixel-component in source-image.
class Image {
  /// Pointer to image-data in which pixels indicated via constructor have been
  /// filled.
  unique_ptr<float> image_;

  /// Number of columns in image.
  unsigned numCols_;

  /// Number of rows in image.
  unsigned numRows_;

  /// Number of components per pixel.
  unsigned numComps_;

public:
  /// Copy source-image, pointed to by `image`, and fill in, according to
  /// Dirichlet-problem, pixels indicated by way of `coords`.
  ///
  /// \tparam C         Type of each component of each pixel pointed to by
  ///                   `image`.
  ///
  /// \param image      Pointer to image-data to be analyzed.  Number of
  ///                   components per pixel must be same as `numComps`, which
  ///                   is one by default.  Pixels should be arranged in memory
  ///                   so that, except at end of row, columns advance while
  ///                   row stays constant (row-major order). Components within
  ///                   pixel advance fastest of all.  Number of elements
  ///                   pointed to by `image` should be
  ///                   `numCols*numRows*numComps`.
  ///
  /// \param numCols    Number of columns in image.  Number of elements pointed
  ///                   to by `image` should be `numCols*numRows*numComps`.
  ///
  /// \param numCols    Number of rows in image.  Number of elements pointed to
  ///                   by `image` should be `numCols*numRows*numComps`.
  ///
  /// \param coords     Pointer to column-row pairs, each pair identifying
  ///                   pixel to be filled according to Dirichlet-problem.  In
  ///                   each pair of `unsigned` elements, column is first, and
  ///                   row is second.  Number of elements pointed to by
  ///                   `coords` should be `numCoords*2`.
  ///
  /// \param numCoords  Number of column-row pairs, each identifying pixel to
  ///                   be filled according to Dirichlet-problem.  Number of
  ///                   elements pointed to by `coords` should be
  ///                   `numCoords*2`.
  ///
  /// \param numComps   Number of components per pixel.  By default, one.
  template<typename C>
  Image(C        *image,
        unsigned  numCols,
        unsigned  numRows,
        unsigned *coords,
        unsigned  numCoords,
        unsigned  numComps= 1);

  /// Pointer to image-data in which pixels indicated via constructor have been
  /// filled.
  ///
  /// Number of components per pixel is same as returned by `numComps()`.
  /// Pixels are arranged in memory so that, except at end of row, columns
  /// advance while row stays constant (row-major order).  Components within
  /// pixel advance fastest of all.  Number of elements pointed to by `image`
  /// is `numCols()*numRows()*numComps()`.
  ///
  /// \return  Pointer to image-data.
  float const *image() const { return image_.get(); }

  /// Number of columns in image.
  unsigned numCols() const { return numCols_; }

  /// Number of rows in image.
  unsigned numRows() const { return numRows_; }

  /// Number of components per pixel.
  unsigned numComps() const { return numComps_; }
};


} // namespace dfill

#endif // ndef DFILL_IMAGE_HPP

// EOF
