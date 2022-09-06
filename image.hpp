/// \file       image.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::image.

#ifndef REGFILL_IMAGE_HPP
#define REGFILL_IMAGE_HPP

#include "pcoord.hpp" // pcoord
#include <iostream>   // istream, ostream
#include <vector>     // vector

namespace regfill {


using std::istream;
using std::ostream;
using std::string;
using std::vector;


/// Gray-scale image that can be initialized from a PGM-file.
class image {
  vector<float> pix_;      ///< Storage for pixel-values.
  unsigned      num_cols_; ///< Number of columns in image.

  /// Rectangular offsets of pixel.
  /// \param off  Linear offset of pixel.
  /// \return     Rectangular offsets of pixel.
  pcoord rct(unsigned off) const;

  /// Draw line by stepping through columns.
  /// This function will behave properly only if `|dc| >= |dr|`.
  /// \param p   Rectangular offsets of starting point.
  /// \param dc  Length of line in columns (may be negative).
  /// \param dr  Length of line in rows (may be negative).
  /// \param v   Intensity of pixels along line.
  void draw_line_by_cols(pcoord p, float dc, float dr, float v);

  /// Draw line by stepping through rows.
  /// This function will behave properly only if `|dr| >= |dc|`.
  /// \param p   Rectangular offsets of starting point.
  /// \param dc  Length of line in columns (may be negative).
  /// \param dr  Length of line in rows (may be negative).
  /// \param v   Intensity of pixels along line.
  void draw_line_by_rows(pcoord p, float dc, float dr, float v);

  /// Check that `mask` has same size as this image.
  /// Throw exception if `mask` be of different size.
  /// \param mask  Mask used by laplacian_fill().
  void check_mask_size(image const &mask);

public:
  image()= default; ///< By default, don't initialize anything.

  /// Initialize image from PGM file.
  /// \param fn  Name of file.
  image(string fn);

  /// Initialize solid-gray image, by default black.
  /// \param nc  Number of columns.
  /// \param nr  Number of rows.
  /// \param v   Default intensity for each pixel.
  image(uint16_t nc, uint16_t nr, float v= 0.0f):
      pix_(nc * nr, v), num_cols_(nc) {}

  /// Linear offset of pixel.
  /// \param p  Rectangular offsets of pixel.
  /// \return   Linear offset of pixel.
  unsigned lin(pcoord p) const;

  /// Read PGM-data from stream.
  /// \param is  Reference to input-stream.
  /// \return    Reference to input-stream.
  istream &read(istream &is);

  /// Number of columns in image.  Value is simply retrieved.
  /// \return  Number of columns in image.
  uint16_t num_cols() const { return num_cols_; }

  /// Number of rows in image.  Value is computed when num_rows() called.
  /// \return  Number of rows in image.
  uint16_t num_rows() const { return pix_.size() / num_cols_; }

  /// Number of pixels in image.
  /// \return  Number of pixels in image.
  unsigned num_pix() const { return pix_.size(); }

  /// Write PGM data to stream.
  /// \param os  Reference to output-stream.
  /// \return    Reference to output-stream.
  ostream &write(ostream &os) const;

  /// Write PGM-data to file.
  /// \param fn  Name of output-file.
  void write(string fn) const;

  /// Reference to value of pixel.
  /// \param p  Rectangular offsets of pixel.
  /// \return   Reference to value of pixel.
  float &pixel(pcoord p) { return pix_[lin(p)]; }

  /// Immutable reference to value of pixel.
  /// \param p  Rectangular offsets of pixel.
  /// \return   Reference to value of pixel.
  float const &pixel(pcoord p) const { return pix_[lin(p)]; }

  /// Reference to value of pixel.
  /// \param p  Rectangular offsets of pixel.
  /// \return   Reference to value of pixel.
  float &operator()(pcoord p) { return pixel(p); }

  /// Immutable reference to value of pixel.
  /// \param p  Rectangular offsets of pixel.
  /// \return   Reference to value of pixel.
  float const &operator()(pcoord p) const { return pixel(p); }

  /// Draw closed polygonal perimeter.
  /// \param p  Vertices of polygon.
  /// \param v  Intensity of pixel along edges of polygon.
  void draw_polyline(vector<pcoord> p, float v);

  /// Start from supplied coordinates, and fill outward with value v until
  /// closed border with value v is found.
  /// \param p  Rectangular offsets of starting point.
  /// \param v  Intensity with which to fill pixels.
  void fill(pcoord p, float v);

  /// Find pixels, each with intensity greater than v (zero by default).
  /// \param v  Intensity of threshold.
  /// \return   Coordinates of every pixel with intensity greater than `v`.
  vector<pcoord> threshold(float v= 0.0f) const;

  /// Find every pixel that has both
  /// (1) its value *not* above threshold intensity `v` and
  /// (2) at least one of four neighbors whose value *is* above `v`.
  /// \param v  Intensity of threshold.
  /// \return   Coordinates of every pixel along boundary of threshold-image.
  vector<pcoord> boundary(float v= 0.0f) const;

  /// Use Laplace to interpolate pixels identified by nonzero pixels in mask.
  ///
  /// `mask` must be image of same size as this image.
  ///
  /// After function returns, this image has been modified by interpolation.
  /// Every pixel in this image has been interpolated if the corresponding
  /// pixel in `mask` have nonzero intensity.
  ///
  /// \param mask  Image indicating which pixels in this image to interpolate.
  void laplacian_fill(image const &mask);
};


} // namespace regfill

#include "pgm-header.hpp"       // pgm_header
#include "threshold-coords.hpp" // threshold_coords
#include <eigen3/Eigen/Dense>   // Triplet, VectorXd
#include <eigen3/Eigen/Sparse>  // SparseMatrix, SimplicialCholesky
#include <fstream>              // ifstream, ofstream

namespace regfill {


using Eigen::SimplicialCholesky;
using Eigen::SparseMatrix;
using Eigen::Triplet;
using Eigen::VectorXd;
using std::ifstream;
using std::map;
using std::ofstream;


inline image::image(string fn) {
  ifstream ifs(fn);
  if(!ifs) { throw "problem opening '" + fn + "'"; }
  read(ifs);
}


inline unsigned image::lin(pcoord p) const {
  if(p.col >= num_cols_) {
    throw format("illegal col %u > %u", p.col, num_cols_);
  }
  unsigned const off= p.row * num_cols_ + p.col;
  if(off >= pix_.size()) {
    throw format("illegal row %u > %u", p.row, num_rows());
  }
  return off;
}


inline istream &image::read(istream &is) {
  pgm_header h(is);
  num_cols_             = h.num_cols();
  unsigned const num_pix= h.num_cols() * h.num_rows();
  pix_.resize(num_pix);
  bool found_max= false;
  for(unsigned i= 0; i < num_pix; ++i) {
    pix_[i]= is.get();
    if(!is) {
      throw format("error reading byte %u of image (after header)", i);
    }
    if(pix_[i] > h.max_val()) {
      throw format("max val %f (at %d) > %d", pix_[i], i, h.max_val());
    } else if(pix_[i] == h.max_val()) {
      found_max= true;
    }
  }
  if(found_max == false) {
    throw format("max val %d > value of every pixel", h.max_val());
  }
  return is;
}


inline ostream &image::write(ostream &os) const {
  float max_val= 0.0f, min_val= 65536.0f;
  for(auto i= pix_.begin(); i != pix_.end(); ++i) {
    if(*i > max_val) { max_val= *i; }
    if(*i < min_val) { min_val= *i; }
  }
  enum { MAX= 255 };
  pgm_header(num_cols_, num_rows(), MAX).write(os);
  float const c= (MAX + 0.99f) / (max_val - min_val);
  for(auto i= pix_.begin(); i != pix_.end(); ++i) {
    os.put(uint8_t(c * (*i - min_val)));
  }
  return os;
}


inline void image::write(string fn) const {
  ofstream ofs(fn);
  if(!ofs) { throw format("error opening '%s' for output", fn.c_str()); }
  write(ofs);
}


inline void image::draw_line_by_cols(pcoord p, float dc, float dr, float v) {
  float const m = dr / dc;
  float const c2= p.col + dc;
  float const r2= p.row + dr;
  float       x1, x2, y1;
  if(dc > 0.0f) {
    x1= p.col;
    x2= c2;
    y1= p.row;
  } else {
    x1= c2;
    x2= p.col;
    y1= r2;
  }
  for(float x= x1; x < x2; x+= 1.0f) {
    pixel({x + 0.5f, y1 + m * (x - x1) + 0.5f})= v;
  }
}


inline void image::draw_line_by_rows(pcoord p, float dc, float dr, float v) {
  float const m = dc / dr;
  float const c2= p.col + dc;
  float const r2= p.row + dr;
  float       x1, x2, y1;
  if(dr > 0.0f) {
    x1= p.row;
    x2= r2;
    y1= p.col;
  } else {
    x1= r2;
    x2= p.row;
    y1= c2;
  }
  for(float x= x1; x < x2; x+= 1.0f) {
    pixel({y1 + m * (x - x1) + 0.5f, x + 0.5f})= v;
  }
}


inline void image::draw_polyline(vector<pcoord> p, float v) {
  if(p.size() == 0) { return; }
  if(p.front() != p.back()) { p.push_back(p.front()); }
  unsigned i= 0;
  do {
    pixel(p[i])= v; // Draw current end-point.
    unsigned j;
    for(j= i + 1; j < p.size(); ++j) {
      if(p[j] != p[i]) break; // Found next point to draw to.
    }
    if(j == p.size()) break; // No next point to draw to.
    int const      dc = int(p[j].col) - int(p[i].col);
    int const      dr = int(p[j].row) - int(p[i].row);
    unsigned const adc= (dc < 0 ? -dc : +dc);
    unsigned const adr= (dr < 0 ? -dr : +dr);
    if(adc > adr) {
      draw_line_by_cols(p[i], dc, dr, v);
    } else {
      draw_line_by_rows(p[i], dc, dr, v);
    }
    i= j;
  } while(i < p.size());
}


inline void image::fill(pcoord p, float v) {
  if(pixel(p) == v) { return; }
  pixel(p)= v;
  if(int(p.col) < int(num_cols_) - 1) {
    fill({uint16_t(p.col + 1), p.row}, v);
  }
  if(int(p.row) < int(num_rows()) - 1) {
    fill({p.col, uint16_t(p.row + 1)}, v);
  }
  if(p.col > 0) { fill({uint16_t(p.col - 1), p.row}, v); }
  if(p.row > 0) { fill({p.col, uint16_t(p.row - 1)}, v); }
}


pcoord image::rct(unsigned off) const {
  uint16_t const c= off % num_cols_;
  uint16_t const r= off / num_cols_;
  return {c, r};
}


vector<pcoord> image::threshold(float v) const {
  vector<pcoord> s;
  for(auto i= pix_.cbegin(); i != pix_.end(); ++i) {
    if(*i > v) s.push_back(rct(i - pix_.begin()));
  }
  return s;
}


vector<pcoord> image::boundary(float v) const {
  vector<pcoord> b;
  int const      nc= num_cols_;
  int const      nr= num_rows();
  for(auto i= pix_.begin(); i != pix_.end(); ++i) {
    // Do nothing on current iteration if current pixel be above threshold.
    if(*i > v) continue;
    // Current pixel is *not* above threshold.  So if neighbor be above
    // threshold, then mark current pixel as on boundary.
    pcoord const p= rct(i - pix_.begin());
    int const    c= p.col;
    int const    r= p.row;
    if(c < nc - 1 && pixel({c + 1, r}) > v) {
      b.push_back(p);
    } else if(r < nr - 1 && pixel({c, r + 1}) > v) {
      b.push_back(p);
    } else if(c > 0 && pixel({c - 1, r}) > v) {
      b.push_back(p);
    } else if(r > 0 && pixel({c, r - 1}) > v) {
      b.push_back(p);
    }
  }
  return b;
}


void image::check_mask_size(image const &mask) {
  uint16_t const inc= num_cols();
  uint16_t const inr= num_rows();
  uint16_t const mnc= mask.num_cols();
  uint16_t const mnr= mask.num_rows();
  if(inc != mnc || inr != mnr) {
    throw format("%ux%u for image, but %ux%u for mask", inc, inr, mnc, mnr);
  }
}


/// Offsets and flags for neighbors of a pixel.
struct neighbors {
  uint16_t const rb; ///< Row-offset below central pixel.
  uint16_t const rt; ///< Row-offset above central pixel.
  uint16_t const cl; ///< Column-offset left of central pixel.
  uint16_t const cr; ///< Column-offset right of central pixel.
  bool const     fb; ///< True only if there be row below central pixel.
  bool const     ft; ///< True only if there be row above central pixel.
  bool const     fl; ///< True only if there be column left of central pixel.
  bool const     fr; ///< True only if there be column right of central pixel.

public:
  /// Initialize offsets and flags for neighbors.
  /// \param nc   Number of columns in mask.
  /// \param nr   Number of rows in mask.
  /// \param cen  Rectangular coordinates of pixel central to neighbors.
  neighbors(int nc, int nr, pcoord cen):
      rb(cen.row + 1),
      rt(cen.row - 1),
      cl(cen.col - 1),
      cr(cen.col + 1),
      fb(int(cen.row) < nr - 1),
      ft(cen.row > 0),
      fl(cen.col > 0),
      fr(int(cen.col) < nc - 1) {}
};


class cholesky_coefs {
  image const &           im_;
  threshold_coords        thresh_;
  VectorXd                b_;
  vector<Triplet<double>> coefs_;

  void f(pcoord p, unsigned i, double w) {
    auto const it= thresh_.map().find(im_.lin(p));
    if(it == thresh_.map().end()) {
      b_(i)+= w * im_.pixel(p);
    } else {
      coefs_.push_back({int(i), int(it->second), -w});
    }
  }

  constexpr static double w_side() { return 0.50 / 3.0; }
  constexpr static double w_diag() { return 0.25 / 3.0; }

public:
  /// Initialize from coordinates above threshold in mask.
  cholesky_coefs(image const &im, image const &mask):
      im_(im), thresh_(mask), b_(thresh_.crd().size()) {
    int const nc= mask.num_cols();
    int const nr= mask.num_rows();
    for(unsigned i= 0; i < thresh_.crd().size(); ++i) {
      b_(i)= 0.0;
      coefs_.push_back({int(i), int(i), 1.0});
      pcoord const    cc= thresh_.crd()[i];
      neighbors const nb(nc, nr, cc);
      if(nb.fb) f({cc.col, nb.rb}, i, w_side());
      if(nb.ft) f({cc.col, nb.rt}, i, w_side());
      if(nb.fr) {
        f({nb.cr, cc.row}, i, w_side());
        if(nb.fb) f({nb.cr, nb.rb}, i, w_diag());
        if(nb.ft) f({nb.cr, nb.rt}, i, w_diag());
      }
      if(nb.fl) {
        f({nb.cl, cc.row}, i, w_side());
        if(nb.fb) f({nb.cl, nb.rb}, i, w_diag());
        if(nb.ft) f({nb.cl, nb.rt}, i, w_diag());
      }
    }
  }

  auto        begin() const { return coefs_.begin(); }
  auto        end() const { return coefs_.end(); }
  auto const &thresh() const { return thresh_; }
  auto const &b() const { return b_; }
};


void image::laplacian_fill(image const &mask) {
  check_mask_size(mask);
  // Calculate coefficients for matrix.
  cholesky_coefs coefs(*this, mask);
  // Get list of coordinates above threshold in mask.
  auto const &crd= coefs.thresh().crd();
  // Get number of coordinates above threshold in mask.
  unsigned const n= crd.size();
  // Set up sparse matrix.
  using mat= SparseMatrix<double>;
  mat a(n, n);
  a.setFromTriplets(coefs.begin(), coefs.end());
  SimplicialCholesky<mat> chol(a);
  // Solve linear system.
  VectorXd const x= chol.solve(coefs.b());
  // Copy filled values into image.
  for(unsigned i= 0; i < n; ++i) { pixel(crd[i])= x[i]; }
}


} // namespace regfill

#endif // ndef REGFILL_IMAGE_HPP

// EOF
