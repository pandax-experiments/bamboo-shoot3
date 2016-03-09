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

#ifndef BS3_UTILS_TYPE_TRAITS_HH
#define BS3_UTILS_TYPE_TRAITS_HH

// auxiliary type traits

#include <type_traits>

namespace pbsu {

// only works if the common base is one of the template arguments.
template <class...>
struct most_derived_common_base;

template <class T>
struct most_derived_common_base<T> {
  typedef T type;
};

template <class First, class... Rest>
struct most_derived_common_base<First, Rest...> {
private:
  using rest_type = typename most_derived_common_base<Rest...>::type;
public:
  typedef typename std::conditional<
    std::is_base_of<First, rest_type>::value,
    First,
    rest_type
  >::type type;
};

// simply holds a list of types
template <class...>
struct type_sequence {};

// check implicit conversion to bool
bool implicit_bool(bool);

} // namespace pbsu

#endif /* BS3_UTILS_TYPE_TRAITS_HH */
