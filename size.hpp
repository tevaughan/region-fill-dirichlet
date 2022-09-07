/// \file       size.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::size.

#ifndef REGFILL_SIZE_HPP
#define REGFILL_SIZE_HPP

#include "coords.hpp" // coords

namespace regfill {


/// Size of image.
struct size: protected coords {
  // Inherit constructors from coords.
  using coords::coords;

  /// Number of columns in image.
  /// \return  Number of columns in image.
  uint16_t cols() const { return col; }

  /// Number of rows in image.
  /// \return  Number of rows in image.
  uint16_t rows() const { return row; }

  /// Number of columns in image.
  /// \return  Number of columns in image.
  uint16_t &cols() { return col; }

  /// Number of rows in image.
  /// \return  Number of rows in image.
  uint16_t &rows() { return row; }

  /// Linear size of image.
  /// \return  Linear size of image.
  uint32_t lin() const { return col * row; }

  /// Linear offset pixel in image.
  /// \param p  Rectangular offsets of pixel in image.
  /// \return   Linear offset of pixel in image.
  uint32_t lin(coords p) const;

  /// Rectangular offsets of pixel in image.
  /// \param off  Linear offset of pixel in image.
  /// \return     Rectangular offset of pixel in image.
  coords rct(uint32_t off) const;
};


} // namespace regfill

#include "format.hpp" // format()

namespace regfill {


uint32_t size::lin(coords p) const {
  if(p.col >= col) { throw format("illegal col %u > %u", p.col, col); }
  unsigned const off= p.row * col + p.col;
  if(off >= lin()) { throw format("illegal row %u > %u", p.row, row); }
  return off;
}


coords size::rct(unsigned off) const {
  uint16_t const c= off % col;
  uint16_t const r= off / col;
  return {c, r};
}


} // namespace regfill

#endif // ndef REGFILL_SIZE_HPP

// EOF
