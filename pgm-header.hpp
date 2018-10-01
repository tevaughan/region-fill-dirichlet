
#ifndef REGFILL_PGM_HEADER_HPP
#define REGFILL_PGM_HEADER_HPP

#include "format.hpp"
#include <iostream>
#include <string>

namespace regfill {

class pgm_header {
  uint16_t num_cols_;
  uint16_t num_rows_;
  uint16_t max_val_;

public:
  pgm_header() = default;

  pgm_header(uint16_t c, uint16_t r, uint16_t m)
      : num_cols_(c), num_rows_(r), max_val_(m) {}

  std::istream &read(std::istream &is) {
    std::string m;
    is >> m;
    if (m != "P5") {
      throw "magic '" + m + "' not 'P5'";
    }
    if (!(is >> num_cols_)) {
      throw std::string("problem reading num_cols");
    }
    if (!(is >> num_rows_)) {
      throw std::string("problem reading num_rows");
    }
    if (!(is >> max_val_)) {
      throw std::string("problem reading max_val");
    }
    char c;
    if (!is.get(c)) {
      throw std::string("problem reading character after maxval");
    }
    if (c != ' ' && c != '\t' & c != '\n') {
      throw format("character %X after maxval not white space", c);
    }
    return is;
  }

  pgm_header(std::istream &is) { read(is); }

  std::ostream &write(std::ostream &os) {
    return os << format("P5\n%u %u\n%u\n", num_cols_, num_rows_, max_val_);
  }

  uint16_t num_cols() const { return num_cols_; }
  uint16_t num_rows() const { return num_rows_; }
  uint16_t max_val() const { return max_val_; }
  unsigned num_pix() const { return num_cols_ * num_rows_; }
};

} // namespace regfill

#endif // ndef REGFILL_PGM_HEADER_HPP

