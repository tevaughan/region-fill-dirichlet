/// \file       format.hpp
/// \author     Thomas E. Vaughan
/// \brief      Definition of regfill::format().
/// \copyright  2018-2022 Thomas E. Vaughan.

#pragma once

#include <cstdio> // for snprintf()
#include <string> // for string

#if __cplusplus < 201703L
// C++11 (and C++14) implementation requires use of unique_ptr.
#include <memory> // for unique_ptr
#endif

namespace regfill {


using std::snprintf;
using std::string;


/// Take printf()-style arguments, and return a formatted string.
/// \tparam Args  Type of list for arguments following format string.
/// \param  fmt   Format string for snprintf().
/// \param  args  Arguments following format string.
/// \return       Formatted string resulting from call to snprintf().
template<typename... Args> string format(string const &fmt, Args... args) {
  // Since C++11, if second argument to snprintf() be zero, then first may be
  // null, and return value is how many bytes would be written.
  size_t const size= snprintf(nullptr, 0, fmt.c_str(), args...);
#if __cplusplus >= 201703L
  // Since C++17, string::data() is writable.
  string s(size, '\0');
  snprintf(s.data(), size + 1, fmt.c_str(), args...);
  // Copy or move on return is elided as of C++-17.
  return s;
#else
  // Before C++17, string::data() is not writable.
  std::unique_ptr<char[]> buf(new char[size + 1]);
  snprintf(buf.get(), size + 1, fmt.c_str(), args...);
  // Relative to C++17, extra copy into string before return, extra delete[] on
  // return, and possible extra copy on return.
  return string(buf.get());
#endif
}


} // namespace regfill

// EOF
