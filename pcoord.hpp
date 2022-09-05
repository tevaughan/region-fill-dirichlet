/// \file       pcoord.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::pcoord.

#pragma once
#include <cstdint> // uint16_t

namespace regfill {


/// Coordinates of pixel.
struct pcoord {
  uint16_t col; ///< Offset of column.
  uint16_t row; ///< Offset of row.

  pcoord()= default; ///< By default, do not initialize.

  /// Initialize from specified offsets.
  /// \param  Offset of column.
  /// \param  Offset of row.
  pcoord(uint16_t c, uint16_t r): col(c), row(r) {}

  /// Convert from pair of ints.
  /// \param  Offset of column.
  /// \param  Offset of row.
  pcoord(int c, int r): col(c < 0 ? 0 : c), row(r < 0 ? 0 : r) {}

  /// Convert from pair of floats.
  /// \param  Offset of column.
  /// \param  Offset of row.
  pcoord(float c, float r):
      col(c < 0.0f ? 0.0f : c + 0.5f), row(r < 0.0f ? 0.0f : r + 0.5f) {}

  /// Convert from pair of doubles.
  /// \param  Offset of column.
  /// \param  Offset of row.
  pcoord(double const &c, double const &r):
      col(c < 0.0 ? 0.0 : c + 0.5), row(r < 0.0 ? 0.0 : r + 0.5) {}

  /// Test equality of coordinates.
  /// \param p  Other coordinates.
  /// \return   True only if these coordinates match other coordinates.
  bool operator==(pcoord p) const { return col == p.col && row == p.row; }

  /// Test inequality of coordinates.
  /// \param p  Other coordinates.
  /// \return   True only if these coordinates do not match other coordinates.
  bool operator!=(pcoord p) const { return col != p.col || row != p.row; }
};


} // namespace regfill

// EOF
