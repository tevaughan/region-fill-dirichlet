/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include "Coords.hpp"          // Coords, Eigen/Dense
#include <eigen3/Eigen/Sparse> // SparseMatrix

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayX4i;
using Eigen::ArrayXi;
using Eigen::ArrayXXf;
using Eigen::ArrayXXi;
using Eigen::Map;
using Eigen::SimplicialCholesky;
using Eigen::SparseMatrix;
using Eigen::VectorXf;


/// Fill holes in image by solving Dirichlet-problem for zero-valued Laplacian
/// across specified hole-pixels in image.
///
/// Every pixel specified as to be filled must be in interior of image; no such
/// pixel may be at edge of image.  See documentation for Coords.
///
/// Instantiating `%Fill` does all preparatory work necessary to enable quick
/// filling of same coordinates in one or more images of same size.
///
/// Filling image happens when instance is used as function-object.
class Fill {
  Coords const &coords_; ///< Coordinates of filled pixels in each image.
  unsigned      wdth_;   ///< Number of columns in each image to fill.
  unsigned      hght_;   ///< Number of rows    in each image to fill.

  /// Map from rectangular coordinates of filled pixel to offset of same
  /// coordinates in value returned by coords().  See documentation for
  /// coordsMap().
  ArrayXXi coordsMap_;

  /// Matrix with four columns to encode information about each neighbor of
  /// each filled pixel.  See documentation for lrtb().
  ArrayX4i lrtb_;

  /// Square matrix for linear problem.
  SparseMatrix<float> a_;

  /// Pointer to Cholesky-decomposition of square matrix for linear problem.
  SimplicialCholesky<SparseMatrix<float>> *A_;

  /// Disallow copying because we store owned pointer.
  Fill(Fill const &)= delete;

  /// Disallow assignment because we store owned pointer.
  Fill &operator=(Fill const &)= delete;

  /// coordsMap_ as single column.
  /// \return  coordsMap_ as single column.
  auto cmCol() const {
    return Map<ArrayXi const>(&coordsMap_(0, 0), wdth_ * hght_, 1);
  }

  /// Expression-template for array, of which each element `e` contains
  /// code-value corresponding to some neighbor `n` of filled pixel `p`.
  ///
  /// Element `e` at offset `i` in returned array corresponds to `p`'s
  /// coordinates at same offset `i` in value returned by coords().
  ///
  /// If `e < 0`, then `n` is not filled but in boundary of hole, and `n`'s
  /// *row-major* linear offset in image is `-e - 1`.  (Arrays and matrices
  /// used in Eigen are by default column-major, and they are column-major in
  /// present library, but pixel-data fed into function-object are assumed to
  /// be row-major.)
  ///
  /// Otherwise, `n` is itself also filled (like `p`), and `n` has coordinates
  /// at offset `e` in value returned by coords().
  ///
  /// \tparam I  Type of expression for array of row   -coordinates.
  /// \tparam J  Type of expression for array of column-coordinates.
  /// \param  r  Array of row   -coordinates for neighbor-pixel.
  /// \param  c  Array of column-coordinates for neighbor-pixel.
  /// \return    Expression-template for columnar array of code-values.
  template<typename I, typename J> auto nVal(I const &r, J const &c) const {
    auto nLinColMaj= r + c * hght_;
    auto nLinRowMaj= c + r * wdth_;
    auto a         = cmCol()(nLinColMaj);
    auto b         = (a == -1).template cast<int>();
    return b * (-1 - nLinRowMaj) + (1 - b) * a;
  }

  /// Expression-template for left neighbor via nVal().
  auto nLft() const { return nVal(coords_.col(0), coords_.col(1) - 1); }

  /// Expression-template for right neighbor via nVal().
  /// \return  Expression-template for columnar array of code-values.
  auto nRgt() const { return nVal(coords_.col(0), coords_.col(1) + 1); }

  /// Expression-template for top neighbor via nVal().
  /// \return  Expression-template for columnar array of code-values.
  auto nTop() const { return nVal(coords_.col(0) - 1, coords_.col(1)); }

  /// Expression-template for bottom neighbor via nVal().
  /// \return  Expression-template for columnar array of code-values.
  auto nBot() const { return nVal(coords_.col(0) + 1, coords_.col(1)); }

public:
  /// Prepare to fill pixels given by `coords` in one or more single-component
  /// images of size `width*height`.
  ///
  /// Values to fill with are later calculated according to Dirichlet-problem
  /// for filling pixels `P`, specified by `coords`, on basis of pixels `B` on
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

  /// Deallocate Cholesky-decomposition.
  virtual ~Fill() {
    if(A_) delete A_;
  }

  /// Coordinates of each filled pixel.
  /// \return  Coordinates of each filled pixel.
  Coords const &coords() const { return coords_; }

  /// Fill pixels by calculating solution to linear system for one
  /// color-component of image.
  ///
  /// Each row in solution corresponds to different pixel.  Order of rows in
  /// solution is same as order of rows in `coords` as specified in call to
  /// constructor.
  ///
  /// What is stored in each element of solution is value of pixel's component
  /// satisfying zero Laplacian of component at location of component.
  ///
  /// If template-parameter `Comp` be non-const type, then solution is not just
  /// returned but also (converted to `Comp` if necessary and) copied back into
  /// image.
  ///
  /// Solution is empty if any specified coordinates be out of bounds.
  /// (Coordinates on edge of image are out of bounds for filling.  See
  /// documentation for Coords.)
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

  /// Map from rectangular coordinates of filled pixel to offset of same
  /// coordinates in value returned by coords().
  ///
  /// Returned matrix has same size as image and as dimensions supplied to
  /// constructor.  Each element has value -1 except at coordinates to be
  /// filled.  If coordinates be for filling, then element contains offset of
  /// same coordinates in value returned by coords().
  ///
  /// Array is empty if any specified coordinates be out of bounds.
  /// (Coordinates on edge of image are out of bounds for filling.  See
  /// documentation for Coords.)
  ///
  /// \return  Map from rectangular coordinates of filled pixel to offset of
  ///          same coordinates in value returned by coords().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }

  /// Matrix with four columns and with each row corresponding to different
  /// filled pixel, such that offset `rp` of row is offset in value returned by
  /// coords() for filled pixel `p`; each column corresponds to neighbor `n`
  /// (left, right, top, and bottom, respectively) of `p`; if `n` be in
  /// boundary (if `n` be not filled), element `e` of returned matrix encodes
  /// *row-major* linear offset `i` of `n` in image as `e = -1 - i`; if `n` be
  /// filled, `e = rn`, where `rn` is offset of coordinates of `n` in value
  /// returned by coords().
  ///
  /// \return  Matrix with four columns to encode information about each
  ///          neighbor of each filled pixel.
  ArrayX4i const &lrtb() const { return lrtb_; }

  /// Initialize square matrix for linear problem.
  void initMatrix();
};


} // namespace dirichlet

// Implementation below.

#include "impl/coordsGood.hpp" // coordsGood()
#include "impl/initCoords.hpp" // initCoords()

namespace dirichlet {


using Eigen::Array;
using Eigen::Dynamic;
using Eigen::RowMajor;
using Eigen::Triplet;
using Eigen::Unaligned;
using impl::coordsGood;
using impl::initCoords;
using std::vector;


void Fill::initMatrix() {
  vector<Triplet<float>> t;
  // At *most* five coefficients in matrix for each filled pixel.  Fewer than
  // five coefficients for each filled pixel that touches boundary of hole to
  // fill.  Only one coefficient for each filled pixel that touches no other
  // filled pixel (for filled pixel that touches only boundary-pixels).
  t.reserve(coords_.rows() * 5);
  for(int i= 0; i < coords_.rows(); ++i) {
    t.push_back({i, i, 4.0f});
    int const lft= lrtb_(i, 0);
    int const rgt= lrtb_(i, 1);
    int const top= lrtb_(i, 2);
    int const bot= lrtb_(i, 3);
    if(lft >= 0) t.push_back({i, lft, -1.0f});
    if(rgt >= 0) t.push_back({i, rgt, -1.0f});
    if(top >= 0) t.push_back({i, top, -1.0f});
    if(bot >= 0) t.push_back({i, bot, -1.0f});
  }
  a_.setFromTriplets(t.begin(), t.end());
  A_= new SimplicialCholesky<SparseMatrix<float>>(a_);
}


Fill::Fill(Coords const &coords, unsigned width, unsigned height):
    coords_(coords),
    wdth_(width),
    hght_(height),
    lrtb_(coords.rows(), 4),
    a_(coords.rows(), coords.rows()) {
  if(!coordsGood(coords, width, height)) return;
  coordsMap_  = initCoords(coords, width, height);
  lrtb_.col(0)= nLft();
  lrtb_.col(1)= nRgt();
  lrtb_.col(2)= nTop();
  lrtb_.col(3)= nBot();
  initMatrix();
}


template<typename Comp>
VectorXf Fill::operator()(Comp *image, unsigned stride) const {
  using Image = Array<Comp, Dynamic, Dynamic, RowMajor>;
  using Stride= Eigen::Stride<1, Dynamic>;
  Map<Image, Unaligned, Stride> im(image, hght_, wdth_, Stride(1, stride));
  // First, calculate 1 for encoded offset; 0 for filled pixel.
  auto const fL= (lrtb_.col(0) < 0).cast<int>();
  auto const fR= (lrtb_.col(1) < 0).cast<int>();
  auto const fT= (lrtb_.col(2) < 0).cast<int>();
  auto const fB= (lrtb_.col(3) < 0).cast<int>();
  // Next, clamp offsets at zero on the low end.
  auto const iL= fL * (-lrtb_.col(0) - 1);
  auto const iR= fR * (-lrtb_.col(1) - 1);
  auto const iT= fT * (-lrtb_.col(2) - 1);
  auto const iB= fB * (-lrtb_.col(3) - 1);
  // Now pull pixel-data into b.
  auto const b= fL * im(iL) + fR * im(iR) + fT * im(iT) + fB * im(iB);
  return A_->solve(b);
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
