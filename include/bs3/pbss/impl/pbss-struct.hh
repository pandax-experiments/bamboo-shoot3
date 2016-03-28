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

#ifndef BS3_PBSS_STRUCT_HH
#define BS3_PBSS_STRUCT_HH

#include "pbss-struct-fwd.hh"
#include <bs3/utils/misc.hh>

namespace pbss {

namespace struct_tagged_impl {

// trait for single member
template <uint8_t id, class Struct, class Member, Member Struct::*member>
struct serializable_member_tag {};

// trait collecting all members
template <class ...T>
struct serialize_members_tag {};

// helper to deduce types
template <class Struct, class Member>
Struct deduce_memptr_struct_type(Member Struct::*);
template <class Struct, class Member>
Member deduce_memptr_member_type(Member Struct::*);

// these macros are public tagging API
#define PBSS_TAGGED_STRUCT(...)                                         \
  typedef ::pbss::struct_tagged_impl::serialize_members_tag<__VA_ARGS__> \
  PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME

#define PBSS_TAG_MEMBER(id, memptr)                                     \
  ::pbss::struct_tagged_impl::serializable_member_tag                   \
  <id,                                                                  \
   decltype(::pbss::struct_tagged_impl::deduce_memptr_struct_type(memptr)), \
   decltype(::pbss::struct_tagged_impl::deduce_memptr_member_type(memptr)), \
   memptr>

template <char h, char... t>
vuint_impl::string_constant<h, t...>
prepend_to_strc(vuint_impl::string_constant<t...>);

// write a compile-time fixed constant size if it has fixed size
template <uint8_t id, class T, class Stream>
auto write_field_header(Stream& stream, const T&) -> decltype(
  decltype(fixed_size(std::declval<T>(), adl_ns_tag()))::value,
  void())
{
  constexpr auto size = decltype(fixed_size(std::declval<T>(), adl_ns_tag()))::value;
  using serialization = vuint_impl::static_vuint_serialization<size>;
  using header = decltype(prepend_to_strc<(char)id>(serialization()));
  header::write(stream);
}

// otherwise use aot size
template <uint8_t id, class T, class Stream>
auto write_field_header(Stream& stream, const T& v) -> decltype(
  typename std::enable_if<has_no_fixed_size<T>()>::type(),
  aot_size(v, adl_ns_tag()),
  void())
{
  serialize(stream, id);
  serialize(stream, pbss::make_var_uint(aot_size(v, adl_ns_tag())));
}

template <uint8_t id, class Struct, class Member, Member Struct::* member, class Stream>
auto serialize_custom_struct_member(Stream& stream, const Struct& obj,
                                    serializable_member_tag<id, Struct, Member, member>)
  -> decltype(aot_size(obj.*member, adl_ns_tag()),
              void())
{
  write_field_header<id>(stream, obj.*member);
  serialize(stream, obj.*member);
}

template <class Struct, class ...Tag, class Stream>
void serialize_custom_struct(Stream& stream, const Struct& obj, serialize_members_tag<Tag...>)
{
  using noop = int[];
  (void)noop {
    (serialize_custom_struct_member(stream, obj, Tag{}), 0)...
  };
  serialize(stream, (uint8_t)0);
}

template <int...>
struct int_sequence {};

template <size_t n, int... v>
struct repeated_sequence
  : std::conditional<
      n,
      repeated_sequence<n-1, 0, v...>,
      int_sequence<v...>
    >::type
{};

template <int... v, class Stream>
void skip_chars(Stream& stream, int_sequence<v...>)
{
  using noop = int[];
  (void) noop {
    (stream.get(), v)...
  };
}

// skipping over a varuint; when a known member has fixed size we can
// blindly ignore that many chars
template <class Member, class Stream>
auto skip_varuint(Stream& stream) -> decltype(
  decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value,
  void())
{
  constexpr auto size = decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value;
  constexpr auto varuint_size = static_size(pbss::make_var_uint(size), adl_ns_tag());
  skip_chars(stream, repeated_sequence<varuint_size>());
}

// otherwise read it
template <class Member, class Stream>
auto skip_varuint(Stream& stream) -> decltype(
  typename std::enable_if<has_no_fixed_size<Member>()>::type(),
  void())
{
  while (true) {
    auto c = stream.get();
    if (c == std::char_traits<char>::eof())
      throw early_eof_error();
    if (!(c&0x80)) break;
  }
}

template <class Struct, class Stream>
void parse_custom_struct_member(Stream& stream, uint8_t, Struct&, serialize_members_tag<>)
{
  // end case of unrecognized type id
  // read in length and skip that many chars
  auto length = parse<pbss::var_uint<size_t>>(stream).v;
  stream.ignore(to_signed(length));
  // FIXME print a warning
}

template <class Struct, class Member, uint8_t type_id, Member Struct::* member, class ...Tag, class Stream>
void parse_custom_struct_member(
  Stream& stream, uint8_t id, Struct& obj,
  serialize_members_tag<serializable_member_tag<type_id, Struct, Member, member>, Tag...>)
{
  if (id == type_id) {
    // skip var int of size
    skip_varuint<Member>(stream);
    // then parse the member
    obj.*member = parse<Member>(stream);
  }
  else parse_custom_struct_member(stream, id, obj, serialize_members_tag<Tag...>{});
}

template <class Struct, class ...Tag, class Stream>
void parse_custom_struct(Stream& stream, Struct& obj, serialize_members_tag<Tag...> tag)
{
  while (auto id = parse<uint8_t>(stream))
    parse_custom_struct_member(stream, id, obj, tag);
}

using pbsu::sumall;

template <uint8_t id, class Struct, class Member, Member Struct::* member>
constexpr auto member_fixed_size(serializable_member_tag<id, Struct, Member, member>)
  -> decltype(
    decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value)
{
  return
    1                           // tag
    + static_size(                                                      // size
        pbss::make_var_uint(decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value),
        adl_ns_tag())
    + decltype(fixed_size(std::declval<Member>(), adl_ns_tag()))::value; // itself
}

template <class ...Tag>
auto compute_fixed_size(serialize_members_tag<Tag...>)
  -> std::integral_constant<std::size_t, sumall(member_fixed_size(Tag())...)+1>;
// +1 is trailing zero

template <class Struct, uint8_t id, class Member, Member Struct::* member>
auto member_aot_size(const Struct& obj, serializable_member_tag<id, Struct, Member, member>)
  -> decltype(aot_size(obj.*member, adl_ns_tag()))
{
  auto member_size = aot_size(obj.*member, adl_ns_tag());
  return 1 + aot_size(pbss::make_var_uint(member_size), adl_ns_tag()) + member_size;
}

template <class T, class ...Tag>
auto compute_aot_size(const T& obj, serialize_members_tag<Tag...>) -> decltype(
  sumall(member_aot_size(obj, Tag())...))
{
  return sumall(member_aot_size(obj, Tag())...) + 1; // +1 is trailing zero
}

} // namespace struct_tagged_impl

template <class T, class Stream>
auto serialize(Stream& stream, const T& value) -> decltype(
  std::declval<typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME>(),
  void())
{
  return struct_tagged_impl::serialize_custom_struct(
    stream, value, typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME());
}

template <class T, class Stream>
auto parse(Stream& stream) -> decltype(
  std::declval<typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME>(),
  T())
{
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=36750
  // so I am not writing type obj{};
  auto obj = typename std::remove_const<T>::type();
  struct_tagged_impl::parse_custom_struct(
    stream, obj, typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME());
  return obj;
}

template <class T>
auto fixed_size(const T&, adl_ns_tag) -> decltype(
  struct_tagged_impl::compute_fixed_size(typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME()));

template <class T>
auto aot_size(const T& obj, adl_ns_tag) -> decltype(
  // a cheat: try aot if we cannot work out a fixed size for this struct
  typename std::enable_if<has_no_fixed_size<T>()>::type(),
  struct_tagged_impl::compute_aot_size(obj, typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME()))
{
  return struct_tagged_impl::compute_aot_size(
    obj, typename T::PBSS_TAGGED_OBJECT_MEMBER_TYPEDEF_NAME());
}

}

#endif /* BS3_PBSS_STRUCT_HH */
