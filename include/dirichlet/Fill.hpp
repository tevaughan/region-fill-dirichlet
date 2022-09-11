/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Coords, dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include <eigen3/Eigen/Dense> // ArrayX2i, ArrayXf, ArrayXXi, ArrayXi, Map

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayXf;  // For solution.
using Eigen::ArrayXXi; // For coordinates-map.


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


/// Fill one or more holes of any size in image by solving Dirichlet-problem
/// for zero-valued Laplacian across specified hole-pixels in image.  Every
/// pixel specified as to be filled must be in interior of image; no such pixel
/// may be at edge of image.  See documentation for Coords.
class Fill {
  /// Solution to linear system.  See documentation for x().
  ArrayXf x_;

  /// Map from rectangular coordinates of pixel in solution to offset in array
  /// returned by x().  See documentation for coordsMap().
  ArrayXXi coordsMap_;

  /// Verify that coordinates support well defined solution.
  ///
  /// Return false if any coordinates be out of bounds.  Every pixel on edge of
  /// image is out of bounds.  See documentation for Coords.
  ///
  /// \param coords  Coordinates of each pixel to be Dirichlet-filled.
  /// \param width   Number of columns in image.
  /// \param height  Number of rows    in image.
  /// \return        True only if coordinates support well defined solution.
  ///
  static bool
  coordsGood(Coords const &coords, unsigned width, unsigned height);

  /// Compute initializer for value returned by coordsMap().
  ///
  /// Empty matrix is returned if any coordinates be out of bounds or in corner
  /// of image.  Every pixel on edge of image is out of bounds.  See
  /// documentation for Coords.
  ///
  /// \param coords  Coordinates of each pixel to be Dirichlet-filled.
  /// \param width   Number of columns in image.
  /// \param height  Number of rows    in image.
  /// \return        Initializer for array returned by coordsMap().
  ///
  static ArrayXXi
  initCoords(Coords const &coords, unsigned width, unsigned height);

public:
  /// Analyze image, and, for each pixel whose location is specified in
  /// `coords`, calculate value with which pixel should be filled, according to
  /// Dirichlet-problem for `coords`.
  ///
  /// Every pixel specified in `coords` must lie in interior of image; every
  /// pixel on edge of image is out of bounds.  If any pixel specified in
  /// `coords` be out of bounds, then no solution is computed.  See
  /// documentation for Coords.
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
  /// Array is empty if any specified coordinates be out of bounds or in corder
  /// of image.
  ///
  /// \return  Map from rectangular coordinates of pixel in solution to offset
  ///          of row in matrix returned by x().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }
};


} // namespace dirichlet

// Implementation below.

#include <iostream>

namespace dirichlet {


using Eigen::ArrayXi;
using Eigen::Map;
using std::cerr;
using std::endl;


inline bool
Fill::coordsGood(Coords const &coords, unsigned width, unsigned height) {
  auto roob= (coords.col(0) <= 0) || (coords.col(0) >= height - 1);
  auto coob= (coords.col(1) <= 0) || (coords.col(1) >= width - 1);
  if((roob || coob).cast<int>().sum() > 0) {
    char const w[]= "Fill::coordsGood: WARNING: ";
    for(int i= 0; i < roob.size(); ++i) {
      int const r= coords(i, 0);
      int const c= coords(i, 1);
      if(roob[i]) cerr << w << "r(" << i << ")=" << r << " OOB" << endl;
      if(coob[i]) cerr << w << "c(" << i << ")=" << c << " OOB" << endl;
    }
    return false;
  }
  return true;
}


inline ArrayXXi
Fill::initCoords(Coords const &coords, unsigned width, unsigned height) {
  if(!coordsGood(coords, width, height)) return ArrayXXi();
  ArrayXXi      cmap(ArrayXXi::Constant(height, width, -1));
  ArrayXi const lin= coords.col(0) + coords.col(1) * height;
  Map<ArrayXi>  m(&cmap(0, 0), height * width, 1);
  m(lin)= ArrayXi::LinSpaced(coords.rows(), 0, coords.rows() - 1);
  return cmap;
}


template<typename C>
Fill::Fill(
      Coords const &coords,
      C            *image,
      unsigned      width,
      unsigned      height,
      unsigned      numComps):
    coordsMap_(initCoords(coords, width, height)) {
  if(coordsMap_.rows() == 0) return;
  // TBS
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
