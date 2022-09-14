/// \file       test/FillTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::Fill.

#include "dirichlet/Fill.hpp"           // Fill
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <iostream>                     // cout, endl


using dirichlet::Fill;
using Eigen::Array2i;
using Eigen::ArrayX2i;
using std::cout;
using std::endl;


uint8_t image1[]= {0,  1,  2,  3,  //
                   4,  5,  6,  7,  //
                   8,  9,  10, 11, //
                   12, 13, 14, 15, //
                   16, 17, 18, 19};

unsigned width1 = 4;
unsigned height1= 5;


uint8_t image2[]= {0,  1,  2,  3,  4,  5,  //
                   6,  7,  8,  9,  10, 11, //
                   12, 13, 14, 15, 16, 17, //
                   18, 19, 20, 21, 22, 23, //
                   24, 25, 26, 27, 28, 29, //
                   30, 31, 32, 33, 34, 35, //
                   36, 37, 38, 39, 40, 41};

unsigned width2 = 6;
unsigned height2= 7;


TEST_CASE("Constructor produces right coordinates-map.", "[Fill]") {
  cout << "starting constructor-test" << endl;
  ArrayX2i coords(3, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(2, 1);
  coords.row(2)= Array2i(3, 2);
  Fill const f(coords, width1, height1);
  cout << "coordsMap:\n" << f.coordsMap() << endl;
  REQUIRE(f.coordsMap()(0, 0) == -1);
  REQUIRE(f.coordsMap()(1, 0) == -1);
  REQUIRE(f.coordsMap()(2, 0) == -1);
  REQUIRE(f.coordsMap()(3, 0) == -1);
  REQUIRE(f.coordsMap()(4, 0) == -1);
  REQUIRE(f.coordsMap()(0, 1) == -1);
  REQUIRE(f.coordsMap()(1, 1) == +0);
  REQUIRE(f.coordsMap()(2, 1) == +1);
  REQUIRE(f.coordsMap()(3, 1) == -1);
  REQUIRE(f.coordsMap()(4, 1) == -1);
  REQUIRE(f.coordsMap()(0, 2) == -1);
  REQUIRE(f.coordsMap()(1, 2) == -1);
  REQUIRE(f.coordsMap()(2, 2) == -1);
  REQUIRE(f.coordsMap()(3, 2) == +2);
  REQUIRE(f.coordsMap()(4, 2) == -1);
  REQUIRE(f.coordsMap()(0, 3) == -1);
  REQUIRE(f.coordsMap()(1, 3) == -1);
  REQUIRE(f.coordsMap()(2, 3) == -1);
  REQUIRE(f.coordsMap()(3, 3) == -1);
  REQUIRE(f.coordsMap()(4, 3) == -1);
  auto const nLft= f.lrtb().col(0);
  auto const nRgt= f.lrtb().col(1);
  auto const nTop= f.lrtb().col(2);
  auto const nBot= f.lrtb().col(3);
  REQUIRE(nLft(0) == -4 - 1);
  REQUIRE(nLft(1) == -8 - 1);
  REQUIRE(nLft(2) == -13 - 1);
  REQUIRE(nRgt(0) == -6 - 1);
  REQUIRE(nRgt(1) == -10 - 1);
  REQUIRE(nRgt(2) == -15 - 1);
  REQUIRE(nTop(0) == -1 - 1);
  REQUIRE(nTop(1) == 0);
  REQUIRE(nTop(2) == -10 - 1);
  REQUIRE(nBot(0) == 1);
  REQUIRE(nBot(1) == -13 - 1);
  REQUIRE(nBot(2) == -18 - 1);
  cout << "done with constructor-test" << endl;
}


TEST_CASE("Constructor from mask produces right coordinates-map.", "[Fill]") {
  cout << "starting constructor-from-mask test" << endl;
  uint8_t  mask1[]= {0, 0, 0, 0, //
                     0, 1, 0, 0, //
                     0, 1, 0, 0, //
                     0, 0, 1, 0, //
                     0, 0, 0, 0};
  ArrayX2i coords(3, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(2, 1);
  coords.row(2)= Array2i(3, 2);
  Fill const f(mask1, width1, height1);
  cout << "f.coords():\n" << f.coords() << endl;
  REQUIRE(f.coords().rows() == 3);
  REQUIRE(f.coords()(0) == coords(0));
  REQUIRE(f.coords()(1) == coords(1));
  REQUIRE(f.coords()(2) == coords(2));
  cout << "done with constructor-from-mask test" << endl;
}


TEST_CASE("Function works as expected.", "[Fill]") {
  cout << "starting function-test" << endl;
  ArrayX2i coords(20, 2);
  int      image[42];
  int      i= 0;
  cout << "image (before):" << endl;
  for(unsigned r= 0; r < height2; ++r) {
    for(unsigned c= 0; c < width2; ++c) {
      int const v          = rand() % 10;
      image[r * width2 + c]= v;
      cout << v << " ";
    }
    cout << endl;
  }
  for(unsigned r= 1; r < height2 - 1; ++r) {
    for(unsigned c= 1; c < width2 - 1; ++c) {
      cout << "inserting coords: " << i << ": " << r << " " << c << endl;
      coords.row(i++)= Array2i(r, c);
    }
  }
  Fill const f(coords, width2, height2);
  cout << "coordsMap:\n" << f.coordsMap() << endl;
  auto const x= f(image);
  cout << "image (after):" << endl;
  for(unsigned r= 0; r < height2; ++r) {
    for(unsigned c= 0; c < width2; ++c) cout << image[r * width2 + c] << " ";
    cout << endl;
  }
  cout << "done with function-test" << endl;
  // FIXME: Call REQUIRE() on something.
}


TEST_CASE("Constructor checks oob hi row.", "[Fill]") {
  ArrayX2i coords(2, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(4, 1); // pixel oob on bottom edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob lo row.", "[Fill]") {
  ArrayX2i coords(2, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(-1, 1); // pixel oob left of left edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob hi col.", "[Fill]") {
  ArrayX2i coords(2, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(1, 3); // pixel oob on right edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob lo col.", "[Fill]") {
  ArrayX2i coords(2, 2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(1, -1); // pixel oob above top edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


/// EOF
