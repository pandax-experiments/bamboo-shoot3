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

#ifndef BS3_PBSS_PARSE_CONTAINER_FWD_HH
#define BS3_PBSS_PARSE_CONTAINER_FWD_HH

#include <utility>

namespace pbss {

namespace parse_cont_impl {

// sequential containers use .push_back
template <class T>
auto check_sequential_container() -> decltype(
  std::declval<typename T::size_type>(),
  std::declval<typename T::value_type>(),
  (std::declval<typename std::remove_const<T>::type>())
    .push_back(std::declval<typename T::const_reference>()),
  T());

template <class T, class X>
auto maybe_insert(size_t, T& c, X&& x)
  -> decltype(c.insert((X&&)x),
              void())
{
  c.insert((X&&)x);
}

template <class T, size_t N, class X>
void maybe_insert(size_t i, std::array<T, N>& c, X&& x)
{
  c[i] = (X&&)x;
}

// containers that maintain their own order use .insert
template <class T>
auto check_ignore_order_container() -> decltype(
  std::declval<typename T::size_type>(),
  std::declval<typename T::value_type>(),
  maybe_insert(size_t(), std::declval<typename std::remove_const<T>::type&>(),
               std::declval<typename T::value_type>()),
  T());

} // namespace parse_cont_impl

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  parse_cont_impl::check_sequential_container<T>());

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  parse_cont_impl::check_ignore_order_container<T>());

}

#endif /* BS3_PBSS_PARSE_CONTAINER_FWD_HH */
