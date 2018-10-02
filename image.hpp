
#ifndef REGFILL_IMAGE_HPP
#define REGFILL_IMAGE_HPP

#include "pgm-header.hpp"
#include <cmath>
#include <fstream>
#include <vector>

namespace regfill {

struct pcoord {
  uint16_t col;
  uint16_t row;
  bool operator==(pcoord p) const { return col == p.col && row == p.row; }
  bool operator!=(pcoord p) const { return col != p.col || row != p.row; }
};

class image {
  std::vector<float> pix_;
  unsigned num_cols_;

public:
  image() = default;
  image(std::string fn);
  image(uint16_t nc, uint16_t nr, float v = 0.0f);
  unsigned lin(uint16_t c, uint16_t r) const;
  std::istream &read(std::istream &is);
  uint16_t num_cols() const { return num_cols_; }
  uint16_t num_rows() const { return pix_.size() / num_cols_; }
  unsigned num_pix() const { return pix_.size(); }
  std::ostream &write(std::ostream &os) const;
  void write(std::string fn) const;
  float &operator()(uint16_t c, uint16_t r) { return pix_[lin(c, r)]; }
  float const &operator()(uint16_t c, uint16_t r) const;
  void draw_polyline(std::vector<pcoord> p, float v);
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

inline unsigned image::lin(uint16_t c, uint16_t r) const {
  if (c >= num_cols_) {
    throw format("illegal col %u > %u", c, num_cols_);
  }
  unsigned const off = r * num_cols_ + c;
  if (off >= pix_.size()) {
    throw format("illegal row %u > %u", r, num_rows());
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

inline float const &image::operator()(uint16_t c, uint16_t r) const {
  return pix_[lin(c, r)];
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
    (*this)(p[i].col, p[i].row) = v; // Draw current end-point.
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
        (*this)(uint16_t(x + 0.5), uint16_t(y + 0.5)) = v;
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
        (*this)(uint16_t(y + 0.5), uint16_t(x + 0.5)) = v;
      }
    }
    i = j;
  } while (i < p.size());
}

} // namespace regfill

#endif // ndef REGFILL_IMAGE_HPP

