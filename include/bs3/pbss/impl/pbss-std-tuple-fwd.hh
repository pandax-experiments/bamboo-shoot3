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

#ifndef BS3_PBSS_STD_TUPLE_FWD_HH
#define BS3_PBSS_STD_TUPLE_FWD_HH

#include <tuple>
#include <utility>
#include <type_traits>
#include <array>

namespace pbss {

namespace stdtuple_impl {

template <class Tuple>
struct is_tuple_impl : std::false_type {};

template <class L, class R>
struct is_tuple_impl<std::pair<L, R> > : std::true_type {};

template <class... T>
struct is_tuple_impl<std::tuple<T...> > : std::true_type {};

// exclude std::array here, which will be handled as containers

template <class T>
using is_tuple = is_tuple_impl<typename std::decay<T>::type>;

} // namespace stdtuple_impl

template <class Tuple, class Stream>
typename std::enable_if<stdtuple_impl::is_tuple<Tuple>::value>::type
serialize(Stream& stream, const Tuple& pair);

template <class Tuple, class Stream>
typename std::enable_if<stdtuple_impl::is_tuple<Tuple>::value, Tuple>::type
parse(Stream& stream);

} // namespace pbss

#endif /* BS3_PBSS_STD_TUPLE_FWD_HH */
