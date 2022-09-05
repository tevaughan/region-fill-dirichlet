/// \copyright  2018-2022 Thomas E. Vaughan
/// \brief      Definition of regfill::pgm_header.

#pragma once

#include "format.hpp" // format()
#include <iostream>   // istream, ostream

namespace regfill {


using std::istream;
using std::ostream;


/// Read or write header of PGM-file.
class pgm_header {
  uint16_t num_cols_; ///< Number of columns in image.
  uint16_t num_rows_; ///< Number of rows in image.
  uint16_t max_val_;  ///< Maximum value of pixel in image.

public:
  pgm_header()= default; ///< By default leave uninitialized on construction.

  /// Initialize header according to specified contents.
  /// \param c  Number of columns in image.
  /// \param r  Number of rows in image.
  /// \param m  Maximum value of pixel in image.
  pgm_header(uint16_t c, uint16_t r, uint16_t m):
      num_cols_(c), num_rows_(r), max_val_(m) {}

  /// Initialize header by reading from stream attached to PGM-file.
  /// \param is  Reference to input-stream.
  pgm_header(istream &is) { read(is); }

  /// Set values in header by reading from stream attached to PGM-file.
  /// \param is  Reference to input-stream.
  istream &read(istream &is);

  /// Write PGM-header to output-stream.
  /// \param os  Reference to output-stream.
  ostream &write(ostream &os) {
    return os << format("P5\n%u %u\n%u\n", num_cols_, num_rows_, max_val_);
  }

  /// Number of columns in image.
  /// \return  Number of columns in image.
  uint16_t num_cols() const { return num_cols_; }

  /// Number of rows in image.
  /// \return  Number of rows in image.
  uint16_t num_rows() const { return num_rows_; }

  /// Maximum value of pixel in image.
  /// \return  Maximum value of pixel in image.
  uint16_t max_val() const { return max_val_; }

  /// Number of pixels in image.
  /// \return  Number of pixels in image.
  unsigned num_pix() const { return num_cols_ * num_rows_; }
};


} // namespace regfill

#include <string> // string

namespace regfill {


using std::string;


inline std::istream &pgm_header::read(istream &is) {
  string m;
  is >> m;
  if(m != "P5") { throw "magic '" + m + "' not 'P5'"; }
  if(!(is >> num_cols_)) { throw string("problem reading num_cols"); }
  if(!(is >> num_rows_)) { throw string("problem reading num_rows"); }
  if(!(is >> max_val_)) { throw string("problem reading max_val"); }
  char c;
  if(!is.get(c)) { throw string("problem reading character after maxval"); }
  if(c != ' ' && c != '\t' && c != '\n') { throw format("%X not whtspc", c); }
  return is;
}


} // namespace regfill

// EOF
