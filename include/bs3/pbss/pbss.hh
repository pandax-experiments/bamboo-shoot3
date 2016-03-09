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

#ifndef BS3_PBSS_HH
#define BS3_PBSS_HH

#include <stdexcept>
#include <sstream>
#include <utility>
#include <memory>

#include <bs3/utils/peek-for-eof.hh>
#include <bs3/utils/range.hh>
#include <bs3/utils/misc.hh>

// This header defines two functions, serialize(std::ostream&, const T&)
// and parse<T>(std::istream&).  Template arguments to serialize is not
// consistent, the user shall rely on compiler deduction by type of the
// second argument.  parse always take one template argument as the wanted
// type.

namespace pbss {

// parse() throws this for early eof
class early_eof_error : public std::runtime_error {
public:
  early_eof_error(const char* msg = "Unexpected EOF")
    : std::runtime_error(msg)
  {}
};

// commonly used
using pbsu::to_signed;
using pbsu::to_unsigned;

}

// Supported types may implement
//
// - std::integral_constant fixed_size(const type&, pbss::adl_ns_tag)
// - constexpr static_size(const type&, pbss::adl_ns_tag)
// - aot_size(const type&, pbss::adl_ns_tag)
// - static is_memory_layout<type>::value
//
// adl_ns_tag is used for adding namespace pbss to the search set of ADL,
// so that types defiend in user namespace can also be matched against
// SFINAE expressions written in namespace pbss, to find overloads defined
// later.
//
// The size functions return serialized length of an object.  fixed_size
// will be called with a std::declval<type>(), so must not rely on the
// value; static_size apparently shall be known at compile time and may
// depend on the value; aot_size is expected to be faster than performing a
// real serialization and obtaining length of the result.  fixed_size need
// only a declaration since it will not be called outside a decltype.
//
// is_memory_layout<type> is an integral constant indicating whether a
// serialized object always have the same representation in memory,
// considering padding bytes if any.  This allows an array of this type to
// be read or written as-is.

namespace pbss {
// second argument provided for SFINAE.
template <class T, class=void>
struct is_memory_layout : std::false_type {};
}

#include "impl/pbss-size.hh"

// primitives cannot refer to another type, so no fwd decl
#include "impl/pbss-primitives.hh"
// enums reference only primitives
#include "impl/pbss-enum.hh"

// variable length unsigned integers reference nothing
#include "impl/pbss-var-uint.hh"

// forward decls for possibly recursive types
// std::pair<L, R> as a heterogeneous static-length sequence {L, R}
#include "impl/pbss-std-tuple-fwd.hh"
// serialization of any sequence iterable by range-based for loop
#include "impl/pbss-serialize-iterable-fwd.hh"
// parse homogeneous dynamic-length sequence to STL containers
// the API cannot support parsing to C arrays, since they cannot be copied
#include "impl/pbss-parse-container-fwd.hh"
// custom classes as tagged sequence
#include "impl/pbss-struct-fwd.hh"
// custom classes as heterogeneous static-length sequence
#include "impl/pbss-tuple-fwd.hh"

// full implementation
#include "impl/pbss-std-tuple.hh"
#include "impl/pbss-serialize-iterable.hh"
#include "impl/pbss-parse-container.hh"
#include "impl/pbss-struct.hh"
#include "impl/pbss-tuple.hh"

#include "char-range-reader.hh"
#include "char-range-writer.hh"

namespace pbss {

template <class T>
auto serialize_to_string(const T& value)
  -> decltype(serialize(std::declval<std::ostream&>(), value), std::string())
{
  std::string str(aot_size(value, adl_ns_tag()), 0);
  char_range_writer writer(&*str.begin());
  serialize(writer, value);
  return str;
}

template <class T>
auto parse_from_string(const std::string& str)
  -> decltype(parse<T>(std::declval<std::istream&>()))
{
  char_range_reader reader(&*str.begin(), (&*str.begin()) + str.size());
  return parse<T>(reader);
}

inline
namespace iter_abiv1 {

template <class Stream, class T>
struct parse_iterator {

  typedef std::input_iterator_tag iterator_category;
  typedef T value_type;
  typedef std::ptrdiff_t difference_type;
  typedef T& reference;
  typedef T* pointer;

private:
  Stream* stream_ptr;
  // this is mutable because a const iterator (e.g. char* const) usually is
  // expected to derefence to a non-const reference
  mutable T value;

public:

  parse_iterator()
    : stream_ptr(0)
  {}

  parse_iterator(Stream& s)
    : stream_ptr(&s)
  {
    ++(*this);
  }

  parse_iterator& operator++()
  {
    auto& stream = *stream_ptr;
    if (pbsu::peek_for_eof(stream))
      stream_ptr = 0;
    else
      value = parse<T>(stream);
    return *this;
  }

  bool operator==(const parse_iterator& other) const
  {
    return stream_ptr == other.stream_ptr;
  }

  bool operator!=(const parse_iterator& other) const
  {
    return !((*this) == other);
  }

  reference operator*() const
  {
    return value;
  }

  pointer operator->() const
  {
    return std::addressof(value);
  }

};

} // inline namespace iter_abiv1

template <class T, class Stream>
pbsu::range<parse_iterator<Stream, T> >
parse_all(Stream& s)
{
  return { s, {} };
}

} // namespace pbss

#endif /* BS3_PBSS_HH */
