/// \file       include/regfill/basic-image.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::basic_image.

#ifndef REGFILL_BASIC_IMAGE_HPP
#define REGFILL_BASIC_IMAGE_HPP

#include "size.hpp" // coords, size

namespace regfill {


/// Store size for any type of image (dense or sparse).
class basic_image {
protected:
  regfill::size size_; ///< Dimensions of image.

  /// By default, do not initialize.
  basic_image()= default;

  /// Initialize size.
  /// \param nc  Number of columns.
  /// \param nr  Number of rows.
  basic_image(uint16_t nc, uint16_t nr): size_(nc, nr) {}

  /// Initialize size.
  /// \param s  Rectangular size of image.
  basic_image(regfill::size s): size_(s) {}

public:
  /// Dimensions of image.
  /// \return  Dimensions of image.
  regfill::size size() const { return size_; }
};


} // namespace regfill

#endif // ndef REGFILL_BASIC_IMAGE_HPP

// EOF
