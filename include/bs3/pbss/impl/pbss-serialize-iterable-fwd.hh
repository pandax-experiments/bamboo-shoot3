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

#ifndef BS3_PBSS_SERIALIZE_ITERABLE_FWD_HH
#define BS3_PBSS_SERIALIZE_ITERABLE_FWD_HH

#include <ostream>
#include <utility>
#include <iterator>

namespace pbss {

namespace homoseq_impl {

using std::begin;
using std::end;

// define size() for arrays
template <class T, std::size_t N>
constexpr std::size_t pbss_size(const T(&array)[N]);

// for class with member .size()
template <class T>
constexpr auto pbss_size(const T& coll) -> decltype(coll.size());

// and the implementation allows ADL override

template <class T>
auto check_sized_iterable() -> decltype(
  pbss_size(std::declval<T&>()),
  begin(std::declval<T&>()) != end(std::declval<T&>()),
  ++std::declval<decltype(begin(std::declval<T&>()))&>(),
  *begin(std::declval<T&>()),
  void());

} // namespace homoseq_impl

template <class T, class Stream>
auto serialize(Stream& stream, const T& coll) -> decltype(
  homoseq_impl::check_sized_iterable<T>());

}

#endif /* BS3_PBSS_SERIALIZE_ITERABLE_FWD_HH */
