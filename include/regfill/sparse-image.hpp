/// \file       include/regfill/sparse-image.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::sparse_image.

#ifndef REGFILL_SPARSE_IMAGE_HPP
#define REGFILL_SPARSE_IMAGE_HPP

#include "basic-image.hpp"  // basic_image
#include "sparse-pixel.hpp" // sparse_pixel
#include <map>              // map
#include <vector>           // vector

namespace regfill {


using std::vector;
class image;


/// Image with storage only for pixels with non-zero value.
class sparse_image: public basic_image {
  /// Storage for pixels with non-zero value.
  vector<sparse_pixel> pix_;

  /// Type for
  /// (a) linear offset of pixel in dense image and
  /// (b) key in map_.
  using dense_offset= unsigned;

  /// Type for
  /// (a) linear offset of pixel in pix_ and
  /// (b) value in map_.
  using sparse_offset= unsigned;

  /// Map from linear offset of pixel in dense image to linear offset of pixel
  /// in pix_.
  std::map<dense_offset, sparse_offset> map_;

public:
  /// Initialize from mask.
  /// \param mask  Reference to mask.
  sparse_image(image const &mask);

  sparse_image(regfill::size s, vector<sparse_pixel> const &p);

  /// List of non-zero pixels.
  /// \return  List of non-zero pixels.
  auto const &pix() const { return pix_; }

  /// Map from linear offset of pixel in dense image to linear offset of pixel
  /// in list of non-zero pixels.
  ///
  /// \return  Map from linear offset of pixel in dense image to linear offset
  ///          of pixel in list of non-zero pixels.
  auto const &map() const { return map_; }
};


} // namespace regfill

#include "image.hpp"

namespace regfill {


inline sparse_image::sparse_image(image const &mask):
    basic_image(mask.size()), pix_(mask.threshold()) {
  for(unsigned i= 0; i < pix_.size(); ++i) {
    map_[mask.size().lin(pix_[i].crd)]= i;
  }
}


inline sparse_image::sparse_image(
      regfill::size s, vector<sparse_pixel> const &p):
    basic_image(s), pix_(p) {
  for(unsigned i= 0; i < pix_.size(); ++i) map_[s.lin(pix_[i].crd)]= i;
}


} // namespace regfill

#endif // ndef REGFILL_SPARSE_IMAGE_HPP

// EOF
