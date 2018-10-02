
#ifndef REGFILL_IMAGE_HPP
#define REGFILL_IMAGE_HPP

#include "pgm-header.hpp"
#include <fstream>
#include <vector>

namespace regfill {

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
  std::ostream &write(std::ostream &os);
  float &operator()(uint16_t c, uint16_t r) { return &pix_[lin(c, r)]; }
  float const &operator()(uint16_t c, uint16_t r) const;
  void draw_polyline(vector<pcoord> const &p, float v);
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

inline std::ostream &image::write(std::ostream &os) {
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

inline float const &image::operator()(uint16_t c, uint16_t r) const {
  return pix_[lin(c, r)];
}

inline void image::draw_polyline(vector<pcoord> const &p, float v) {
  if (p.size() == 0) {
    return;
  }
  vector<pcoord> q;
  q.push_back(p[0]);
  for (unsigned i = 1; i < p.size(); ++i) {
    if (p[i] == p[i - 1]) {
      continue; // Remove consecutive duplicates.
    }
    q.push_back(p[i]);
  }
  if (q.size() == 1) {
    (*this)(p[0]) = v;
    return;
  }
  if (q.front() != q.back()) {
    q.push_back(q.front());
  }
  for (unsigned i = 1; i < q.size(); ++i) {
    // TBD: Color every pixel through which line passes.
  }
}

} // namespace regfill

#endif // ndef REGFILL_IMAGE_HPP

