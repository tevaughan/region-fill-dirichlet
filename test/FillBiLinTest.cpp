/// \file       test/FillBiLinTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::FillBiLin.

#include "dirichlet/FillBiLin.hpp"      // FillBiLin
#include "mask1.hpp"                    // mask1, etc.
#include "mask2.hpp"                    // mask2, etc.
#include "mask3.hpp"                    // mask3, etc.
#include "pgm.hpp"                      // Image, drawMask(), test::pgm
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <chrono>                       // steady_clock
#include <fstream>                      // ifstream, ofstream
#include <iostream>                     // cout, endl

using dirichlet::FillBiLin;
using std::cout;
using std::endl;


void verify(Eigen::ArrayXX<int8_t> const &a, int8_t const *b) {
  for(int r= 0; r < a.rows(); ++r) {
    for(int c= 0; c < a.cols(); ++c) {
      REQUIRE(a(r, c) == b[r * a.cols() + c]);
    }
  }
}


TEST_CASE("Minimal mask works.", "[FillBiLin]") {
  FillBiLin const f(mask1, 12, 12);
  verify(f.weights().cen(), expectedCen1);
  verify(f.weights().lft(), expectedLft1);
  verify(f.weights().rgt(), expectedRgt1);
  verify(f.weights().top(), expectedTop1);
  verify(f.weights().bot(), expectedBot1);
  cout << "mask=\n";
  for(int r= 0; r < 12; ++r) {
    for(int c= 0; c < 12; ++c) { cout << mask1[r * 12 + c] << " "; }
    cout << endl;
  }
  cout << "weights.cen=\n" << f.weights().cen() << endl;
  cout << "coordsMap=\n" << f.coordsMap() << endl;

  FillBiLin const g(mask2, 24, 24);
  verify(g.weights().cen(), expectedCen2);
  verify(g.weights().lft(), expectedLft2);
  verify(g.weights().rgt(), expectedRgt2);
  verify(g.weights().top(), expectedTop2);
  verify(g.weights().bot(), expectedBot2);
  cout << "mask=\n";
  for(int r= 0; r < 24; ++r) {
    for(int c= 0; c < 24; ++c) { cout << mask2[r * 24 + c] << " "; }
    cout << endl;
  }
  cout << "weights.cen=\n" << g.weights().cen() << endl;
  cout << "coordsMap=\n" << g.coordsMap() << endl;

  FillBiLin const h(mask3, 24, 24);
  verify(h.weights().cen(), expectedCen3);
  verify(h.weights().lft(), expectedLft3);
  verify(h.weights().rgt(), expectedRgt3);
  verify(h.weights().top(), expectedTop3);
  verify(h.weights().bot(), expectedBot3);
  cout << "mask=\n";
  for(int r= 0; r < 24; ++r) {
    for(int c= 0; c < 24; ++c) { cout << mask3[r * 24 + c] << " "; }
    cout << endl;
  }
  cout << "weights.cen=\n" << h.weights().cen() << endl;
  cout << "coordsMap=\n" << h.coordsMap() << endl;
}


#if 0
TEST_CASE("Big image.", "[FillBiLin]") {
  test::Image image= test::pgm::read("gray.pgm");
  test::Image const mask= test::drawMask(image);

  auto            start= std::chrono::steady_clock::now();
  FillBiLin const f(&mask(0, 0), image.cols(), image.rows());
  auto            way1= std::chrono::steady_clock::now();
#define SOLVE 1
#if SOLVE
  f(&image(0, 0));
  auto end= std::chrono::steady_clock::now();
#endif

  std::chrono::duration<double> t1= way1 - start;
#if SOLVE
  std::chrono::duration<double> t2= end - way1;
#endif
  cout << "\ntime to construct: " << t1.count() << " s\n" << endl;
#if SOLVE
  cout << "time to solve: " << t2.count() << " s" << endl;
#endif
  test::pgm::write("central-weight.pgm", f.weights().cen().cast<int>());
  test::pgm::write("gray-bilin.pgm", image.cast<int>());
}
#endif


/// EOF
