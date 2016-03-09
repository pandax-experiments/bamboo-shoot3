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

#ifndef BS3_PBSS_PRIMITIVES_HH
#define BS3_PBSS_PRIMITIVES_HH

// primitives := is_arithmetic

// so it excludes pointers, though they are numeric; this is intentional,
// because we do not want to allow references of any type

#include <istream>
#include <ostream>
#include <type_traits>

namespace pbss {

namespace prim_impl {

template <class T, class Stream>
void write_prim(Stream& stream, const T& value, std::true_type /*is_one_byte*/)
{
  stream.put(reinterpret_cast<const char&>(value));
}

template <class T, class Stream>
void write_prim(Stream& stream, const T& value, std::false_type /*is_not_one_byte*/)
{
  stream.write(reinterpret_cast<const char*>(&value), sizeof value);
}

template <class T, class Stream>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
read_prim(Stream& stream, std::true_type /*is_one_byte*/)
{
  auto v = stream.get();
  if (v == std::char_traits<char>::eof())
    throw early_eof_error();
  return static_cast<T>(v);
}

template <class T, class Stream>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
read_prim(Stream& stream, std::false_type /*is_not_one_byte*/)
{
  typename std::remove_const<T>::type value;
  stream.read(reinterpret_cast<char*>(&value), sizeof value);
  if (stream.eof())
    throw early_eof_error();
  return value;
}

} // namespace prim_impl

template <class T, class Stream>
typename std::enable_if<std::is_arithmetic<T>::value>::type
serialize(Stream& stream, const T& value)
{
  prim_impl::write_prim(stream, value, std::integral_constant<bool, sizeof value == 1>());
}

template <class T, class Stream>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
parse(Stream& stream)
{
  return prim_impl::read_prim<T>(stream, std::integral_constant<bool, sizeof(T)==1>());
}

template <class T>
auto fixed_size(const T&, adl_ns_tag) -> decltype(
  typename std::enable_if<std::is_arithmetic<T>::value>::type(),
  std::integral_constant<std::size_t, sizeof(T)>());

template <class T>
struct is_memory_layout<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
  : std::true_type {};

}

#endif /* BS3_PBSS_PRIMITIVES_HH */
