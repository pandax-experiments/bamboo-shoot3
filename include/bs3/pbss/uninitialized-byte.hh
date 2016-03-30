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

#ifndef BS3_PBSS_UNINITIALIZED_BYTE_HH
#define BS3_PBSS_UNINITIALIZED_BYTE_HH

namespace pbss {

class uninitialized_byte {

  struct empty_t {};

  union {
    empty_t empty;
    char value;
  };

public:

  constexpr uninitialized_byte()
    : empty()
  {}

  constexpr uninitialized_byte(char ch)
    : value(ch)
  {}

  constexpr operator char() const
  {
    return value;
  }

};

static_assert(sizeof(uninitialized_byte)==1, "unexpected layout");

} // namespace pbss

#endif /* BS3_PBSS_UNINITIALIZED_BYTE_HH */
