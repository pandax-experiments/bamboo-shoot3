/*

    Copyright 2016 Carl Lei

    This file is part of Bamboo Shoot 3.

    Bamboo Shoot 3 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Bamboo Shoot 3 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Bamboo Shoot 3.  If not, see <http://www.gnu.org/licenses/>.

    Carl Lei <xecycle@gmail.com>

*/

#ifndef BS3_PBSS_CHAR_RANGE_WRITER_HH
#define BS3_PBSS_CHAR_RANGE_WRITER_HH

#include <ios>
#include <string>

namespace pbss {

inline
namespace chrange_abiv1 {

template <class Char, class Traits=std::char_traits<Char> >
struct basic_char_range_writer {

private:

  Char* ptr;

public:

  basic_char_range_writer(Char* p)
    : ptr(p)
  {}

  basic_char_range_writer& put(Char ch)
  {
    *ptr++ = ch;
    return *this;
  }

  basic_char_range_writer& write(const Char* src, std::streamsize count)
  {
    Traits::copy(ptr, src, to_unsigned(count));
    ptr += count;
    return *this;
  }

  Char* pptr() const
  {
    return ptr;
  }

};

} // inline namespace chrange_abiv1

using char_range_writer = basic_char_range_writer<char>;

} // namespace pbss

#endif /* BS3_PBSS_CHAR_RANGE_WRITER_HH */
