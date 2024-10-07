#ifndef CSTD_STRING
#define CSTD_STRING

#include "base.cpp"

namespace cstd {
// `size` and `cstr` are to be considered immutable
struct string {
  size_t size;
  const char *cstr;

  template <size_t N>
  constexpr string(const char (&cstr)[N]) : size(N - 1), cstr(cstr) {}
  // constexpr string(const char *cstr) : size(sizeof(cstr)-1), cstr(cstr) {}
  constexpr string(size_t size, const char *cstr) : size(size), cstr(cstr) {}
  constexpr string() : size(0), cstr(0) {}

  constexpr string prefix(size_t end) const {
    return string(clamp_top(this->size, end), this->cstr);
  }

  constexpr string postfix(size_t start) const {
    return string(this->size, this->cstr + start);
  }

  constexpr string substr(size_t end) const {
    return string(clamp_top(this->size, end), this->cstr);
  }

  constexpr string substr(size_t start, size_t end) const {
    return string(clamp_top(end, this->size) - start, this->cstr + start);
  }

  constexpr string split(char ch) const {
    size_t newsize = 0;

    for (newsize = 0; newsize < this->size && this->cstr[newsize] != ch;
         ++newsize)
      ;
    return string(newsize, this->cstr);
  }

  constexpr char operator[](size_t idx) const {
    return idx > this->size ? 0 : this->cstr[idx];
  }

  constexpr bool operator==(string &other) const {
    if (this->size != other.size) {
      return false;
    }

    for (size_t i = 0; i < this->size; ++i) {
      if (this->cstr[i] != other.cstr[i]) {
        return false;
      }
    }

    return true;
  }
};
} // namespace cstd

// ?Maybe temporary?
#include <ostream>
std::ostream &operator<<(std::ostream &os, cstd::string s) {
  os << s.size << " ";
  for (size_t i = 0; i < s.size; ++i) {
    os << s.cstr[i];
  }

  return os;
}
#endif
