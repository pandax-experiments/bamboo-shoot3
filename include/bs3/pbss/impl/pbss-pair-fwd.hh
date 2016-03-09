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

#ifndef BS3_PBSS_PAIR_FWD_HH
#define BS3_PBSS_PAIR_FWD_HH

#include <utility>
#include <type_traits>

namespace pbss {

template <class L, class R, class Stream>
void serialize(Stream& stream, const std::pair<L, R>& pair);

namespace pair_impl {
template <class T> struct is_pair : std::false_type {};
template <class L, class R> struct is_pair<std::pair<L, R>> : std::true_type {};
}

template <class T, class Stream>
typename std::enable_if<pair_impl::is_pair<typename std::remove_const<T>::type>::value, T>::type
parse(Stream& stream);

}

#endif /* BS3_PBSS_PAIR_FWD_HH */
