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

#ifndef BS3_PBSS_TUPLE_HH
#define BS3_PBSS_TUPLE_HH

// This is not std::tuple but custom structs with a fixed layout

#include <bs3/utils/misc.hh>

namespace pbss {

namespace tuple_impl {

// single member
template <class Struct, class Member, Member Struct::* member>
struct tuple_member_tag {};

// collect all members
template <class ...T>
struct tuple_members_tag {};

// public marking API
#define PBSS_TUPLE_MEMBERS(...)                                 \
  typedef ::pbss::tuple_impl::tuple_members_tag<__VA_ARGS__>    \
  PBSS_TUPLE_MEMBER_TYPEDEF_NAME

#define PBSS_TUPLE_MEMBER(memptr)                                       \
  ::pbss::tuple_impl::tuple_member_tag                                  \
  <decltype(::pbss::struct_tagged_impl::deduce_memptr_struct_type(memptr)), \
   decltype(::pbss::struct_tagged_impl::deduce_memptr_member_type(memptr)), \
   memptr>

template <class Struct, class Member, Member Struct::* member, class Stream>
void serialize_tuple_member(Stream& stream, const Struct& value, tuple_member_tag<Struct, Member, member>)
{
  serialize(stream, value.*member);
}

template <class T, class ...Tag, class Stream>
void serialize_tuple(Stream& stream, const T& value, tuple_members_tag<Tag...>)
{
  using noop = int[];
  (void) noop {
    (serialize_tuple_member(stream, value, Tag{}), 0)...
  };
}

template <class Struct, class Member, Member Struct::* member, class Stream>
void parse_tuple_member(Stream& stream, Struct& tuple, tuple_member_tag<Struct, Member, member>)
{
  tuple.*member = parse<Member>(stream);
}

template <class T, class ...Tag, class Stream>
void parse_tuple(Stream& stream, T& tuple, tuple_members_tag<Tag...>)
{
  using noop = int[];
  (void) noop {
    (parse_tuple_member(stream, tuple, Tag{}), 0)...
  };
}

} // namespace tuple_impl

template <class T, class Stream>
auto serialize(Stream& stream, const T& value) -> decltype(
  std::declval<typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME>(),
  void())
{
  tuple_impl::serialize_tuple(stream, value, typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME());
}

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  std::declval<typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME>(),
  T())
{
  auto tuple = typename std::remove_const<T>::type();
  tuple_impl::parse_tuple(
    stream, tuple, typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME());
  return tuple;
}

namespace tuple_impl {

using pbsu::sumall;

template <class Struct, class Member, Member Struct::* member>
constexpr auto member_fixed_size(tuple_member_tag<Struct, Member, member>)
  -> decltype(decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value)
{
  return decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value;
}

template <class... Tag>
auto compute_fixed_size(tuple_members_tag<Tag...>)
  -> std::integral_constant<std::size_t, sumall(member_fixed_size(Tag())...)>;

template <class Struct, class Member, Member Struct::* member>
auto member_aot_size(const Struct& obj, tuple_member_tag<Struct, Member, member>)
  -> decltype(aot_size(obj.*member, adl_ns_tag()))
{
  return aot_size(obj.*member, adl_ns_tag());
}

template <class Struct, class... Tag>
auto compute_aot_size(const Struct& obj, tuple_members_tag<Tag...>)
  -> decltype(sumall(member_aot_size(obj, Tag())...))
{
  return sumall(member_aot_size(obj, Tag())...);
}

} // namespace tuple_impl

template <class T>
auto fixed_size(const T&, adl_ns_tag) -> decltype(
  tuple_impl::compute_fixed_size(typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME()));

template <class T>
auto aot_size(const T& obj, adl_ns_tag) -> decltype(
  typename std::enable_if<has_no_fixed_size<T>()>::type(),
  tuple_impl::compute_aot_size(obj, typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME()))
{
  return tuple_impl::compute_aot_size(obj, typename T::PBSS_TUPLE_MEMBER_TYPEDEF_NAME());
}

}

#endif /* BS3_PBSS_TUPLE_HH */
