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

#ifndef BS3_PBSS_SIZE_HH
#define BS3_PBSS_SIZE_HH

#include <type_traits>

namespace pbss {

// see ../pbss.hh
struct adl_ns_tag {};

template <class T>
constexpr auto static_size(const T&, adl_ns_tag) -> decltype(
  decltype(fixed_size(std::declval<T>(), adl_ns_tag()))::value)
{
  return decltype(fixed_size(std::declval<T>(), adl_ns_tag()))::value;
}

template <class T>
auto aot_size(const T& v, adl_ns_tag) -> decltype(static_size(v, adl_ns_tag()))
{
  return static_size(v, adl_ns_tag());
}

namespace size_impl {

template <class T>
auto check_no_fixed_size(const T& v)
  -> decltype(fixed_size(v, adl_ns_tag()), std::false_type());

template <class... T>
auto check_no_fixed_size(T...) -> std::true_type;

} // namespace size_impl

template <class T>
constexpr auto has_no_fixed_size()
  -> decltype(size_impl::check_no_fixed_size(std::declval<T>()))
{
  return {};
}

}

#endif /* BS3_PBSS_SIZE_HH */
