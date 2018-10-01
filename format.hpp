
/// \file   format.hpp
/// \author Thomas E. Vaughan
/// \brief  Definition of regfill::format().

#ifndef REGFILL_FORMAT_HPP
#define REGFILL_FORMAT_HPP

#include <cstdio> // for snprintf()
#include <string> // for std::string

#if __cplusplus < 201703L
// C++11 (and C++14) implementation requires use of unique_ptr.
#include <memory> // for std::unique_ptr
#endif

namespace regfill {

/// Take printf-style arguments, and return a formatted string.
/// \tparam Args  Type of list for arguments following format string.
/// \param  fmt   Format string for snprintf().
/// \param  args  Arguments following format string.
/// \return       Formatted string resulting from call to snprintf().
template <typename... Args>
std::string format(std::string const &fmt, Args... args) {
  // Since C++11, if second argument to snprintf() be zero, then first may be
  // null, and return value is how many bytes would be written.
  size_t const size = snprintf(nullptr, 0, fmt.c_str(), args...);
#if __cplusplus >= 201703L
  std::string s(size, '\0'); // Since C++17, s.data() is writable.
  std::snprintf(s.data(), size + 1, fmt.c_str(), args...);
  return s; // Just move pointer on return.
#else
  std::unique_ptr<char[]> buf(new char[size + 1]);
  std::snprintf(buf.get(), size + 1, fmt.c_str(), args...);
  // Relative to C++17, extra copy and delete[].
  return std::string(buf.get());
#endif
}

} // namespace regfill

#endif // ndef REGFILL_FORMAT_HPP
