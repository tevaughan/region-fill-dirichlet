/// \file       threshold-coords.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::threshold_coords.

#ifndef REGFILL_THRESHOLD_COORDS_HPP
#define REGFILL_THRESHOLD_COORDS_HPP

#include "pcoord.hpp" // pcoord
#include <map>        // map
#include <vector>     // vector

namespace regfill {


using std::vector;
class image;


/// Coordinates above threshold in mask.
class threshold_coords {
  /// List of coordinates above threshold in mask.
  vector<pcoord> crd_;

  /// Type for
  /// (a) linear offset of pixel in mask and
  /// (b) key in m_.
  using pix_offset= unsigned;

  /// Type for
  /// (a) linear offset of pixel's coordinates in crd_ and
  /// (b) value in m_.
  using crd_offset= unsigned;

  /// For each pixel above threshold in mask, map from linear offset of pixel
  /// in mask to linear offset of pixel's coordinates in crd_.
  std::map<pix_offset, crd_offset> map_;

public:
  /// Initialize from mask.
  /// \param mask  Reference to mask.
  threshold_coords(image const &mask);

  /// List of coordinates above threshold in mask.
  /// \return  List of coordinates above threshold in mask.
  auto const &crd() const { return crd_; }

  /// For each pixel above threshold in mask, map from linear offset of pixel
  /// in mask to linear offset of pixel's coordinates in crd_.
  ///
  /// \return  Map from linear offset of pixel in mask to linear offset of
  ///          pixel's coordinates in list of coordinates above threshold.
  auto const &map() const { return map_; }
};


} // namespace regfill

#include "image.hpp"

namespace regfill {


inline threshold_coords::threshold_coords(image const &mask):
    crd_(mask.threshold()) {
  for(unsigned i= 0; i < crd_.size(); ++i) map_[mask.lin(crd_[i])]= i;
}


} // namespace regfill

#endif // ndef REGFILL_THRESHOLD_COORDS_HPP

// EOF
