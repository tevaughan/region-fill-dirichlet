/// \file       cholesky-coefs.hpp
/// \copyright  2022 Thomas E. Vaughan
/// \brief      Definition of regfill::cholesky_coefs.

#ifndef REGFILL_CHOLESKY_COEFS_HPP
#define REGFILL_CHOLESKY_COEFS_HPP

#include "eigen3/Eigen/Dense"  // VectorXd
#include "eigen3/Eigen/Sparse" // Triplet
#include "image.hpp"           // image
#include "neighbors.hpp"       // neighbors
#include "sparse-image.hpp"    // sparse_image
#include <vector>              // vector

namespace regfill {


using Eigen::Triplet;
using Eigen::VectorXd;
using std::vector;


class cholesky_coefs {
  image const &           im_;
  sparse_image            thresh_;
  VectorXd                b_;
  vector<Triplet<double>> coefs_;

  void f(coords p, unsigned i, double w) {
    auto const &map= thresh_.map();
    auto const  it = map.find(im_.size().lin(p));
    if(it == map.end()) {
      b_(i)-= w * im_.pixel(p);
    } else {
      coefs_.push_back({int(i), int(it->second), w});
    }
  }

public:
  /// Initialize from coordinates above threshold in mask.
  cholesky_coefs(image const &im, image const &mask):
      im_(im), thresh_(mask), b_(thresh_.pix().size()) {
    int const nc= mask.size().cols();
    int const nr= mask.size().rows();
    for(unsigned i= 0; i < thresh_.pix().size(); ++i) {
      b_(i)= 0.0;
      coefs_.push_back({int(i), int(i), 4.0});
      coords const    cc= thresh_.pix()[i].crd;
      neighbors const nb(nc, nr, cc);
      if(nb.fb) f({cc.col, nb.rb}, i, -1.0);
      if(nb.ft) f({cc.col, nb.rt}, i, -1.0);
      if(nb.fr) f({nb.cr, cc.row}, i, -1.0);
      if(nb.fl) f({nb.cl, cc.row}, i, -1.0);
    }
  }

  auto        begin() const { return coefs_.begin(); }
  auto        end() const { return coefs_.end(); }
  auto const &thresh() const { return thresh_; }
  auto const &b() const { return b_; }
};


} // namespace regfill

#endif // ndef REGFILL_CHOLESKY_COEFS_HPP
