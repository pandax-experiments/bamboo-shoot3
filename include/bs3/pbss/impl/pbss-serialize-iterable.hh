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

#ifndef BS3_PBSS_SERIALIZE_ITERABLE_HH
#define BS3_PBSS_SERIALIZE_ITERABLE_HH

#include "pbss-serialize-iterable-fwd.hh"

namespace pbss {

namespace homoseq_impl {

// define size() for arrays
template <class T, std::size_t N>
constexpr std::size_t size(const T(&)[N])
{
  return N;
}

// for class with member .size()
template <class T>
constexpr auto size(const T& coll) -> decltype(coll.size())
{
  return coll.size();
}

// where using impl::size is not available
template <class T>
constexpr auto get_size(const T& coll) -> decltype(size(coll)) {
  return size(coll);
}

// cover types with member typedef value_type and C arrays
template <class T>
auto value_type_of(const T&) -> typename T::value_type;

template <class T, std::size_t N>
auto value_type_of(const T(&array)[N]) -> T;

template <class T>
auto begin_pointer_of(T&& coll) -> decltype(&*(coll.begin()))
{
  return &*(coll.begin());
}

template <class T>
T* begin_pointer_of(T array[])
{
  return array;
}

// concept is_contiguous_container
template <class T, class=void>
struct is_contiguous_container : std::false_type {};

// arrays are contiguous
template <class T, std::size_t N>
struct is_contiguous_container<T[N]> : std::true_type {};

// treat those with a .data() to be contiguous
// in STL this is (by far) true
template <class T>
struct is_contiguous_container<T, decltype(std::declval<T&>().data(), void())>
  : std::true_type {};

template <class T, class Stream>
void write_elems(Stream& stream, const T& coll, std::false_type /* cannot simply copy */)
{
  for (auto&& v : coll)
    serialize(stream, v);
}

template <class T, class Stream>
void write_elems(Stream& stream, const T& coll, std::true_type /* can simply copy */)
{
  stream.write(reinterpret_cast<const char*>(begin_pointer_of(coll)),
               to_signed(sizeof(decltype(value_type_of(coll))) * size(coll)));
}

} // namespace homoseq_impl

template <class T, class Stream>
auto serialize(Stream& stream, const T& coll) -> decltype(
  homoseq_impl::check_sized_iterable<T>())
{
  using homoseq_impl::size;
  using homoseq_impl::value_type_of;
  using homoseq_impl::is_contiguous_container;
  serialize(stream, pbss::make_var_uint(size(coll)));
  homoseq_impl::write_elems(
    stream, coll,
    std::integral_constant<bool,
    is_memory_layout<decltype(value_type_of(coll))>() &&
    is_contiguous_container<T>()>());
}

// has fixed size if element has fixed size and container has compile-time
// known length
// TODO implement this case

// has aot size n*fixed_size(element) if element type has it
template <class T>
auto aot_size(const T& coll, adl_ns_tag) -> decltype(
  homoseq_impl::get_size(coll),
  fixed_size(std::declval<decltype(homoseq_impl::value_type_of(coll))>(), adl_ns_tag()),
  std::size_t())
{
  using homoseq_impl::size;
  typedef decltype(homoseq_impl::value_type_of(coll)) value_type;
  return aot_size(pbss::make_var_uint(size(coll)), adl_ns_tag()) +
    size(coll) * decltype(fixed_size(std::declval<value_type>(), adl_ns_tag()))::value;
}

// has aot size of sum of aot_size of all elements
// but this would be slow, so only enable it when fixed size is not available
// static_size does not help because it may depend on the value, but we may
// not know that at compile time
template <class T>
auto aot_size(const T& coll, adl_ns_tag) -> decltype(
  homoseq_impl::get_size(coll),
  typename std::enable_if<has_no_fixed_size<decltype(homoseq_impl::value_type_of(coll))>()>::type(),
  aot_size(std::declval<decltype(homoseq_impl::value_type_of(coll))>(), adl_ns_tag()),
  std::size_t())
{
  using homoseq_impl::size;
  std::size_t s = aot_size(pbss::make_var_uint(size(coll)), adl_ns_tag());
  for (const auto& x : coll)
    s += aot_size(x, adl_ns_tag());
  return s;
}

}

#endif /* BS3_PBSS_SERIALIZE_ITERABLE_HH */
