
#ifndef REGFILL_IMAGE_HPP
#define REGFILL_IMAGE_HPP

#include "pcoord.hpp"
#include "pgm-header.hpp"
#include <fstream>
#include <vector>

namespace regfill {

class image {
  std::vector<float> pix_;
  uint16_t num_cols_;

public:
  image() = default;
  image(std::string fn);
  image(uint16_t nc, uint16_t nr, float v = 0.0f);
  std::istream &read(std::istream &is);
  uint16_t num_cols() const { return num_cols_; }
  uint16_t num_rows() const { return pix_.size() / num_cols_; }
  unsigned num_pix() const { return pix_.size(); }
  std::ostream &write(std::ostream &os);
  float &operator()(pcoord p) { return pix_[p.lin(num_cols_)]; }
  float const &operator()(pcoord p) const { return pix_[p.lin(num_cols_)]; }
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

} // namespace regfill

#endif // ndef REGFILL_IMAGE_HPP

