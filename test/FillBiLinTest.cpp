/// \file       test/FillBiLinTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::FillBiLin.

#include "dirichlet/FillBiLin.hpp"      // FillBiLin
#include "image1.hpp"                   // image1, mask1, etc.
#include "mask2.hpp"                    // mask2, etc.
#include "mask3.hpp"                    // mask3, etc.
#include "pgm.hpp"                      // Image, drawMask(), test::pgm
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <chrono>                       // steady_clock
#include <fstream>                      // ifstream, ofstream
#include <iostream>                     // cout, endl

using dirichlet::FillBiLin;
using std::array;
using std::cout;
using std::endl;


/// Verify that Eigen::Array matches row-major C-array.
void verify(Eigen::ArrayXX<int8_t> const &a, int8_t const *b) {
  for(int r= 0; r < a.rows(); ++r) {
    for(int c= 0; c < a.cols(); ++c) {
      REQUIRE(a(r, c) == b[r * a.cols() + c]);
    }
  }
}


void print(int const *im, int w, int h) {
  for(int r= 0; r < h; ++r) {
    for(int c= 0; c < w; ++c) { printf("%4d ", im[r * w + c]); }
    cout << "\n";
  }
}


void printDiff(int const *im2, int const *im1, int w, int h) {
  for(int r= 0; r < h; ++r) {
    for(int c= 0; c < w; ++c) {
      int const i= r * w + c;
      printf("%4d ", im2[i] - im1[i]);
    }
    cout << "\n";
  }
}


TEST_CASE("mask1 works on construction.", "[FillBiLin]") {
  FillBiLin const f(mask1, 12, 12);
#if 0
  verify(f.weights().cen(), expectedCen1);
  verify(f.weights().lft(), expectedLft1);
  verify(f.weights().rgt(), expectedRgt1);
  verify(f.weights().top(), expectedTop1);
  verify(f.weights().bot(), expectedBot1);
#endif
  cout << "mask1=\n";
  for(int r= 0; r < 12; ++r) {
    for(int c= 0; c < 12; ++c) { cout << mask1[r * 12 + c] << " "; }
    cout << endl;
  }
  cout << "weights1.cen=\n" << f.weights().cen() << endl;
  cout << "coordsMap1=\n" << f.coordsMap() << endl;
  cout << "a1(0,0)=" << f.a().coeff(0, 0) << endl;
  cout << "a1(0,1)=" << f.a().coeff(0, 1) << endl;
  cout << "a1(0,2)=" << f.a().coeff(0, 2) << endl;
  cout << "a1(0,3)=" << f.a().coeff(0, 3) << endl;
  cout << "a1(0,4)=" << f.a().coeff(0, 4) << endl;
  cout << "a1=\n" << f.a() << endl;
}


TEST_CASE("image1 works with mask1.", "[FillBiLin]") {
  enum { R=12, C=12 };
  FillBiLin f(mask1, R, C);
  array<int, R * C> im1= image1;
  cout << "image1 before:\n";
  print(im1.begin(), R, C);
  auto im1Old= im1;
  f(im1.begin());
  cout << "b1=\n" << f.b().transpose() << endl;
  cout << "image1 after:\n";
  print(im1.begin(), R, C);
  cout << "image1 diff:\n";
  printDiff(im1.begin(), im1Old.begin(), R, C);
}


TEST_CASE("mask2 works on construction.", "[FillBiLin]") {
  FillBiLin const g(mask2, 24, 24);
#if 0
  verify(g.weights().cen(), expectedCen2);
  verify(g.weights().lft(), expectedLft2);
  verify(g.weights().rgt(), expectedRgt2);
  verify(g.weights().top(), expectedTop2);
  verify(g.weights().bot(), expectedBot2);
#endif
  cout << "mask=\n";
  for(int r= 0; r < 24; ++r) {
    for(int c= 0; c < 24; ++c) { cout << mask2[r * 24 + c] << " "; }
    cout << endl;
  }
  cout << "weights.cen=\n" << g.weights().cen() << endl;
  cout << "coordsMap=\n" << g.coordsMap() << endl;
}


TEST_CASE("image2 works with mask2.", "[FillBiLin]") {
  enum { R=24, C=24 };
  FillBiLin g(mask2, R, C);
  array<int,R*C> image2;
  for(int r= 0; r < R; ++r) {
    for(int c= 0; c < C; ++c) {
      image2[r*C + c]= 1000 + 100*r + c;
    }
  }
  cout << "image2 before:\n";
  print(image2.begin(), R, C);
  auto const image2Old= image2;
  g(image2.begin());
  cout << "image2 after:\n";
  print(image2.begin(), R, C);
  cout << "image2 diff:\n";
  printDiff(image2.begin(), image2Old.begin(), R, C);
}


TEST_CASE("mask3 works on construction.", "[FillBiLin]") {
  enum { R=34, C=34 };
  FillBiLin const h(mask3, R, C);
#if 0
  verify(h.weights().cen(), expectedCen3);
  verify(h.weights().lft(), expectedLft3);
  verify(h.weights().rgt(), expectedRgt3);
  verify(h.weights().top(), expectedTop3);
  verify(h.weights().bot(), expectedBot3);
#endif
  cout << "mask=\n";
  for(int r= 0; r < R; ++r) {
    for(int c= 0; c < C; ++c) { cout << mask3[r * C + c] << " "; }
    cout << endl;
  }
  cout << "weights.cen=\n" << h.weights().cen() << endl;
  cout << "coordsMap=\n" << h.coordsMap() << endl;
}


TEST_CASE("image3 works with mask3.", "[FillBiLin]") {
  enum { R=34, C=34 };
  FillBiLin h(mask3, R, C);
  array<int,R*C> image3;
  for(int r= 0; r < R; ++r) {
    for(int c= 0; c < C; ++c) {
      image3[r*C + c]= 1000 + 100*r + c;
    }
  }
  cout << "image3 before:\n";
  print(image3.begin(), R, C);
  auto const image3Old= image3;
  h(image3.begin());
  cout << "image3 after:\n";
  print(image3.begin(), R, C);
  cout << "image3 diff:\n";
  printDiff(image3.begin(), image3Old.begin(), R, C);
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
