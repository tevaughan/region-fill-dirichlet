/// \file       include/dirichlet/impl/initLrtb.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::initLrtb().

#ifndef DIRICHLET_IMPL_INIT_LRTB_HPP
#define DIRICHLET_IMPL_INIT_LRTB_HPP

#include "../Coords.hpp" // Coords, Eigen/Dense

namespace dirichlet::impl {


using Eigen::ArrayXi;
using Eigen::ArrayXXi;
using Eigen::Map;


/// Compute initializer for value returned by Fill::lrtb().
///
/// Each row in returned matrix corresponds to different filled pixel.  Each
/// column in returned matrix corresponds to value of element in `cMap` for
/// different neighbor of pixel.
///
/// \param coords  Coordinates of each pixel to be Dirichlet-filled.
///
/// \param cMap    Map from rectangular coordinates of filled pixel to offset
///                of row in matrix returned by x().  See documentation for
///                coordsMap().
///
/// \return        Initializer for Fill::lrtb().
///
ArrayXXi initLrtb(Coords const &coords, ArrayXXi const &cMap) {
  int const          height= cMap.rows();
  int const          width = cMap.cols();
  ArrayXXi           lrtb(coords.rows(), 4);
  Map<ArrayXi const> m(&cMap(0, 0), width * height, 1);
  lrtb.col(0)= m(coords.col(0) + (coords.col(1) - 1) * height);
  lrtb.col(1)= m(coords.col(0) + (coords.col(1) + 1) * height);
  lrtb.col(2)= m(coords.col(0) - 1 + coords.col(1) * height);
  lrtb.col(3)= m(coords.col(0) + 1 + coords.col(1) * height);
  return lrtb;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_INIT_LRTB_HPP
