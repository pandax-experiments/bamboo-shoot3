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

#ifndef BS3_PBSS_TUPLE_FWD_HH
#define BS3_PBSS_TUPLE_FWD_HH

#ifndef PBSS_TUPLE_MEMBER_TYPEDEF_NAME
#  define PBSS_TUPLE_MEMBER_TYPEDEF_NAME __pbss_tuple_member_tag__
#endif

#include <type_traits>
#include <utility>

namespace pbss {

template <class T, class Stream>
auto serialize(Stream& stream, const T& value) -> decltype(
  std::declval<typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME>(),
  void());

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  std::declval<typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME>(),
  T());

}

#endif /* BS3_PBSS_TUPLE_FWD_HH */
