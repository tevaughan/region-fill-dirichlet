
#ifndef REGFILL_IMAGE_HPP
#define REGFILL_IMAGE_HPP

#include "pgm-header.hpp"
#include <cmath>
#include <fstream>
#include <vector>

namespace regfill {

/// Coordinates of pixel.
struct pcoord {
  uint16_t col; ///< Offset of column.
  uint16_t row; ///< Offset of row.

  /// Test equality of coordinates.
  bool operator==(pcoord p) const { return col == p.col && row == p.row; }

  /// Test inequality of coordinates.
  bool operator!=(pcoord p) const { return col != p.col || row != p.row; }
};

/// Gray-scale image.
class image {
  std::vector<float> pix_; ///< Storage for pixel-values.
  unsigned num_cols_;      ///< Number of columns in image.

public:
  image() = default;     ///< By default, don't initialize anything.
  image(std::string fn); ///< Initialize image from PGM file.

  /// Initialize solid-gray image, by default black.
  image(uint16_t nc, uint16_t nr, float v = 0.0f);

  unsigned lin(pcoord p) const;         ///< Linear offset of pixel.
  std::istream &read(std::istream &is); ///< Read PGM data from stream.

  /// Number of columns in image.  Value is simply retrieved.
  uint16_t num_cols() const { return num_cols_; }

  /// Number of rows in image.  Value is computed when num_rows() called.
  uint16_t num_rows() const { return pix_.size() / num_cols_; }

  /// Number of pixels in image.
  unsigned num_pix() const { return pix_.size(); }

  /// Write PGM data to stream.
  std::ostream &write(std::ostream &os) const;

  void write(std::string fn) const; ///< Write PGM data to file.

  /// Reference to value of pixel.
  float &pixel(pcoord p) { return pix_[lin(p)]; }

  /// Immutable reference to value of pixel.
  float const &pixel(pcoord p) const { return pix_[lin(p)]; }

  /// Reference to value of pixel.
  float &operator()(pcoord p) { return pixel(p); }

  /// Immutable reference to value of pixel.
  float const &operator()(pcoord p) const { return pixel(p); }

  /// Draw closed polygonal perimeter.
  void draw_polyline(std::vector<pcoord> p, float v);

  /// Start from supplied coordinates, and fill outward with value v until
  /// closed border with value v is found.
  void fill(pcoord p, float v);
};

inline image::image(std::string fn) {
  std::ifstream ifs(fn);
  if (!ifs) {
    throw "problem opening '" + fn + "'";
  }
  read(ifs);
}

inline image::image(uint16_t nc, uint16_t nr, float v)
    : pix_(nc * nr, v), num_cols_(nc) {}

inline unsigned image::lin(pcoord p) const {
  if (p.col >= num_cols_) {
    throw format("illegal col %u > %u", p.col, num_cols_);
  }
  unsigned const off = p.row * num_cols_ + p.col;
  if (off >= pix_.size()) {
    throw format("illegal row %u > %u", p.row, num_rows());
  }
  return off;
}

inline std::istream &image::read(std::istream &is) {
  pgm_header h(is);
  num_cols_ = h.num_cols();
  unsigned const num_pix = h.num_cols() * h.num_rows();
  pix_.resize(num_pix);
  bool found_max = false;
  for (unsigned i = 0; i < num_pix; ++i) {
    pix_[i] = is.get();
    if (!is) {
      throw format("error reading byte %u of image (after header)", i);
    }
    if (pix_[i] > h.max_val()) {
      throw format("max val %f (at %d) > %d", pix_[i], i, h.max_val());
    } else if (pix_[i] == h.max_val()) {
      found_max = true;
    }
  }
  if (found_max == false) {
    throw format("max val %d > value of every pixel", h.max_val());
  }
  return is;
}

inline std::ostream &image::write(std::ostream &os) const {
  float max_val = 0.0f, min_val = 65536.0f;
  for (auto i = pix_.begin(); i != pix_.end(); ++i) {
    if (*i > max_val) {
      max_val = *i;
    }
    if (*i < min_val) {
      min_val = *i;
    }
  }
  enum { MAX = 255 };
  pgm_header(num_cols_, num_rows(), MAX).write(os);
  float const c = (MAX + 0.99f) / (max_val - min_val);
  for (auto i = pix_.begin(); i != pix_.end(); ++i) {
    os.put(uint8_t(c * (*i - min_val)));
  }
  return os;
}

inline void image::write(std::string fn) const {
  std::ofstream ofs(fn);
  if (!ofs) {
    throw format("error opening '%s' for output", fn.c_str());
  }
  write(ofs);
}

inline void image::draw_polyline(std::vector<pcoord> p, float v) {
  if (p.size() == 0) {
    return;
  }
  if (p.front() != p.back()) {
    p.push_back(p.front());
  }
  unsigned i = 0;
  do {
    pixel(p[i]) = v; // Draw current end-point.
    unsigned j;
    for (j = i + 1; j < p.size(); ++j) {
      if (p[j] != p[i]) {
        break; // Found next point to draw to.
      }
    }
    if (j == p.size()) {
      break; // No next point to draw to.
    }
    double const c1 = p[i].col;
    double const r1 = p[i].row;
    double const c2 = p[j].col;
    double const r2 = p[j].row;
    double const dc = c2 - c1;
    double const dr = r2 - r1;
    double const adc = fabs(dc);
    double const adr = fabs(dr);
    if (adc > adr) {
      double const m = dr / dc;
      double x1, x2, y1;
      if (dc > 0.0) {
        x1 = c1;
        x2 = c2;
        y1 = r1;
      } else {
        x1 = c2;
        x2 = c1;
        y1 = r2;
      }
      for (double x = x1; x < x2; x += 1.0) {
        double const y = y1 + m * (x - x1);
        pixel({uint16_t(x + 0.5), uint16_t(y + 0.5)}) = v;
      }
    } else {
      double const m = dc / dr;
      double x1, x2, y1;
      if (dr > 0.0) {
        x1 = r1;
        x2 = r2;
        y1 = c1;
      } else {
        x1 = r2;
        x2 = r1;
        y1 = c2;
      }
      for (double x = x1; x < x2; x += 1.0) {
        double const y = y1 + m * (x - x1);
        pixel({uint16_t(y + 0.5), uint16_t(x + 0.5)}) = v;
      }
    }
    i = j;
  } while (i < p.size());
}

inline void image::fill(pcoord p, float v) {
  if (pixel(p) == v) {
    return;
  }
  pixel(p) = v;
  if (int(p.col) < int(num_cols_) - 1) {
    fill({uint16_t(p.col + 1), p.row}, v);
  }
  if (int(p.row) < int(num_rows()) - 1) {
    fill({p.col, uint16_t(p.row + 1)}, v);
  }
  if (p.col > 0) {
    fill({uint16_t(p.col - 1), p.row}, v);
  }
  if (p.row > 0) {
    fill({p.col, uint16_t(p.row - 1)}, v);
  }
}

} // namespace regfill

#endif // ndef REGFILL_IMAGE_HPP

