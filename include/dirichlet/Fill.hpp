/// \file       include/dirichlet/Fill.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::Fill.

#ifndef DIRICHLET_FILL_HPP
#define DIRICHLET_FILL_HPP

#include <eigen3/Eigen/Sparse> // SparseMatrix

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet {


using Eigen::ArrayX2i; // coords
using Eigen::ArrayX4i;
using Eigen::ArrayXi;
using Eigen::ArrayXXf;
using Eigen::ArrayXXi;
using Eigen::ConjugateGradient;
using Eigen::Lower;
using Eigen::SimplicialCholesky;
using Eigen::SparseMatrix;
using Eigen::Upper;
using Eigen::VectorXf;


/// Fill holes in image by solving Dirichlet-problem for zero-valued Laplacian
/// across specified hole-pixels in image.
///
/// Every pixel specified as to be filled must be in interior of image; no such
/// pixel may be at edge of image.
///
/// Instantiating `%Fill` does all preparatory work necessary to enable quick
/// filling of same coordinates in one or more images of same size.
///
/// Filling image happens when instance is used as function-object.
class Fill {
  ArrayX2i coords_; ///< Coordinates of filled pixels in each image.
  unsigned wdth_;   ///< Number of columns in each image to fill.
  unsigned hght_;   ///< Number of rows    in each image to fill.

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
  SimplicialCholesky<SparseMatrix<float>> *A_= nullptr;

  /// Workspace for conjugate-gradient approach.
  ConjugateGradient<SparseMatrix<float>, Lower | Upper> CG_;

  /// True only if conjugate-gradient method should be used instead of
  /// Cholesky.
  bool cg_;

  /// Disallow copying because we store owned pointer.
  Fill(Fill const &)= delete;

  /// Disallow assignment because we store owned pointer.
  Fill &operator=(Fill const &)= delete;

  /// coordsMap_ as single column.
  /// \return  coordsMap_ as single column.
  auto cmCol() const {
    return Eigen::Map<ArrayXi const>(&coordsMap_(0, 0), wdth_ * hght_, 1);
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

  /// Find non-zero coordinates in `mask`.
  /// \tparam Comp    Type of each component in mask.
  /// \param  mask    Pointer to mask.
  /// \param  width   Width of mask.
  /// \param  height  Height of mask.
  template<typename Comp>
  static ArrayX2i
  findCoords(Comp *mask, unsigned width, unsigned height, int stride);

  /// Initialize square matrix for linear problem.
  void initMatrix();

  /// Initialize coords_ (which pixels to fill), and initialize coordsMap_.
  /// \param coords  Coordinates of each pixel to be Dirichlet-filled.
  void initCoords(ArrayX2i const &coords);

  /// Calculate solution to linear system for one color-component of image.
  /// \tparam Map    Type of single-component image.
  /// \param  image  Reference to single-component image.
  /// \return        Solution to linear system.
  template<typename Map> VectorXf solve(Map const &image) const;

  /// Copy solution back into original image.
  /// \tparam Map    Type of single-component image.
  /// \param  image  Reference to single-component image.
  /// \param  x      Solution.
  template<typename Map>
  void copySolutionBackIntoImage(Map &im, VectorXf const &x) const;

public:
  /// Prepare for filling one or more single-component images of size
  /// `width*height`; pixels to fill are given by `coords`.
  ///
  /// Values to fill with are later calculated according to Dirichlet-problem
  /// for filling pixels `P`, specified by `coords`, on basis of pixels `B` on
  /// boundary of `P`.
  ///
  /// Every pixel specified in `coords` must lie in interior of image in order
  /// to indicate a pixel that should be filled; every pixel on edge of image
  /// can serve as boundary-condition but cannot be filled.  If any pixel
  /// specified in `coords` be on our outside of image's edge, then that pixel
  /// is ignored; no corresponding pixel in image will be filled.
  ///
  /// \param coords  Coordinates of each pixel to be Dirichlet-filled.
  /// \param width   Number of columns in image.
  /// \param height  Number of rows in image.
  /// \param cg      True if conjugate-gradient method should be used.
  ///
  Fill(ArrayX2i const &coords,
       unsigned        width,
       unsigned        height,
       bool            cg= false);

  /// Prepare for filling one or more single-component images of size
  /// `width*height`; pixels to fill are given by non-zero pixels in `mask`.
  ///
  /// Values to fill with are later calculated according to Dirichlet-problem
  /// for filling pixels `P`, non-zero in `mask`, on basis of pixels `B` on
  /// boundary of P.
  ///
  /// Every pixel specified as non-zero in `mask` must lie in interior of image
  /// in order to cause corresponding pixel in image to be filled; every pixel
  /// on edge of image can serve as boundary-condition but cannot be filled.
  /// If any pixel specified as non-zero in `mask` be on edge of image, then
  /// that pixel is ignored (will not be filled).
  ///
  /// \tparam Comp    Type of each component of each pixel.
  ///
  /// \param  mask    Pointer to image whose non-zero pixels are to be filled
  ///                 when same-sized image is provided later.
  ///
  /// \param  width   Number of columns in image.
  /// \param  height  Number of rows in image.
  ///
  /// \param  stride  Number of instances of type Comp between component of one
  ///                 pixel and corresponding component of next pixel.
  ///
  /// \param cg       True if conjugate-gradient method should be used instead
  ///                 of Cholesky.
  ///
  template<typename Comp>
  Fill(Comp    *mask,
       unsigned width,
       unsigned height,
       int      stride= 1,
       bool     cg    = false);

  /// Deallocate Cholesky-decomposition.
  virtual ~Fill() {
    if(A_ != nullptr) delete A_;
  }

  /// Coordinates of each filled pixel.
  /// \return  Coordinates of each filled pixel.
  ArrayX2i const &coords() const { return coords_; }

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
  /// (Coordinates on edge of image are out of bounds for filling.)
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
  VectorXf operator()(Comp *image, int stride= 1) const;

  /// Map from rectangular coordinates of filled pixel to offset of same
  /// coordinates in value returned by coords().
  ///
  /// Returned matrix has same size as image and as dimensions supplied to
  /// constructor.  Each element has value -1 except at coordinates to be
  /// filled.  If coordinates be for filling, then element contains offset of
  /// same coordinates in value returned by coords().
  ///
  /// Array is empty if any specified coordinates be out of bounds.
  /// (Coordinates on edge of image are out of bounds for filling.)
  ///
  /// \return  Map from rectangular coordinates of filled pixel to offset of
  ///          same coordinates in value returned by coords().
  ///
  ArrayXXi const &coordsMap() const { return coordsMap_; }

  /// Matrix with each row corresponding to different filled pixel `p` and with
  /// four columns, each corresponding to different neighbor `n` of `p`.  Row
  /// `r` in returned matrix corresponds to coordinates at same row in matrix
  /// returned by coords(); each column in matrix returned by `%lrtb()`
  /// corresponds to neighbor (left, right, top, and bottom, respectively) `n`
  /// of `p`.  On one hand, if `n` be in boundary (if `n` be not filled), then
  /// element `e` of returned matrix encodes row-major linear image-offset `i`
  /// of `n` as `e = -1 - i`; on other hand, if `n` itself be filled pixel,
  /// then `e = rn`, where `rn` is row-offset of coordinates of `n` in matrix
  /// returned by coords().
  ///
  /// \return  Matrix with four columns to encode information about each
  ///          neighbor of each filled pixel.
  ArrayX4i const &lrtb() const { return lrtb_; }
};


} // namespace dirichlet

// Implementation below.

namespace dirichlet {


using Eigen::Array;
using Eigen::Array2i;
using Eigen::Dynamic;
using Eigen::RowMajor;
using Eigen::Triplet;
using Eigen::Unaligned;
using std::is_const_v;
using std::is_floating_point_v;
using std::is_integral_v;
using std::is_same_v;
using std::is_unsigned_v;
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
  if(cg_) {
    CG_.compute(a_);
  } else {
    A_= new SimplicialCholesky<SparseMatrix<float>>(a_);
  }
}


void Fill::initCoords(ArrayX2i const &coords) {
  // First build coords_ by excluding illegal pixels.
  constexpr int b   = 1; // Width (pixels) of illegal border.
  int const     rmin= b;
  int const     cmin= b;
  int const     rmax= hght_ - 1 - b;
  int const     cmax= wdth_ - 1 - b;
  int           j   = 0;
  for(unsigned i= 0; i < coords.rows(); ++i) {
    int const r= coords.row(i)[0];
    int const c= coords.row(i)[1];
    if(r < rmin || c < cmin || r > rmax || c > cmax) continue;
    coords_.row(j)[0]= r;
    coords_.row(j)[1]= c;
    ++j;
  }
  coords_.conservativeResize(j, 2);
  // Second, build coordsMap_.
  ArrayXXi            cmap= ArrayXXi::Constant(hght_, wdth_, -1);
  auto const          lin = coords_.col(0) + coords_.col(1) * hght_;
  Eigen::Map<ArrayXi> m(&cmap(0, 0), hght_ * wdth_, 1);
  m(lin)    = ArrayXi::LinSpaced(coords_.rows(), 0, coords_.rows() - 1);
  coordsMap_= cmap;
}


Fill::Fill(ArrayX2i const &coords, unsigned width, unsigned height, bool cg):
    coords_(coords.rows(), coords.cols()),
    wdth_(width),
    hght_(height),
    lrtb_(coords.rows(), 4),
    a_(coords.rows(), coords.rows()),
    cg_(cg) {
  initCoords(coords); // Side effect is change of coords_.rows().
  lrtb_.conservativeResize(coords_.rows(), 4);
  lrtb_.col(0)= nLft();
  lrtb_.col(1)= nRgt();
  lrtb_.col(2)= nTop();
  lrtb_.col(3)= nBot();
  a_.conservativeResize(coords_.rows(), coords_.rows());
  initMatrix();
}


template<typename Comp>
ArrayX2i Fill::findCoords(Comp *m, unsigned w, unsigned h, int stride) {
  Comp const zero(0);
  // Width (pixels) of edge.
  constexpr unsigned ew= 1;
  // Ignore every pixel on edge of image.
  if(h <= 2 * ew || w <= 2 * ew) return ArrayX2i();
  ArrayX2i coords((h - 2 * ew) * (w - 2 * ew), 2);
  // Walk the mask.
  int i= 0;
  for(unsigned r= ew; r < h - ew; ++r) {
    Comp const *mask= m + (r * w + ew) * stride;
    for(unsigned c= ew; c < w - ew; ++c) {
      if(*mask != zero) {
        coords.row(i)= Array2i{r, c};
        ++i;
      }
      mask+= stride;
    }
  }
  coords.conservativeResize(i, 2);
  return coords;
}


template<typename Comp>
Fill::Fill(Comp *mask, unsigned width, unsigned height, int stride, bool cg):
    Fill(findCoords(mask, width, height, stride), width, height, cg) {}


template<typename Map> VectorXf Fill::solve(Map const &im) const {
  // First, calculate 1 for encoded offset; 0 for filled pixel.
  auto const fL= (lrtb_.col(0) < 0);
  auto const fR= (lrtb_.col(1) < 0);
  auto const fT= (lrtb_.col(2) < 0);
  auto const fB= (lrtb_.col(3) < 0);
  // Next, clamp offsets at zero on the low end.
  auto const iL= fL.cast<int>() * (-lrtb_.col(0) - 1);
  auto const iR= fR.cast<int>() * (-lrtb_.col(1) - 1);
  auto const iT= fT.cast<int>() * (-lrtb_.col(2) - 1);
  auto const iB= fB.cast<int>() * (-lrtb_.col(3) - 1);
  // Next, calculate values.
  auto const bL= fL.cast<float>() * im(iL).template cast<float>();
  auto const bR= fR.cast<float>() * im(iR).template cast<float>();
  auto const bT= fT.cast<float>() * im(iT).template cast<float>();
  auto const bB= fB.cast<float>() * im(iB).template cast<float>();
  // Now, pull pixel-data into b by evaluated vectorized expression.
  VectorXf const b= bL + bR + bT + bB;
  if(cg_) {
    return CG_.solve(b);
  } else {
    return A_->solve(b);
  }
}


template<typename Map>
void Fill::copySolutionBackIntoImage(Map &im, VectorXf const &x) const {
  using Comp                = typename Map::CompType;
  constexpr bool is_integral= is_integral_v<Comp>;
  // Image is row-major.
  auto const ii= /*col*/ coords_.col(0) * wdth_ + /*row*/ coords_.col(1);
  if constexpr(is_integral) {
    if constexpr(is_unsigned_v<Comp>) {
      im(ii)= (x.array() + 0.5f).cast<Comp>();
    } else {
      auto const neg= (x.array() < 0.0f).cast<Comp>();
      auto const rup= (x.array() + 0.5f).cast<Comp>();
      auto const rdn= (x.array() - 0.5f).cast<Comp>();
      // Round in correct direction.
      im(ii)= neg * rdn + (Comp(1) - neg) * rup;
    }
  } else {
    if constexpr(is_same_v<float, Comp>) {
      im(ii)= x.array();
    } else {
      im(ii)= x.array().cast<Comp>();
    }
  }
}


/// Type to which single-component image is mapped.
///
/// Single-component image *appears* as if it were of this type, regardless of
/// how component is stored relative to other components in same image.
///
/// \tparam Comp  Type of each color-component in image.
template<typename Comp> using Image= Array<Comp, Dynamic, 1>;


/// Allow run-time (Eigen::Dynamic) stride in map for single-component,
/// row-major image.
using ImageStride= Eigen::Stride<1, Dynamic>;


/// Descend from Eigen::Map to provide access to type of image-component.
/// \tparam Comp  Type of each color-component in image.
template<typename Comp>
struct Map: public Eigen::Map<Image<Comp>, Unaligned, ImageStride> {
  /// Type of ancestor.
  using P= Eigen::Map<Image<Comp>, Unaligned, ImageStride>;

  /// Type of each color-component in image.
  using CompType= Comp;

  /// Initialize ancestor.
  ///
  /// \param image   Pointer to first component in single-component image.
  /// \param rows    Number of rows    in mapped image.
  /// \param cols    Number of columns in mapped image.
  ///
  /// \param stride  Number of instances of `Comp` between consecutive
  ///                components in single-component image.
  ///
  Map(Comp *image, unsigned rows, unsigned cols, ImageStride const &stride):
      P(image, rows, cols, stride) {}
};


template<typename Comp>
VectorXf Fill::operator()(Comp *image, int stride) const {
  Map im(image, int(hght_ * wdth_), 1, ImageStride(1, stride));
  // Find solution.
  VectorXf const x= solve(im);
  // If possible, copy solution back into original image.
  constexpr bool is_const   = is_const_v<Comp>;
  constexpr bool is_integral= is_integral_v<Comp>;
  constexpr bool is_fp      = is_floating_point_v<Comp>;
  if constexpr(!is_const && (is_integral || is_fp)) {
    copySolutionBackIntoImage(im, x);
  }
  return x;
}


} // namespace dirichlet

#endif // ndef DIRICHLET_FILL_HPP

// EOF
