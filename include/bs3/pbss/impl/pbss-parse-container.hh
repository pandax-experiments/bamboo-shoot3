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

#ifndef BS3_PBSS_PARSE_CONTAINER_HH
#define BS3_PBSS_PARSE_CONTAINER_HH

#include "pbss-parse-container-fwd.hh"

#include <type_traits>

#include "../var-uint.hh"

namespace pbss {

namespace parse_cont_impl {

// call .reserve() on a collection if supported
template <class Collection, class Size_t>
auto reserve_if_applicable(Collection& coll, Size_t size) -> decltype(coll.reserve(size))
{
  return coll.reserve(size);
}

// this overload is less specific than above, so does not cause ambiguation
template <class ...T>
void reserve_if_applicable(T...)
{}

using pbss::homoseq_impl::is_contiguous_container;

template <class T, class Stream>
T parse_elems(Stream& stream, typename T::size_type size,
              std::false_type /* cannot simply read */)
{
  typename std::remove_const<T>::type coll;
  reserve_if_applicable(coll, size);
  for (decltype(size) i=0; i!=size; ++i)
    coll.push_back(parse<typename T::value_type>(stream));
  return coll;
}

template <class Collection, class Size_t>
auto resize_if_applicable(Collection& coll, Size_t size) -> decltype(coll.resize(size))
{
  return coll.resize(size);
}

template <class... T>
void resize_if_applicable(T...)
{}

template <class T, class Stream>
T parse_elems(Stream& stream, typename T::size_type size,
              std::true_type /* can simply read */)
{
  using pbss::homoseq_impl::begin_pointer_of;
  using pbss::homoseq_impl::value_type_of;
  typename std::remove_const<T>::type coll;
  resize_if_applicable(coll, size); // std::array cannot resize;
                                    // but not checking size mismatch (FIXME)
  stream.read(reinterpret_cast<char*>(begin_pointer_of(coll)),
              to_signed(sizeof(decltype(value_type_of(coll))) * size));
  if (BS3_UNLIKELY(stream.eof()))
    throw early_eof_error();
  return coll;
}

} // namespare parse_cont_impl

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  parse_cont_impl::check_sequential_container<T>())
{
  using pbss::homoseq_impl::is_contiguous_container;
  auto size = (parse<pbss::var_uint<typename T::size_type>>(stream)).v;
  return parse_cont_impl::parse_elems<T>(
    stream, size,
    std::integral_constant<bool,
    is_contiguous_container<T>() && is_memory_layout<typename T::value_type>()>());
}

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  parse_cont_impl::check_ignore_order_container<T>())
{
  auto size = (parse<pbss::var_uint<typename T::size_type>>(stream)).v;
  typename std::remove_const<T>::type coll;
  parse_cont_impl::reserve_if_applicable(coll, size);
  for (decltype(size) i=0; i!=size; ++i)
    parse_cont_impl::maybe_insert(i, coll, parse<typename T::value_type>(stream));
  return coll;
}

}

#endif /* BS3_PBSS_PARSE_CONTAINER_HH */
