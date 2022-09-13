/// \file       test/FillTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::Fill.

#include "dirichlet/Fill.hpp"           // Fill
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <iostream>                     // cout, endl


using dirichlet::Coords;
using dirichlet::Fill;
using Eigen::Array2i;
using std::cout;
using std::endl;


uint8_t image1[]= {0,  1,  2,  3,  //
                   4,  5,  6,  7,  //
                   8,  9,  10, 11, //
                   12, 13, 14, 15, //
                   16, 17, 18, 19};

unsigned width1 = 4;
unsigned height1= 5;


TEST_CASE("Constructor produces right coordinates-map.", "[Fill]") {
  Coords coords(3);
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
  auto const nLft= f.nLft();
  auto const nRgt= f.nRgt();
  auto const nTop= f.nTop();
  auto const nBot= f.nBot();
  REQUIRE(nLft(0) == -1);
  REQUIRE(nLft(1) == -1);
  REQUIRE(nLft(2) == -1);
  REQUIRE(nRgt(0) == -1);
  REQUIRE(nRgt(1) == -1);
  REQUIRE(nRgt(2) == -1);
  REQUIRE(nTop(0) == -1);
  REQUIRE(nTop(1) == 0);
  REQUIRE(nTop(2) == -1);
  REQUIRE(nBot(0) == 1);
  REQUIRE(nBot(1) == -1);
  REQUIRE(nBot(2) == -1);
}


TEST_CASE("Constructor checks oob hi row.", "[Fill]") {
  Coords coords(2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(4, 1); // pixel oob on bottom edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob lo row.", "[Fill]") {
  Coords coords(2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(-1, 1); // pixel oob left of left edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob hi col.", "[Fill]") {
  Coords coords(2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(1, 3); // pixel oob on right edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


TEST_CASE("Constructor checks oob lo col.", "[Fill]") {
  Coords coords(2);
  coords.row(0)= Array2i(1, 1);
  coords.row(1)= Array2i(1, -1); // pixel oob above top edge
  Fill const f(coords, width1, height1);
  REQUIRE(f.coordsMap().rows() == 0);
  REQUIRE(f.coordsMap().cols() == 0);
}


/// EOF
