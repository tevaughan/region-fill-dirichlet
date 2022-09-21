/// \file       test/FillBiLinTest.cpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Tests for dirichlet::FillBiLin.

#include "dirichlet/FillBiLin.hpp"      // FillBiLin
#include <catch2/catch_test_macros.hpp> // TEST_CASE
#include <chrono>                       // steady_clock
#include <fstream>                      // ifstream, ofstream
#include <iostream>                     // cout, endl

using dirichlet::FillBiLin;
using std::cout;
using std::endl;


uint8_t image1[]= {0,  1,  2,  3,  //
                   4,  5,  6,  7,  //
                   8,  9,  10, 11, //
                   12, 13, 14, 15, //
                   16, 17, 18, 19};

unsigned width1 = 4;
unsigned height1= 5;


TEST_CASE("Constructor from mask runs.", "[FillBiLin]") {
  cout << "starting constructor-from-mask test" << endl;
  uint8_t  mask1[]= {0, 0, 0, 0, //
                     0, 1, 0, 0, //
                     0, 1, 0, 0, //
                     0, 0, 1, 0, //
                     0, 0, 0, 0};
  FillBiLin const f(mask1, width1, height1);
  cout << "done with constructor-from-mask test" << endl;
}


/// EOF
