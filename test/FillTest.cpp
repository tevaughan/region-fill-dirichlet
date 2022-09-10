/// \file       test/FillTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::Fill.

#include "dirichlet/Fill.hpp" // Fill
#include <catch.hpp>          // TEST_CASE
#include <iostream>           // cout, endl


using dirichlet::Fill;
using Eigen::Array2i;
using Eigen::ArrayX2i;
using std::cout;
using std::endl;


uint8_t  image1[]= {0, 1, 2, 3, 4, 5, 6, 7, 8};
unsigned width1  = 3;
unsigned height1 = 3;


TEST_CASE("Constructor produces right coordinates-map.", "[Fill]") {
  ArrayX2i coords(2, 2);
  coords.row(0)= Array2i(1, 1); // pixel at         center of 3x3
  coords.row(1)= Array2i(2, 1); // pixel just below center of 3x3
  Fill const f(coords, image1, width1, height1);
  cout << "coordsMap:\n" << f.coordsMap() << endl;
  REQUIRE(f.coordsMap()(0, 0) == -1);
  REQUIRE(f.coordsMap()(1, 0) == -1);
  REQUIRE(f.coordsMap()(2, 0) == -1);
  REQUIRE(f.coordsMap()(0, 1) == -1);
  REQUIRE(f.coordsMap()(1, 1) == +0);
  REQUIRE(f.coordsMap()(2, 1) == +1);
  REQUIRE(f.coordsMap()(0, 2) == -1);
  REQUIRE(f.coordsMap()(1, 2) == -1);
  REQUIRE(f.coordsMap()(2, 0) == -1);
}


/// EOF
