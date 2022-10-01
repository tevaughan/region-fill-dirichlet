/// \file       test/bin2x2Test.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for df::bin2x2().

#include "df/bin2x2.hpp"                // bin2x2()
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <iostream>

using df::bin2x2;
using Eigen::ArrayXX;
using std::cout;
using std::endl;


TEST_CASE("bin2x2() basically works.", "[bin2x2]") {
  ArrayXX<int> a(2, 2);
  a << 1, 2, 3, 4;
  auto const b= bin2x2(a);
  REQUIRE(b.rows() == 1);
  REQUIRE(b.cols() == 1);
  REQUIRE(b(0, 0) == 10);
}


TEST_CASE("bin2x2() extends with zero.", "[bin2x2]") {
  ArrayXX<int> a(3, 3);
  a.row(0) << 1, 2, 3;
  a.row(1) << 4, 5, 6;
  a.row(2) << 7, 8, 9;
  auto const b= bin2x2(a);
  REQUIRE(b.rows() == 2);
  REQUIRE(b.cols() == 2);
  REQUIRE(b(0, 0) == 12);
  REQUIRE(b(0, 1) == 9);
  REQUIRE(b(1, 0) == 15);
  REQUIRE(b(1, 1) == 9);
}


TEST_CASE("bin2x2() extends by duplication.", "[bin2x2]") {
  ArrayXX<int> a(3, 3);
  a.row(0) << 1, 2, 3;
  a.row(1) << 4, 5, 6;
  a.row(2) << 7, 8, 9;
  auto const b= bin2x2(a, true);
  REQUIRE(b.rows() == 2);
  REQUIRE(b.cols() == 2);
  REQUIRE(b(0, 0) == 12);
  REQUIRE(b(0, 1) == 18);
  REQUIRE(b(1, 0) == 30);
  REQUIRE(b(1, 1) == 36);
}


// EOF
