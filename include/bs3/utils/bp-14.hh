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

#ifndef BS3_UTILS_BP_14_HH
#define BS3_UTILS_BP_14_HH

// backport some STL utilities from C++14 into C++11

#include <memory>

namespace pbsu {

// and this namespace is different
namespace std_bp {

/// Class template integer_sequence
template<class Int, Int... n>
struct integer_sequence
{
  typedef Int value_type;
  static constexpr size_t size() { return sizeof...(n); }
};

template <class Int, Int N, Int... i>
struct _make_integer_sequence
  : std::conditional<
      (N>0),
      _make_integer_sequence<Int, N-1, N-1, i...>,
      std::enable_if<true, integer_sequence<Int, i...>>
    >::type
{};

/// Alias template make_integer_sequence
template<class Int, Int N>
using make_integer_sequence
= typename _make_integer_sequence<Int, N>::type;

/// Alias template index_sequence
template<size_t... n>
using index_sequence = integer_sequence<size_t, n...>;

/// Alias template make_index_sequence
template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

/// Alias template index_sequence_for
template<class... _Types>
using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

// not checking for arrays
template<class T, class... Arg>
std::unique_ptr<T>
make_unique(Arg&&... arg)
{
  return std::unique_ptr<T>(new T(std::forward<Arg>(arg)...));
}

}

}

#endif /* BS3_UTILS_BP_14_HH */
