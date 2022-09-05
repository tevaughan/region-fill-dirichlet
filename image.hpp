/// \file       image.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of regfill::image.

#pragma once

#include "pcoord.hpp"     // pccord
#include "pgm-header.hpp" // pgm_header
#include <cmath>
#include <vector>

namespace regfill {


using std::istream;
using std::ostream;
using std::string;
using std::vector;


/// Gray-scale image.
class image {
  vector<float> pix_;      ///< Storage for pixel-values.
  unsigned      num_cols_; ///< Number of columns in image.

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

  /// Find boundary of threshold-image above intensity v (zero by default).
  /// \param v  Intensity of threshold.
  /// \return   Coordinates of every along boundary of threshold-image.
  vector<pcoord> boundary(float v= 0.0f) const;

  /// Use Laplace to fill pixels identified by nonzero pixels in mask.
  void laplacian_fill(image const &mask);
};


} // namespace regfill

#include <eigen3/Eigen/Dense>  // Triplet, VectorXd
#include <eigen3/Eigen/Sparse> // SparseMatrix, SimplicialCholesky
#include <fstream>             // ifstream, ofstream

namespace regfill {


using Eigen::SimplicialCholesky;
using Eigen::SparseMatrix;
using Eigen::Triplet;
using Eigen::VectorXd;
using std::ifstream;
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


inline void image::draw_polyline(vector<pcoord> p, float v) {
  if(p.size() == 0) { return; }
  if(p.front() != p.back()) { p.push_back(p.front()); }
  unsigned i= 0;
  do {
    pixel(p[i])= v; // Draw current end-point.
    unsigned j;
    for(j= i + 1; j < p.size(); ++j) {
      if(p[j] != p[i]) {
        break; // Found next point to draw to.
      }
    }
    if(j == p.size()) {
      break; // No next point to draw to.
    }
    double const c1 = p[i].col;
    double const r1 = p[i].row;
    double const c2 = p[j].col;
    double const r2 = p[j].row;
    double const dc = c2 - c1;
    double const dr = r2 - r1;
    double const adc= fabs(dc);
    double const adr= fabs(dr);
    if(adc > adr) {
      double const m= dr / dc;
      double       x1, x2, y1;
      if(dc > 0.0) {
        x1= c1;
        x2= c2;
        y1= r1;
      } else {
        x1= c2;
        x2= c1;
        y1= r2;
      }
      for(double x= x1; x < x2; x+= 1.0) {
        double const y                               = y1 + m * (x - x1);
        pixel({uint16_t(x + 0.5), uint16_t(y + 0.5)})= v;
      }
    } else {
      double const m= dc / dr;
      double       x1, x2, y1;
      if(dr > 0.0) {
        x1= r1;
        x2= r2;
        y1= c1;
      } else {
        x1= r2;
        x2= r1;
        y1= c2;
      }
      for(double x= x1; x < x2; x+= 1.0) {
        double const y                               = y1 + m * (x - x1);
        pixel({uint16_t(y + 0.5), uint16_t(x + 0.5)})= v;
      }
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


vector<pcoord> image::threshold(float v) const {
  vector<pcoord> s;
  for(auto i= pix_.cbegin(); i != pix_.end(); ++i) {
    if(*i > v) {
      unsigned const off= i - pix_.begin();
      s.push_back({uint16_t(off % num_cols_), uint16_t(off / num_cols_)});
    }
  }
  return s;
}


vector<pcoord> image::boundary(float v) const {
  vector<pcoord> b;
  int const      nc= num_cols_;
  int const      nr= num_rows();
  for(auto i= pix_.cbegin(); i != pix_.end(); ++i) {
    if(*i > v) { continue; }
    unsigned const off= i - pix_.begin();
    uint16_t const c  = off % num_cols_;
    uint16_t const r  = off / num_cols_;
    if(int(c) < nc - 1 && pixel({uint16_t(c + 1), r}) > v) {
      b.push_back({c, r});
      continue;
    }
    if(int(r) < nr - 1 && pixel({c, uint16_t(r + 1)}) > v) {
      b.push_back({c, r});
      continue;
    }
    if(c > 0 && pixel({uint16_t(c - 1), r}) > v) {
      b.push_back({c, r});
      continue;
    }
    if(r > 0 && pixel({c, uint16_t(r - 1)}) > v) {
      b.push_back({c, r});
      continue;
    }
  }
  return b;
}


void image::laplacian_fill(image const &mask) {
  uint16_t const inc= num_cols();
  uint16_t const inr= num_rows();
  uint16_t const mnc= mask.num_cols();
  uint16_t const mnr= mask.num_rows();
  if(inc != mnc || inr != mnr) {
    throw format("%ux%u for image, but %ux%u for mask", inc, inr, mnc, mnr);
  }
  auto const                   pc= mask.threshold();
  unsigned const               n = pc.size();
  std::map<unsigned, unsigned> m;
  for(unsigned i= 0; i < n; ++i) { m[lin(pc[i])]= i; }
  VectorXd b(n);
  using T= Triplet<double>;
  vector<T> coefs;
  auto      f= [&](pcoord p, unsigned i, double w) {
    auto const it= m.find(lin(p));
    if(it == m.end()) {
      b(i)+= w * pixel(p);
    } else {
      coefs.push_back({int(i), int(it->second), -w});
    }
  };
  int const    nc    = num_cols_;
  int const    nr    = num_rows();
  double const w_side= 0.50 / 3.0;
  double const w_diag= 0.25 / 3.0;
  for(unsigned i= 0; i < n; ++i) {
    coefs.push_back({int(i), int(i), 1.0});
    b(i)                = 0.0;
    pcoord const   cc   = pc[i];
    bool const     fr   = (int(cc.col) < nc - 1);
    bool const     fb   = (int(cc.row) < nr - 1);
    bool const     fl   = (cc.col > 0);
    bool const     ft   = (cc.row > 0);
    uint16_t const col_r= cc.col + 1;
    uint16_t const col_l= cc.col - 1;
    uint16_t const row_b= cc.row + 1;
    uint16_t const row_t= cc.row - 1;
    if(fr) {
      f({col_r, cc.row}, i, w_side);
      if(fb) { f({col_r, row_b}, i, w_diag); }
      if(ft) { f({col_r, row_t}, i, w_diag); }
    }
    if(fb) { f({cc.col, row_b}, i, w_side); }
    if(fl) {
      f({col_l, cc.row}, i, w_side);
      if(fb) { f({col_l, row_b}, i, w_diag); }
      if(ft) { f({col_l, row_t}, i, w_diag); }
    }
    if(ft) { f({cc.col, row_t}, i, w_side); }
  }
  SparseMatrix<double> a(n, n);
  a.setFromTriplets(coefs.begin(), coefs.end());
  SimplicialCholesky<SparseMatrix<double>> chol(a);
  VectorXd const                           x= chol.solve(b);
  for(unsigned i= 0; i < n; ++i) { pixel(pc[i])= x[i]; }
}


} // namespace regfill

// EOF
