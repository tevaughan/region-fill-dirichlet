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
