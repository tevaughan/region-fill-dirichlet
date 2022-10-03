/// \file       BinPrep.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
///
/// \brief      Definition of
///               df::minMult(),
///               df::maxBinFactor(),
///               df::Extension,
///               df::BinPrep.

#pragma once
#include <eigen3/Eigen/Dense> // ArrayXX, seq

namespace df {


using Eigen::ArrayXX;
using Eigen::seq;


/// Find smallest multiple `m` of factor `f` such that `m >= n`.
/// \param n  Minimum value to return.
/// \param f  Factor required in return-value.
/// \return   Of `f`, minimum multiple greater than or equal to `n`.
int minMult(int n, int f) {
  int const j= n / f;
  if(j * f < n) return (j + 1) * f;
  return j * f;
}


/// Maximum binning factor for extension of array `a`.
///
/// Maximum factor is largest power of two that is
/// - less than or equal to 1/4 of number of rows    in `a`, and
/// - less than or equal to 1/4 of number of columns in `a`.
///
/// By limiting the maximum binning factor in this way, one ensures that most
/// binned image has at least 4x4 pixels.
///
/// \param a  Original array that might be extended in rows and columns.
/// \return   Maximum binning factor.
template<typename A> int maxBinFactor(A const &a) {
  int           bf= 1;
  constexpr int D = 8;
  while(bf <= a.rows() / D && bf <= a.cols() / D) bf*= 2;
  return bf;
}


/// Code identifying how to fill new cells in extension of original array.
enum Extension {
  ZEROS, ///< Fill new cells with zeros.
  COPIES ///< Fill new cels with copy of edge of original array.
};


/// Calculate array that is guaranteed to be binnable.
/// \tparam A  Type of original array passed to constructor.
template<typename A> class BinPrep {
  using S= typename A::Scalar; ///< Type of scalar in original array.

  /// Maximum row-offset for array.
  /// \tparam T  Type of array.
  /// \param  t  Reference to array.
  /// \return    Maximum row-offset.
  template<typename T> auto mr(T const &t) { return t.rows() - 1; }

  /// Maximum column-offset for array.
  /// \tparam T  Type of array.
  /// \param  t  Reference to array.
  /// \return    Maximum column-offset.
  template<typename T> auto mc(T const &t) { return t.cols() - 1; }

  /// Expression for bottom left of extArray.
  /// \param a  Reference to original array.
  /// \return   Expression for bottom left of extArray.
  auto botLft(A const &a) {
    return extArray(seq(a.rows(), mr(extArray)), seq(0, mc(a)));
  }

  /// Expression for top right of extArray.
  /// \param a  Reference to original array.
  /// \return   Expression for top right of extArray.
  auto topRgt(A const &a) {
    return extArray(seq(0, mr(a)), seq(a.cols(), mc(extArray)));
  }

  /// Expression for bottom right of extArray.
  /// \param a  Reference to original array.
  /// \return   Expression for bottom right of extArray.
  auto botRgt(A const &a) {
    return extArray(seq(a.rows(), mr(extArray)), seq(a.cols(), mc(extArray)));
  }

public:
  int const maxBinFactor; ///< Maximum binning factor in each direction.
  int const bRows;        ///< Number of rows in extArray.
  int const bCols;        ///< Number of cols in extArray.

  /// Array that, in comparison with original array passed to constructor, is
  /// (possibly) extended in number of rows and/or columns so that extended
  /// array is guaranteed to be binnable, initialized in upper left with copy
  /// of original array, and padded on right and bottom as specified in
  /// constructor.
  ArrayXX<S> extArray;

  /// Initialize (possibly) extended array.
  ///
  /// \param a  Original array from which extended array is built.
  ///
  /// \param e  If ZEROS, fill with zeros all new cells in portion of exteded
  ///           array not covered by `a`, which is copied into top left of
  ///           extended array; otherwise, replicate bottom-right edges of `a`
  ///           as necessary to fill extended array.
  ///
  BinPrep(A const &a, Extension e= ZEROS):
      maxBinFactor(df::maxBinFactor(a)),
      bRows(minMult(a.rows(), maxBinFactor)),
      bCols(minMult(a.cols(), maxBinFactor)),
      extArray(bRows, bCols) {
    extArray(seq(0, a.rows() - 1), seq(0, a.cols() - 1))= a;
    if(extArray.rows() == a.rows() && extArray.cols() == a.cols()) return;
    if(e == ZEROS) {
      botLft(a)= ArrayXX<S>::Zero(botLft(a).rows(), botLft(a).cols());
      topRgt(a)= ArrayXX<S>::Zero(topRgt(a).rows(), topRgt(a).cols());
      botRgt(a)= ArrayXX<S>::Zero(botRgt(a).rows(), botRgt(a).cols());
    } else {
      // Compute amount of extension in each of row- and column-directions.
      auto const dr= extArray.rows() - a.rows();
      auto const dc= extArray.cols() - a.cols();
      // Replicate things on lower right of original array, and fill extArray.
      botLft(a)= a(mr(a), seq(0, mc(a))).replicate(dr, 1);
      topRgt(a)= a(seq(0, mr(a)), mc(a)).replicate(1, dc);
      botRgt(a)= a(seq(mr(a), mr(a)), seq(mc(a), mc(a))).replicate(dr, dc);
    }
  }
};


} // namespace df

// EOF
