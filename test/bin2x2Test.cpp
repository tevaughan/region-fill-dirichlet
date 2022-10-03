/// \file       test/bin2x2Test.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for df::bin2x2().

#include "df/bin2x2.hpp"                // bin2x2()
#include "df/BinPrep.hpp"               // BinPrep
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <iostream>

using df::bin2x2;
using df::BinPrep;
using df::Extension;
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


TEST_CASE("BinPrep extends with zero.", "[bin2x2]") {
  ArrayXX<int> a(9, 9);
  int          i  = 0;
  int          b00= 0;
  int          b01= 0;
  int          b10= 0;
  int          b11= 0;
  int          b04= 0;
  int          b44= 0;
  for(int c= 0; c < 9; ++c) {
    for(int r= 0; r < 9; ++r) {
      a(r, c)= i++;
      if(r / 2 == 0 && c / 2 == 0) b00+= a(r, c);
      if(r / 2 == 0 && c / 2 == 1) b01+= a(r, c);
      if(r / 2 == 1 && c / 2 == 0) b10+= a(r, c);
      if(r / 2 == 1 && c / 2 == 1) b11+= a(r, c);
      if(r / 2 == 0 && c / 2 == 4) b04+= a(r, c);
      if(r / 2 == 4 && c / 2 == 4) b44+= a(r, c);
    }
  }
  auto const bp= BinPrep(a);
  auto const b = bin2x2(bp.extArray);
  REQUIRE(b.rows() == 5);
  REQUIRE(b.cols() == 5);
  REQUIRE(b(0, 0) == b00);
  REQUIRE(b(0, 1) == b01);
  REQUIRE(b(1, 0) == b10);
  REQUIRE(b(1, 1) == b11);
  REQUIRE(b(0, 4) == b04);
  REQUIRE(b(4, 4) == b44);
}


TEST_CASE("BinPrep extends by duplication.", "[bin2x2]") {
  ArrayXX<int> a(9, 9);
  int          i  = 0;
  int          b00= 0;
  int          b01= 0;
  int          b10= 0;
  int          b11= 0;
  int          b04= 0;
  int          b44= 0;
  for(int c= 0; c < 9; ++c) {
    for(int r= 0; r < 9; ++r) {
      a(r, c)= i++;
      if(r / 2 == 0 && c / 2 == 0) b00+= a(r, c);
      if(r / 2 == 0 && c / 2 == 1) b01+= a(r, c);
      if(r / 2 == 1 && c / 2 == 0) b10+= a(r, c);
      if(r / 2 == 1 && c / 2 == 1) b11+= a(r, c);
      if(r / 2 == 0 && c / 2 == 4) b04+= a(r, c);
      if(r / 2 == 4 && c / 2 == 4) b44+= a(r, c);
    }
  }
  auto const bp= BinPrep(a, Extension::COPIES);
  auto const b = bin2x2(bp.extArray);
  REQUIRE(b.rows() == 5);
  REQUIRE(b.cols() == 5);
  REQUIRE(b(0, 0) == b00);
  REQUIRE(b(0, 1) == b01);
  REQUIRE(b(1, 0) == b10);
  REQUIRE(b(1, 1) == b11);
  REQUIRE(b(0, 4) == b04 + a(0, 8) + a(1, 8));
  REQUIRE(b(4, 4) == b44 + 3 * a(8, 8));
}


// EOF
