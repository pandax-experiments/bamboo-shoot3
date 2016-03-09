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

#ifndef BS3_TYPES_VAR_UINT_HH
#define BS3_TYPES_VAR_UINT_HH

#include <type_traits>

namespace pbss {

inline
namespace var_uint_abiv1 {

// simple wrapper around unsigned integer, serializes in variable encoding
// by pbss
template <class T>
struct var_uint {

  static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value,
                "Type must be unsigned integer");

  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;

  T v;

  // conversion to embedded type
  constexpr operator T() const
  {
    return v;
  }

};

} // inline namespace var_uint_abiv1

template <class UInt>
constexpr bool operator==(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v == b.v;
}

template <class UInt>
constexpr bool operator!=(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v != b.v;
}

template <class UInt>
constexpr bool operator<(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v < b.v;
}

template <class UInt>
constexpr bool operator<=(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v <= b.v;
}

template <class UInt>
constexpr bool operator>(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v > b.v;
}

template <class UInt>
constexpr bool operator>=(const var_uint<UInt>& a, const var_uint<UInt>& b)
{
  return a.v >= b.v;
}

// and helper
template <class UInt>
constexpr var_uint<UInt> make_var_uint(const UInt& x)
{
  return var_uint<UInt>{x};
}

} // namespace pbss

#endif /* BS3_TYPES_VAR_UINT_HH */
