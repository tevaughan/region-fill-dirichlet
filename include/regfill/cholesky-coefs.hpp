/// \file       cholesky-coefs.hpp
/// \copyright  2022 Thomas E. Vaughan
/// \brief      Definition of regfill::cholesky_coefs.

#ifndef REGFILL_CHOLESKY_COEFS_HPP
#define REGFILL_CHOLESKY_COEFS_HPP

#include "eigen3/Eigen/Dense"   // VectorXd
#include "eigen3/Eigen/Sparse"  // Triplet
#include "image.hpp"            // image
#include "neighbors.hpp"        // neighbors
#include "threshold-coords.hpp" // threshold_coords
#include <vector>               // vector

namespace regfill {


using Eigen::Triplet;
using Eigen::VectorXd;
using std::vector;


class cholesky_coefs {
  image const &           im_;
  threshold_coords        thresh_;
  VectorXd                b_;
  vector<Triplet<double>> coefs_;

  void f(coords p, unsigned i, double w) {
    auto const &map= thresh_.map();
    auto const  it = map.find(im_.size().lin(p));
    if(it == map.end()) {
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
    int const nc= mask.size().cols();
    int const nr= mask.size().rows();
    for(unsigned i= 0; i < thresh_.crd().size(); ++i) {
      b_(i)= 0.0;
      coefs_.push_back({int(i), int(i), 1.0});
      coords const    cc= thresh_.crd()[i];
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


} // namespace regfill

#endif // ndef REGFILL_CHOLESKY_COEFS_HPP
