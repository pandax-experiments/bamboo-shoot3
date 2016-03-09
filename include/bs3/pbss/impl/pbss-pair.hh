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

#ifndef BS3_PBSS_PAIR_HH
#define BS3_PBSS_PAIR_HH

#include "pbss-pair-fwd.hh"

namespace pbss {

template <class L, class R, class Stream>
void serialize(Stream& stream, const std::pair<L, R>& pair)
{
  serialize(stream, pair.first);
  serialize(stream, pair.second);
}

template <class T, class Stream>
typename std::enable_if<pair_impl::is_pair<typename std::remove_const<T>::type>::value, T>::type
parse(Stream& stream)
{
  // because of a GCC bug before 4.9.1
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51253
  // we cannot simply brace-initialize the pair and do parsing at same time
  auto L = parse<typename T::first_type>(stream);
  auto R = parse<typename T::second_type>(stream);
  return T(std::move(L), std::move(R));
}

template <class L, class R>
auto fixed_size(const std::pair<L, R>&, adl_ns_tag) -> decltype(
  std::integral_constant<
    std::size_t,
    decltype(fixed_size(std::declval<L>(), adl_ns_tag()))::value +
    decltype(fixed_size(std::declval<R>(), adl_ns_tag()))::value>());

template <class L, class R>
constexpr auto static_size(const std::pair<L, R>& p, adl_ns_tag) -> decltype(
  static_size(p.first, adl_ns_tag()) + static_size(p.second, adl_ns_tag()))
{
  return static_size(p.first, adl_ns_tag()) + static_size(p.second, adl_ns_tag());
}

template <class L, class R>
auto aot_size(const std::pair<L, R>& p, adl_ns_tag) -> decltype(
  aot_size(p.first, adl_ns_tag()) + aot_size(p.second, adl_ns_tag()))
{
  return aot_size(p.first, adl_ns_tag()) + aot_size(p.second, adl_ns_tag());
}

}

#endif /* BS3_PBSS_PAIR_HH */
