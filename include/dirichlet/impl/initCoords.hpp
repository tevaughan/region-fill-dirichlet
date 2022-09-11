/// \file       include/dirichlet/impl/initCoords.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::initCoords().

#ifndef DIRICHLET_IMPL_INIT_COORDS_HPP
#define DIRICHLET_IMPL_INIT_COORDS_HPP

#include "../Coords.hpp" // Coords and classes in Eigen/Dense
#include <iostream>      // cerr, endl

/// Namespace for code that solves Dirichlet-problem for zero-valued Laplacian
/// across specified pixels in image.
namespace dirichlet::impl {


using Eigen::ArrayXi;
using Eigen::ArrayXXi;
using Eigen::Map;
using std::cerr;
using std::endl;


/// Compute initializer for value returned by Fill::coordsMap().
///
/// Empty matrix is returned if any coordinates be out of bounds.  Every pixel
/// on edge of image is out of bounds.  See documentation for Coords.
///
/// \param coords  Coordinates of each pixel to be Dirichlet-filled.
/// \param width   Number of columns in image.
/// \param height  Number of rows    in image.
/// \return        Initializer for array returned by Fill::coordsMap().
///
inline ArrayXXi
initCoords(Coords const &coords, unsigned width, unsigned height) {
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
    return ArrayXXi();
  }
  ArrayXXi      cmap(ArrayXXi::Constant(height, width, -1));
  ArrayXi const lin= coords.col(0) + coords.col(1) * height;
  Map<ArrayXi>  m(&cmap(0, 0), height * width, 1);
  int const     numCoords= coords.rows();
  // Assign offset of coordinate-pair to its grid-cell in cmap.
  m(lin)= ArrayXi::LinSpaced(numCoords, 0, numCoords - 1);
  return cmap;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_INIT_COORDS_HPP

// EOF
