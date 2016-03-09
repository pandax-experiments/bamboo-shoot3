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

#ifndef BS3_PBSS_STD_TUPLE_HH
#define BS3_PBSS_STD_TUPLE_HH

#include <bs3/utils/bp-14.hh>

#include "pbss-std-tuple-fwd.hh"

namespace pbss {

namespace stdtuple_impl {

template <size_t i, class Tuple, class Stream>
typename std::enable_if<(i >= std::tuple_size<Tuple>::value)>::type
write_tuple(Stream&, const Tuple&)
{}

template <size_t i, class Tuple, class Stream>
typename std::enable_if<(i < std::tuple_size<Tuple>::value)>::type
write_tuple(Stream& stream, const Tuple& t)
{
  serialize(stream, std::get<i>(t));
  write_tuple<(i+1)>(stream, t);
}

} // namespace stdtuple_impl

template <class Tuple, class Stream>
typename std::enable_if<stdtuple_impl::is_tuple<Tuple>::value>::type
serialize(Stream& stream, const Tuple& t)
{
  stdtuple_impl::write_tuple<0>(stream, t);
}

namespace stdtuple_impl {

template <class Tuple, class Stream, class... T>
typename std::enable_if<(sizeof...(T) >= std::tuple_size<Tuple>::value), Tuple>::type
read_tuple_continue(Stream&, T&&... values)
{
  return Tuple { ((T&&)values)... };
}

template <class Tuple, class Stream, class... T>
typename std::enable_if<(sizeof...(T) < std::tuple_size<Tuple>::value), Tuple>::type
read_tuple_continue(Stream& stream, T&&... values)
{
  using current_type = typename std::tuple_element<(sizeof...(values)), Tuple>::type;
  return read_tuple_continue<Tuple>(
    stream, ((T&&)values)...,
    parse<current_type>(stream));
}

} // namespace stdtuple_impl

template <class Tuple, class Stream>
typename std::enable_if<stdtuple_impl::is_tuple<Tuple>::value, Tuple>::type
parse(Stream& stream)
{
  return stdtuple_impl::read_tuple_continue<Tuple>(stream);
}

namespace stdtuple_impl {

template <class Tuple, size_t i>
using tuple_member_fixed_size = decltype(
  fixed_size(
    std::declval<typename std::tuple_element<i, Tuple>::type>(),
    adl_ns_tag()));

template <class Tuple, size_t... i>
constexpr auto compute_fixed_size(pbsu::std_bp::index_sequence<i...>)
  -> decltype(pbsu::sumall(tuple_member_fixed_size<Tuple, i>::value...))
{
  return pbsu::sumall(tuple_member_fixed_size<Tuple, i>::value...);
}

template <class Tuple, size_t... i>
constexpr auto compute_static_size(const Tuple& t, pbsu::std_bp::index_sequence<i...>)
  -> decltype(pbsu::sumall(static_size(std::get<i>(t), adl_ns_tag())...))
{
  return pbsu::sumall(static_size(std::get<i>(t), adl_ns_tag())...);
}

template <class Tuple, size_t... i>
auto compute_aot_size(const Tuple& t, pbsu::std_bp::index_sequence<i...>)
  -> decltype(pbsu::sumall(aot_size(std::get<i>(t), adl_ns_tag())...))
{
  return pbsu::sumall(aot_size(std::get<i>(t), adl_ns_tag())...);
}

} // namespace stdtuple_impl

template <class Tuple>
typename std::enable_if<
  stdtuple_impl::is_tuple<Tuple>::value,
  std::integral_constant<
    std::size_t,
    stdtuple_impl::compute_fixed_size<Tuple>(
      pbsu::std_bp::make_index_sequence<std::tuple_size<Tuple>::value>())>
>::type
fixed_size(const Tuple&, adl_ns_tag);

template <class Tuple>
constexpr auto static_size(const Tuple& t, adl_ns_tag) ->
  typename std::enable_if<
    (stdtuple_impl::is_tuple<Tuple>::value
     && bool(has_no_fixed_size<Tuple>())),
    decltype(stdtuple_impl::compute_static_size(
               t, pbsu::std_bp::make_index_sequence<std::tuple_size<Tuple>::value>()))
>::type
{
  return stdtuple_impl::compute_static_size(
    t, pbsu::std_bp::make_index_sequence<std::tuple_size<Tuple>::value>());
}

template <class Tuple>
constexpr auto aot_size(const Tuple& t, adl_ns_tag) ->
  typename std::enable_if<
    (stdtuple_impl::is_tuple<Tuple>::value
     && bool(has_no_static_size<Tuple>())),
    decltype(stdtuple_impl::compute_aot_size(
               t, pbsu::std_bp::make_index_sequence<std::tuple_size<Tuple>::value>()))
>::type
{
  return stdtuple_impl::compute_aot_size(
    t, pbsu::std_bp::make_index_sequence<std::tuple_size<Tuple>::value>());
}

} // namespace pbss

#endif /* BS3_PBSS_STD_TUPLE_HH */
