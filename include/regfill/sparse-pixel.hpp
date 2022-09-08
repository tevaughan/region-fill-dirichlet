/// \file       include/regfill/sparse-pixel.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::sparse_pixel.

#ifndef REGFILL_SPARSE_PIXEL_HPP
#define REGFILL_SPARSE_PIXEL_HPP

#include "coords.hpp" // coords

namespace regfill {


/// Pixel with non-zero value in sparse_image.
struct sparse_pixel {
  coords crd; ///< Coordinates of pixel.
  float  val; ///< Value of pixel.
};


} // namespace regfill

#endif // ndef REGFILL_SPARSE_PIXEL_HPP

// EOF
