/// \file       neighbors.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::neighbors.

#ifndef REGFILL_NEIGHBORS_HPP
#define REGFILL_NEIGHBORS_HPP

#include "coords.hpp" // coords
#include <cstdint>    // uint16_t

namespace regfill {


/// Offsets and flags for neighbors of a pixel.
struct neighbors {
  uint16_t const rb; ///< Row-offset below central pixel.
  uint16_t const rt; ///< Row-offset above central pixel.
  uint16_t const cl; ///< Column-offset left of central pixel.
  uint16_t const cr; ///< Column-offset right of central pixel.
  bool const     fb; ///< True only if there be row below central pixel.
  bool const     ft; ///< True only if there be row above central pixel.
  bool const     fl; ///< True only if there be column left of central pixel.
  bool const     fr; ///< True only if there be column right of central pixel.

public:
  /// Initialize offsets and flags for neighbors.
  /// \param nc   Number of columns in mask.
  /// \param nr   Number of rows in mask.
  /// \param cen  Rectangular coordinates of pixel central to neighbors.
  neighbors(int nc, int nr, coords cen):
      rb(cen.row + 1),
      rt(cen.row - 1),
      cl(cen.col - 1),
      cr(cen.col + 1),
      fb(int(cen.row) < nr - 1),
      ft(cen.row > 0),
      fl(cen.col > 0),
      fr(int(cen.col) < nc - 1) {}
};


} // namespace regfill

#endif // ndef REGFILL_NEIGHBOR_HPP

// EOF
