/// \file       bin2x2.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
///
/// \brief      Definition of
///               df::impl::BinArr,
///               df::impl::BinFunctor,
///               df::impl::BinArrType,
///               df::impl::makeBinArr(),
///               df::bin2x2().

#pragma once
#include <eigen3/Eigen/Dense> // seq, Array, ArrayBase, ColMajor, etc.

namespace df {


using Eigen::seq;


namespace impl {


using Eigen::Array;
using Eigen::ArrayBase;
using Eigen::ColMajor;
using Eigen::CwiseNullaryOp;
using Eigen::Dynamic;
using Eigen::Index;


/// Provide type, BinArr::Type, of array used for binning.
///
/// BinArr::Type is derived from type of first argument to bin2x2().
///
/// Size of instance of BinArr::Type will, if necessary, be one larger in rows
/// and/or cols, to guarantee that size is even in each dimension.
///
/// \tparam A  Type of first argument to bin2x2().
///
template<typename A> class BinArr {
  /// Increment size `n` only if it be neither Dynamic nor even.
  /// \param n  Size to consider.
  /// \return   Quantity compatible with even size at least so large as `n`.
  constexpr static auto possiblyIncrement(Index n) {
    if(n == Dynamic || n == n / 2 * 2) return n;
    return n + 1;
  }

  /// Either Dynamic or else even size at least A::RowsAtCompileTime.
  /// \return  Either Dynamic or else even size at least A::RowsAtCompileTime.
  constexpr static auto NR() {
    return possiblyIncrement(A::RowsAtCompileTime);
  }

  /// Either Dynamic or else even size at least A::ColsAtCompileTime.
  /// \return  Either Dynamic or else even size at least A::ColsAtCompileTime.
  constexpr static auto NC() {
    return possiblyIncrement(A::ColsAtCompileTime);
  }

  /// Either Dynamic or else even size at least A::MaxRowsAtCompileTime.
  ///
  /// \return  Either Dynamic or else even size at least
  ///          A::MaxRowsAtCompileTime.
  ///
  constexpr static auto MAX_NR() {
    return possiblyIncrement(A::MaxRowsAtCompileTime);
  }

  /// Either Dynamic or else even size at least A::MaxColsAtCompileTime.
  ///
  /// \return  Either Dynamic or else even size at least
  ///          A::MaxColsAtCompileTime.
  ///
  constexpr static auto MAX_NC() {
    return possiblyIncrement(A::MaxColsAtCompileTime);
  }

  /// Type of scalar used in `A`.
  using Scalar= typename A::Scalar;

public:
  /// Type of array used by bin2x2() via BinFunctor and makeBinArr().
  using Type= Array<Scalar, NR(), NC(), ColMajor, MAX_NR(), MAX_NC()>;
};


/// Functor that presents an array whose number of rows is guaranteed to be
/// even; so too for number of columns.
///
/// In case underlying array have odd dimension, and in case of access to last
/// row or column in ostensibly even-sized array, functor will either return
/// zero or else return duplicate of the element at largest available index.
///
/// \tparam A  Type of underlying array, which might have odd size in either or
///            both dimensions.
template<typename A> class BinFunctor {
  /// Scalar for underlying array.
  using Scalar= typename A::Scalar;

  /// Reference to underlying array.
  A const &a_;

  /// Value of zero that can be referenced.
  Scalar zero_= Scalar(0);

  /// True only if reference to value at edge should returned when access out
  /// of bounds; otherwise, reference to zero will be returned by functor.
  bool dup_;

public:
  /// Initialize from reference to underlying array and from flag indicating
  /// how to treat out-of-bounds access.
  ///
  /// \param a  Reference to underlying array.
  ///
  /// \param dup  True only if reference to value at edge should be returned
  ///             when access out of bounds; otherwise, reference to zero will
  ///             be returned by functor.
  ///
  BinFunctor(A const &a, bool dup): a_(a), dup_(dup) {}

  /// Provide interface as if underlying array have even size along each
  /// dimension.
  ///
  /// Pass through access to underlying array, but handle out-of-bounds access
  /// in case underlying array have odd dimension.
  ///
  /// \param r  Offset of row.
  /// \param c  Offset of column.
  /// \return   Reference to element, to closest edge-element, or to zero.
  ///
  Scalar const &operator()(Index r, Index c) const {
    if(r >= a_.rows()) {
      if(dup_) {
        r= a_.rows() - 1;
      } else {
        return zero_;
      }
    }
    if(c >= a_.cols()) {
      if(dup_) {
        c= a_.cols() - 1;
      } else {
        return zero_;
      }
    }
    return a_(r, c);
  }
};


/// Convenience so that `typename` keyword need not be used.
/// \tparam A  Type of first argument to bin2x2().
template<typename A> using BinArrType= typename BinArr<A>::Type;


/// Construct nullary expression that provides interface to array that is
/// guaranteed to have even size along each dimension.
///
/// \tparam A    Type of underlying array, which might have odd dimension.
/// \param  a    Reference to underlying array.
/// \param  nr   Even number of rows provided by interface.
/// \param  nc   Even number of columns provided by interface.
///
/// \param  dup  True if edge-value should be duplicated on out-of-bounds
///              access; false if zero should be returned on out-of-bounds
///              access.
///
/// \return     Expression-template for even-sized array.
///
template<typename A>
CwiseNullaryOp<BinFunctor<A>, BinArrType<A>>
makeBinArr(ArrayBase<A> const &a, Index nr, Index nc, bool dup) {
  if(nr != nr / 2 * 2) throw "nr must be even";
  if(nc != nc / 2 * 2) throw "nc must be even";
  return BinArrType<A>::NullaryExpr(nr, nc, BinFunctor<A>(a.derived(), dup));
}


} // namespace impl


/// Perform 2x2-binning on array `a`.
///
/// Each superpixel in returned, binned array `b` will contain sum of
/// corresponding 2x2 pixels in `a`.
///
/// Throw exception if `a` have fewer than two rows or two columns.
///
/// By default, if `a` have odd number of rows and/or columns, then pixels out
/// of bounds will be considered to have zero value.  However, if `dup` be set
/// true, then row or column at bottom or right edge (respecively) of `a` will
/// be duplicated as necessary for binning.
///
/// \tparam A    Type of array to bin.
/// \param  a    Array to bin.
///
/// \param  dup  Only if explicitly set true, then right or bottom edge of `a`
///              will be duplicated in case of odd number of columns or rows,
///              respectively; by default, treat pixels out of bounds as having
///              zero value.
///
/// \return      Binned array `b`.
///
template<typename A> auto bin2x2(A const &a, bool dup= false) {
  auto const anr= a.rows();
  auto const anc= a.cols();
  if(anr < 2) throw "too few rows";
  if(anc < 2) throw "too few cols";
  auto const anrEven= (anr == anr / 2 * 2);
  auto const ancEven= (anc == anc / 2 * 2);
  auto const bnr    = (anrEven ? anr / 2 : (anr + 1) / 2);
  auto const bnc    = (ancEven ? anc / 2 : (anc + 1) / 2);
  auto const nr     = bnr * 2;
  auto const nc     = bnc * 2;
  auto const aExt   = impl::makeBinArr(a, nr, nc, dup);
  auto const lft    = seq(0, nc - 2, 2);
  auto const rgt    = seq(1, nc - 1, 2);
  auto const top    = seq(0, nr - 2, 2);
  auto const bot    = seq(1, nr - 1, 2);
  return aExt(top, lft) + aExt(bot, lft) + aExt(top, rgt) + aExt(bot, rgt);
}


} // namespace df

// EOF
