/// \file       BinPrep.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of df::minMult(), df::BinPrep.

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


/// Code identifying how prepBinning() should fill new cells in extension of
/// original array.
enum Extension {
  ZEROS, ///< Fill new cells with zeros.
  COPIES ///< Fill new cels with copy of edge of original array.
};


/// Type of array returned by prepBinning().
/// \tparam A  Type of array passed to prepBinning().
template<typename A> using BinArray= ArrayXX<typename A::Scalar>;


/// Calculate BinPrep::extArray, which is guaranteed to be binnable to level
/// specified in constructor.
///
/// \tparam A  Type of original array passed to constructor.
///
template<typename A> class BinPrep {
  using S= typename A::Scalar; ///< Type of scalar in original array.
  int const maxBin_;           ///< Maximum binning factor in each direction.
  int const bRows_;            ///< Number of rows in extArray.
  int const bCols_;            ///< Number of cols in extArray.

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
  /// Array that, in comparison with original array passed to constructor, is
  /// (possibly) extended in number of rows and/or columns so that extArray is
  /// guaranteed to be binnable to level specified in constructor, initialized
  /// in upper left with copy of original array, and padded on right and bottom
  /// as specified in constructor.
  ArrayXX<S> extArray;

  /// Initialize extArray.
  ///
  /// \param a      Original array from which extArray is built.
  ///
  /// \param level  Number of 2x2-binnings that extArray is guaranteed to be
  ///               binnable to.  Each binning reduces each of number of rows
  ///               and number of columns by factor of two.
  ///
  /// \param e      If ZEROS, fill with zeros all new cells in portion of
  ///               extArray not covered by `a`, which is copied into top left
  ///               of extArray; otherwise, replicate bottom-right edges of `a`
  ///               as necessary to fill extArray.
  ///
  BinPrep(A const &a, int level, Extension e= ZEROS):
      maxBin_(1 << level),
      bRows_(minMult(a.rows(), maxBin_)),
      bCols_(minMult(a.cols(), maxBin_)),
      extArray(bRows_, bCols_) {
    if(level < 0) throw "negative level of binning";
    extArray(seq(0, a.rows() - 1), seq(0, a.cols() - 1))= a;
    if(extArray.rows() == a.rows() && extArray.cols() == a.cols()) return;
    if(e == ZEROS) {
      botLft(a)= ArrayXX<S>::Zero(botLft(a).rows(), botLft(a).cols());
      topRgt(a)= ArrayXX<S>::Zero(topRgt(a).rows(), topRgt(a).cols());
      botRgt(a)= ArrayXX<S>::Zero(botRgt(a).rows(), botRgt(a).cols());
    } else {
      auto const dr= extArray.rows() - a.rows();
      auto const dc= extArray.cols() - a.cols();
      botLft(a)    = a(mr(a), seq(0, mc(a))).replicate(dr, 1);
      topRgt(a)    = a(seq(0, mr(a)), mc(a)).replicate(1, dc);
      botRgt(a)    = a(seq(mr(a), mr(a)), seq(mc(a), mc(a))).replicate(dr, dc);
    }
  }
};


} // namespace df

// EOF
