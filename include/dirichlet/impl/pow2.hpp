/// \file       include/dirichlet/impl/pow2.hpp
/// \copyright  2022 Thomas E. Vaughan.  See terms in LICENSE.
/// \brief      Definition of dirichlet::impl::pow2().

#ifndef DIRICHLET_IMPL_POW2_HPP
#define DIRICHLET_IMPL_POW2_HPP

#include <cstdint> // int32_t

namespace dirichlet::impl {


/// Greater than or equal to `i`, smallest power of two.
/// \param i  Lower bound of returned value.
/// \return   Greater than or equal to `i`, smallest power of two.
inline int32_t pow2(int32_t i) {
  int32_t r= 1;
  while(r < i) {
    if(r == 0x40000000) throw "too big";
    r<<= 1;
  }
  return r;
}


} // namespace dirichlet::impl

#endif // ndef DIRICHLET_IMPL_POW2_HPP

// EOF
