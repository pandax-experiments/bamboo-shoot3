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

#ifndef BS3_PBSS_ENUM_HH
#define BS3_PBSS_ENUM_HH

namespace pbss {

// handle enums by underlying type
template <class T, class Stream>
typename std::enable_if<std::is_enum<T>::value>::type
serialize(Stream& stream, const T& value)
{
  typedef typename std::underlying_type<T>::type utype;
  serialize(stream, static_cast<utype>(value));
}

template <class T, class Stream>
typename std::enable_if<std::is_enum<T>::value, T>::type
parse(Stream& stream)
{
  return static_cast<T>(parse<typename std::underlying_type<T>::type>(stream));
}

template <class T>
auto fixed_size(const T&, adl_ns_tag) -> decltype(
  typename std::enable_if<std::is_enum<T>::value, std::size_t>::type(),
  std::integral_constant<std::size_t, sizeof(T)>());

}

#endif /* BS3_PBSS_ENUM_HH */
