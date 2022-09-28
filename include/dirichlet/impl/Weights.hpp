/// \file       include/dirichlet/impl/Weights.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::Weights.

#ifndef DIRICHLET_IMPL_WEIGHTS_HPP
#define DIRICHLET_IMPL_WEIGHTS_HPP

#include <cstdint>            // uint64_t
#include <eigen3/Eigen/Dense> // ArrayXX
#include <vector>             // vector

namespace dirichlet::impl {


using Eigen::ArrayXX;
using Eigen::InnerStride;
using Eigen::Unaligned;
using std::vector;


/// For each pixel in image, store value for each of left, right, top, bottom,
/// and center of cruciform pattern of weights for use in solving
/// Dirichlet-problem.  Each weight is stored as two-byte integer.
class Weights {
  vector<uint64_t> s_; ///< Storage-space for weights.
  int              h_; ///< Height of image.
  int              w_; ///< Width of image.

  /// Type for each weight.
  using W= int16_t;

  /// Number of `uint64_t` words needed for storing `nWeights` weights.
  /// \param nWeights  Number of weights to store.
  /// \return          Number of `uint64_t` needed for `nWeights` weights.
  static int nWords(int nWeights) {
    return sizeof(W) * nWeights / sizeof(uint64_t) + 1;
  }

  /// Offset of each weight.
  enum WeightOffset { LFT, RGT, TOP, BOT, CEN, NUM_WEIGHTS };

  /// Stride for same weight in subsequent elements.
  using Stride= InnerStride<NUM_WEIGHTS>;

  /// Type of array for non-const element.
  using Map= Eigen::Map<ArrayXX<W>, Unaligned, Stride>;

  /// Type of array for const element.
  using ConstMap= Eigen::Map<ArrayXX<W> const, Unaligned, Stride>;

  /// Expression-template for weight at specified offset.
  /// \param off  Offset of weight in pattern.
  /// \return     Expression template for array of weights at same offset.
  auto weight(int off) {
    W *first= ((W *)&s_[0]) + off;
    return Map(first, h_, w_);
  }

  /// Expression-template for weight at specified offset.
  /// \param off  Offset of weight in pattern.
  /// \return     Expression template for array of weights at same offset.
  auto weight(int off) const {
    W const *first= ((W const *)&s_[0]) + off;
    return ConstMap(first, h_, w_);
  }

public:
  /// Initialize all weights to zero.
  /// \param h  Height of image.
  /// \param w  Width  of image.
  Weights(int h, int w): s_(nWords(NUM_WEIGHTS * h * w), 0), h_(h), w_(w) {}

  /// Expression-template for left weight.
  auto lft() { return weight(LFT); }

  /// Expression-template for right weight.
  auto rgt() { return weight(RGT); }

  /// Expression-template for top weight.
  auto top() { return weight(TOP); }

  /// Expression-template for bottom weight.
  auto bot() { return weight(BOT); }

  /// Expression-template for center weight.
  auto cen() { return weight(CEN); }

  /// Expression-template for left weight.
  auto lft() const { return weight(LFT); }

  /// Expression-template for right weight.
  auto rgt() const { return weight(RGT); }

  /// Expression-template for top weight.
  auto top() const { return weight(TOP); }

  /// Expression-template for bottom weight.
  auto bot() const { return weight(BOT); }

  /// Expression-template for center weight.
  auto cen() const { return weight(CEN); }

  /// Height of image.
  int h() const { return h_; }

  /// Width of image.
  int w() const { return w_; }
};


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_WEIGHTS_HPP

// EOF
