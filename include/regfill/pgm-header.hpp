/// \file       pgm-header.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::pgm_header.

#pragma once

#include "format.hpp" // format()
#include "size.hpp"   // size
#include <iostream>   // istream, ostream

namespace regfill {


using std::istream;
using std::ostream;


/// Read or write header of PGM-file.
class pgm_header {
  regfill::size size_; ///< Dimensions of image.
  uint16_t      max_;  ///< Maximum value of pixel in image.

public:
  pgm_header()= default; ///< By default leave uninitialized on construction.

  /// Initialize header according to specified contents.
  /// \param c  Number of columns in image.
  /// \param r  Number of rows in image.
  /// \param m  Maximum value of pixel in image.
  pgm_header(uint16_t c, uint16_t r, uint16_t m): size_(c, r), max_(m) {}

  /// Initialize header by reading from stream attached to PGM-file.
  /// \param is  Reference to input-stream.
  pgm_header(istream &is) { read(is); }

  /// Set values in header by reading from stream attached to PGM-file.
  /// \param is  Reference to input-stream.
  istream &read(istream &is);

  /// Write PGM-header to output-stream.
  /// \param os  Reference to output-stream.
  ostream &write(ostream &os) {
    return os << format("P5\n%u %u\n%u\n", size_.cols(), size_.rows(), max_);
  }

  /// Dimensions of image.
  /// \return  Dimensions of image.
  regfill::size size() const { return size_; }

  /// Maximum value of pixel in image.
  /// \return  Maximum value of pixel in image.
  uint16_t max() const { return max_; }
};


} // namespace regfill

#include <string> // string

namespace regfill {


using std::string;


inline std::istream &pgm_header::read(istream &is) {
  string m;
  is >> m;
  if(m != "P5") { throw "magic '" + m + "' not 'P5'"; }
  if(!(is >> size_.cols())) { throw string("problem reading num_cols"); }
  if(!(is >> size_.rows())) { throw string("problem reading num_rows"); }
  if(!(is >> max_)) { throw string("problem reading max_val"); }
  char c;
  if(!is.get(c)) { throw string("problem reading character after maxval"); }
  if(c != ' ' && c != '\t' && c != '\n') { throw format("%X not whtspc", c); }
  return is;
}


} // namespace regfill

// EOF
