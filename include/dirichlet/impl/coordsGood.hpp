/// \file       include/dirichlet/impl/coordsGood.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::coordsGood().

#ifndef DIRICHLET_IMPL_COORDS_GOOD_HPP
#define DIRICHLET_IMPL_COORDS_GOOD_HPP

#include "../Coords.hpp" // Coords and classes in Eigen/Dense
#include <iostream>      // cerr, endl

namespace dirichlet::impl {


using std::cerr;
using std::endl;


/// True only if coordinates all be in bounds.
///
/// Every pixel on edge of image is out of bounds.  See documentation for
/// Coords.
///
/// \param coords  Coordinates of each pixel to be Dirichlet-filled.
/// \param width   Number of columns in image.
/// \param height  Number of rows    in image.
/// \return        True only if coordinates all be in bounds.
///
inline bool coordsGood(Coords const &coords, unsigned width, unsigned height) {
  auto roob= (coords.col(0) <= 0) || (coords.col(0) >= height - 1);
  auto coob= (coords.col(1) <= 0) || (coords.col(1) >= width - 1);
  if((roob || coob).cast<int>().sum() > 0) {
    char const w[]= "Fill::coordsGood: WARNING: ";
    for(int i= 0; i < roob.size(); ++i) {
      int const r= coords(i, 0);
      int const c= coords(i, 1);
      if(roob[i]) cerr << w << "r(" << i << ")=" << r << " OOB" << endl;
      if(coob[i]) cerr << w << "c(" << i << ")=" << c << " OOB" << endl;
    }
    return false;
  }
  return true;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_COORDS_GOOD_HPP

// EOF

