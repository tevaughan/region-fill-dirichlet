/// \file       test/interpolateTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for df::interpolate().

#include "df/interpolate.hpp"           // interpolate()
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <iostream>

using df::interpolate;
using Eigen::Array22;
using Eigen::ArrayXX;
using std::cout;
using std::endl;


TEST_CASE("interpolate() basically works.", "[interpolate]") {
  Array22<float> crn;
  crn << -0.5f, +1.5f, +3.5f, +5.5f;
  ArrayXX<float> cen(2, 2);
  interpolate(crn, cen);
  ArrayXX<float> truth(2, 2);
  truth << 1.0f, 2.0f, 3.0f, 4.0f;
  REQUIRE((cen == truth).cast<int>().sum() == 4);
}


TEST_CASE("interpolate() rounds properly.", "[interpolate]") {
  Array22<float> crn;
  crn << -4.5f, -2.5f, +1.5f, +3.5f;
  ArrayXX<int> cen(2, 2);
  interpolate(crn, cen);
  ArrayXX<int> truth(2, 2);
  truth << -3, -2, 1, 2;
  REQUIRE((cen == truth).cast<int>().sum() == 4);
}


// EOF
