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

#ifndef BS3_UTILS_TUPLE_UTIL_HH
#define BS3_UTILS_TUPLE_UTIL_HH

// utilities specific to std::tuple

#include <cstddef>
#include <tuple>
#include <utility>

namespace pbsu {

inline
namespace tupleutil_abiv1 {

// generate index_sequence for tuple
template <class Tuple>
std::make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>
constexpr tuple_index_sequence()
{
  return {};
}

namespace tuple_util_impl {

template <size_t... i, class Function, class Tuple>
constexpr auto spread_call(std::index_sequence<i...>, Function f, Tuple&& t)
  -> decltype(f(std::get<i>(t)...))
{
  return f(std::get<i>(t)...);
}

} // namespace tuple_util_impl

template <class Function, class Tuple>
constexpr auto spread_call(Function&& f, Tuple&& t) -> decltype(
  tuple_util_impl::spread_call(
    tuple_index_sequence<Tuple>(),
    std::forward<Function>(f), std::forward<Tuple>(t)))
{
  return tuple_util_impl::spread_call(
    tuple_index_sequence<Tuple>(),
    std::forward<Function>(f), std::forward<Tuple>(t));
}

template <size_t... i, class Function, class Tuple>
constexpr auto pick_call(Function f, Tuple&& t) -> decltype(
  f(std::get<i>(t)...))
{
  return f(std::get<i>(t)...);
}

namespace tuple_util_impl {

template <class Function, class Tuple, size_t... i>
auto map_tuple(Function f, Tuple&& t, std::index_sequence<i...>)
  -> std::tuple<decltype(f(std::get<i>(t)))...>
{
  return std::make_tuple(f(std::get<i>(t))...);
}

} // namespace tuple_util_impl

template <class Function, class Tuple>
auto map_tuple(Function&& f, Tuple&& t) -> decltype(
  tuple_util_impl::map_tuple(
    std::forward<Function>(f), std::forward<Tuple>(t),
    tuple_index_sequence<Tuple>()))
{
  return tuple_util_impl::map_tuple(
    std::forward<Function>(f), std::forward<Tuple>(t),
    tuple_index_sequence<Tuple>());
}

} // inline namespace tupleutil_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_TUPLE_UTIL_HH */
