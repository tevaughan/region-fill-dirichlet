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


TEST_CASE("interpolate() works.", "[interpolate]") {
  Array22<int> crn;
  crn << 1, 2, 4, 8;
  ArrayXX<float> cen(2, 2);
  interpolate(crn, cen);
  cout << "cen=\n" << cen << endl;
}


// EOF
