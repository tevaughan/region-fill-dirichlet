/// \file       format.hpp
/// \copyright  2018-2022 Thomas E. Vaughan.  See terms in LICENSE.
///
/// \brief      Definition of
///               regfill::impl::format_17_et_seq(),
///               regfill::impl::format_11-14(),
///               regfill::format().

#pragma once

#include <cstdio> // snprintf()
#include <string> // string

#if __cplusplus < 201703L
#include <memory> // unique_ptr (for C++11 and C++14)
#endif

namespace regfill {


using std::snprintf;
using std::string;


namespace impl {


/// Take printf()-style arguments, and return a formatted string.  This is the
/// low-level function used for C++17 and later.
/// \tparam Args  Type of list for arguments following format string.
/// \param  size  Number of bytes that sprintf() will write.
/// \param  fmt   Format string for snprintf().
/// \param  args  Arguments following format string.
/// \return       Formatted string resulting from call to snprintf().
template<typename... Args>
string format_17_et_seq(size_t size, string const &fmt, Args... args) {
  string s(size, '\0');
  snprintf(s.data(), size + 1, fmt.c_str(), args...);
  // Copy or move on return is elided as of C++-17.
  return s;
}


/// Take printf()-style arguments, and return a formatted string.  This is the
/// low-level function used for C++11 and C++14.
/// \tparam Args  Type of list for arguments following format string.
/// \param  size  Number of bytes that sprintf() will write.
/// \param  fmt   Format string for snprintf().
/// \param  args  Arguments following format string.
/// \return       Formatted string resulting from call to snprintf().
template<typename... Args>
string format_11_14(size_t size, string const &fmt, Args... args) {
  std::unique_ptr<char[]> buf(new char[size + 1]);
  snprintf(buf.get(), size + 1, fmt.c_str(), args...);
  // Relative to C++17, extra copy into string before return, extra delete[] on
  // return, and possibly extra copy on return.
  return string(buf.get());
}


} // namespace impl


/// Take printf()-style arguments, and return a formatted string.  Use fancy
/// optimizations if compiler configured for C++17 or later.
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
  return impl::format_17_et_seq(size, fmt, args...);
#else
  // Before C++17, string::data() is not writable.
  return impl::format_11_14(size, fmt, args...);
#endif
}


} // namespace regfill

// EOF
