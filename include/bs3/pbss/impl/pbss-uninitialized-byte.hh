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

#ifndef BS3_PBSS_IMPL_UNNITIALIZED_BYTE_HH
#define BS3_PBSS_IMPL_UNNITIALIZED_BYTE_HH

namespace pbss {

template <class Stream>
void serialize(Stream& stream, const uninitialized_byte& byte)
{
  serialize(stream, static_cast<char>(byte));
}

template <class T, class Stream>
typename std::enable_if<
  std::is_same<T, uninitialized_byte>::value,
  uninitialized_byte>::type
parse(Stream& stream)
{
  return parse<char>(stream);
}

std::integral_constant<size_t, 1>
fixed_size(const uninitialized_byte&, adl_ns_tag);

template <>
struct is_memory_layout<uninitialized_byte, void>
  : std::true_type
{};

} // namespace pbss

#endif /* BS3_PBSS_IMPL_UNNITIALIZED_BYTE_HH */
