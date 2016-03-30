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

#ifndef BS3_PBSS_VAR_UINT_HH
#define BS3_PBSS_VAR_UINT_HH

#include <type_traits>

#include <bs3/utils/functional.hh>

#include "../var-uint.hh"

namespace pbss {

namespace vuint_impl {

template <class UInt, class Stream>
typename std::enable_if<std::is_unsigned<UInt>::value>::type
write_var_uint(Stream& stream, UInt n)
{
  do {
    stream.put(char((n&0x7f) | (n>0x7f ? 0x80 : 0)));
  } while (n>>=7);
}

template <class UInt, class Stream>
typename std::enable_if<std::is_unsigned<UInt>::value, UInt>::type
read_var_uint(Stream& stream)
{
  UInt n = 0;
  size_t offset = 0;
  std::char_traits<char>::int_type c;
  do {
    c = stream.get();
    if (BS3_UNLIKELY(c == std::char_traits<char>::eof()))
      throw early_eof_error();
    n |= (UInt(c&0x7f))<<offset;
    offset += 7;
  } while (c&0x80);
  return n;
}

template <class T>
struct is_vuint : std::false_type {};
template <class UInt>
struct is_vuint<pbss::var_uint<UInt>> : std::true_type {};

template <char... v>
struct string_constant {

  template <class Stream>
  static void write(Stream& stream)
  {
    do_write(stream, std::integral_constant<bool,
             (sizeof...(v)>sizeof(uintmax_t) || sizeof...(v)==1)>());
  }

private:

  template <class Stream>
  static void do_write(Stream& stream, std::true_type /*large or only 1*/)
  {
    using noop = int[];
    (void) noop {
      (stream.put(v), 0)...
    };
  }

  struct acc {
    constexpr uintmax_t operator()(uintmax_t low, uintmax_t high) const
    {
      return (high<<8) | low;
    }
  };

  template <class Stream>
  static void do_write(Stream& stream, std::false_type /*short enough*/)
  {
    constexpr uintmax_t pack = pbsu::static_foldr(acc(), static_cast<unsigned char>(v)...);
    stream.write(reinterpret_cast<const char*>(&pack), sizeof...(v));
  }

};

template <size_t x, char... ch>
struct static_vuint_serialization
  : std::conditional<
      (x >> 7),
      static_vuint_serialization<(x>>7), ch..., (char)((x&0x7f)|0x80)>,
      string_constant<ch..., (char)x>
    >::type
{};

} // namespace vuint_impl

template <class UInt, class Stream>
void serialize(Stream& stream, const pbss::var_uint<UInt>& n)
{
  vuint_impl::write_var_uint(stream, n.v);
}

template <class T, class Stream>
typename std::enable_if<vuint_impl::is_vuint<T>::value, T>::type
parse(Stream& stream)
{
  return T{vuint_impl::read_var_uint<typename T::value_type>(stream)};
}

template <class UInt>
constexpr std::size_t static_size(const pbss::var_uint<UInt>& n, adl_ns_tag)
{
  return n.v>>7 ? static_size(pbss::var_uint<UInt>{n.v>>7}, adl_ns_tag())+1 : 1;
}

// may be faster than recursive static_size above, but untested
template <class UInt>
std::size_t aot_size(const pbss::var_uint<UInt>& n, adl_ns_tag)
{
  auto x = n.v;
  std::size_t size = 0;
  do ++size; while (x>>=7);
  return size;
}

}

#endif /* BS3_PBSS_VAR_UINT_HH */
