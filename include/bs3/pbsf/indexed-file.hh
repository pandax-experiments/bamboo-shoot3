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

#ifndef BS3_PBSF_INDEXED_FILE_HH
#define BS3_PBSF_INDEXED_FILE_HH

// Simple single-index at end of file recording all seek positions of data
// blocks.

#include <cstdint>
#include <ios>
#include <fstream>
#include <map>
#include <memory>
#include <utility>
#include <tuple>
#include <iterator>

#include <bs3/pbss/pbss.hh>
#include <bs3/utils/optional.hh>
#include <bs3/utils/lazy-value.hh>
#include <bs3/utils/iter-util.hh>

#include "realm.hh"
#include "data-block.hh"

namespace pbsf {

namespace indexed_impl {

using pbsu::optional;
using pbsu::nullopt;

// for a well-defined data binary format, we explicitly store seek
// positions as signed 64-bit integers; but if std::streamoff is larger
// than 64 bits, fail early.
static_assert(sizeof(int64_t)>=sizeof(std::streamoff),
              "std::streamoff on this platform cannot fit in 64 bits");

inline
namespace abiv1 {

template <class Key>
struct blocks_index {

  std::map<Key, int64_t> map;

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&blocks_index::map));

};

struct index_position_marker {

  int64_t pos;
  int16_t keyid;

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&index_position_marker::pos),
    PBSS_TUPLE_MEMBER(&index_position_marker::keyid));

};

template <class Key>
PBSF_ABSTRACT_REALM(
  index_meta_realm,
  PBSF_REGISTER_TYPE(-10, index_position_marker),
  PBSF_REGISTER_TYPE(-11, blocks_index<Key>));

template <class Realm>
struct encoded_block_accessor {

  EncodedBlock block;

  template <class T>
  bool is() const
  {
    constexpr auto tid = lookup_id<T>(Realm());
    return tid == block.contentType;
  }

  template <class T>
  T as()
  {
    if (!this->is<T>())
      throw type_mismatch_error();
    return pbss::parse_from_buffer<T>(decode_block(std::move(block)));
  }

};

template <class Stream, class Realm, class T>
std::streamoff remembered_append(Stream& s, Realm r, const T& v)
{
  s.seekp(0, std::ios_base::end);
  std::streamoff pos = s.tellp();
  write_block(s, r, v);
  return pos;
}

// virtual base to be shared by reader and writer

template <class Key, class Stream>
struct indexed_file_state {
  std::unique_ptr<Stream> stream_ptr;
  blocks_index<Key> index;
};

template <class Key, class Stream, class Realm>
struct indexed_ofile : protected virtual indexed_file_state<Key, Stream> {

private:
  bool need_write_index;        // needed if file is new, or changed

public:

  indexed_ofile(bool is_new)
    : need_write_index(is_new)
  {}

  indexed_ofile(indexed_ofile&&) = default;

  ~indexed_ofile()
  {
    if (need_write_index) {
      auto pos = indexed_impl::remembered_append(
        *this->stream_ptr, index_meta_realm<Key>(), this->index);
      constexpr auto keyid = lookup_id<Key>(Realm());
      write_block(*this->stream_ptr, index_meta_realm<Key>(),
                  index_position_marker { pos, keyid });
    }
  }

  template <class Tuple>
  void insert(Tuple&& t)
  {
    auto pos = indexed_impl::remembered_append(
      *this->stream_ptr, Realm(), std::get<1>((Tuple&&)t));
    this->index.map[std::get<0>((Tuple&&)t)] = pos;
    need_write_index = true;
  }

  pbsu::variadic_insert_iterator<indexed_ofile>
  write_iterator()
  {
    return { *this };
  }

};

template <class Key, class Stream, class Realm>
struct indexed_ifile : protected virtual indexed_file_state<Key, Stream> {

private:

  template <class map_iter>
  struct iter {

  private:
    struct delayed_read_t {
      Stream* stream_ptr;
      std::streamoff pos;

      encoded_block_accessor<Realm> operator()() const
      {
        stream_ptr->seekg(pos);
        return { pbss::parse<EncodedBlock>(*stream_ptr) };
      }

    };

  public:
    typedef std::pair<Key, pbsu::lazy_value<delayed_read_t> > value_type;
    typedef value_type& reference;
    typedef value_type* pointer;
    typedef std::ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

  private:

    map_iter ipos;
    mutable value_type value;

  public:

    iter(Stream& s, map_iter i)
      : ipos(i), value { {}, delayed_read_t{&s, 0} }
    {}

    // input iterator
    reference operator*() const
    {
      return value = {
        ipos->first,
        delayed_read_t { value.second.computation.stream_ptr, ipos->second }
      };
    }

    pointer operator->() const
    {
      return std::addressof(*(*this));
    }

    iter& operator++()
    {
      ++ipos;
      return *this;
    }

    iter operator++(int)
    {
      auto copy = *this;
      ++*this;
      return copy;
    }

    bool operator!=(const iter& other) const
    {
      return ipos != other.ipos;
    }

    bool operator==(const iter& other) const
    {
      return ipos == other.ipos;
    }

    // bidirectional iterator
    iter& operator--()
    {
      --ipos;
      return *this;
    }

    iter operator--(int)
    {
      auto copy = *this;
      --*this;
      return copy;
    }

  };

public:

  using iterator = iter<typename std::map<Key, int64_t>::const_iterator>;
  using const_iterator = iter<typename std::map<Key, int64_t>::const_iterator>;
  using reverse_iterator = iter<typename std::map<Key, int64_t>::const_reverse_iterator>;
  using const_reverse_iterator = iter<typename std::map<Key, int64_t>::const_reverse_iterator>;

  iterator begin() const
  {
    return { *this->stream_ptr, this->index.map.begin() };
  }

  iterator end() const
  {
    return { *this->stream_ptr, this->index.map.end() };
  }

  iterator cbegin() const
  {
    return { *this->stream_ptr, this->index.map.begin() };
  }

  iterator cend() const
  {
    return { *this->stream_ptr, this->index.map.end() };
  }

  reverse_iterator rbegin() const
  {
    return { *this->stream_ptr, this->index.map.rbegin() };
  }

  reverse_iterator rend() const
  {
    return { *this->stream_ptr, this->index.map.rend() };
  }

  reverse_iterator crbegin() const
  {
    return { *this->stream_ptr, this->index.map.rbegin() };
  }

  reverse_iterator crend() const
  {
    return { *this->stream_ptr, this->index.map.rend() };
  }

  iterator find(const Key& k) const
  {
    return { *this->stream_ptr, this->index.map.find(k) };
  }

  iterator lower_bound(const Key& k) const
  {
    return { *this->stream_ptr, this->index.map.lower_bound(k) };
  }

  iterator upper_bound(const Key& k) const
  {
    return { *this->stream_ptr, this->index.map.upper_bound(k) };
  }

  size_t size() const
  {
    return this->index.map.size();
  }

  auto operator[](const Key& k) const
    -> decltype(this->begin()->second)
  {
    auto it = this->find(k);
    if (it == this->end())
      throw key_missing_error();
    return it->second;
  }

};

template <class Key, class Stream, class Realm>
blocks_index<Key> read_current_index(Stream& s, Realm)
{
  constexpr auto marker_size =
    decltype(fixed_size(index_position_marker(), pbss::adl_ns_tag()))::value;
  constexpr auto marker_full_size =
    sizeof(uint16_t)            // contentType
    + sizeof(uint16_t)          // contentEncoding
    + sizeof(uint32_t)          // contentChecksum
    + static_size(pbss::make_var_uint(marker_size), pbss::adl_ns_tag())
    + marker_size;
  s.seekg(-static_cast<std::streamoff>(marker_full_size), std::ios_base::end);

  using accessor = encoded_block_accessor<index_meta_realm<Key>>;

  auto marker = accessor{pbss::parse<EncodedBlock>(s)}.template as<index_position_marker>();

  constexpr auto keyid = lookup_id<Key>(Realm());
  if (marker.keyid != keyid)
    throw key_mismatch_error();

  s.seekg(marker.pos);
  return accessor{pbss::parse<EncodedBlock>(s)}.template as<blocks_index<Key>>();
}

} // inline namespace abiv1

} // namespace indexed_impl

inline
namespace abiv1 {

template <class Key, class Stream, class Realm, bool writable=false>
struct indexed_file
  : indexed_impl::indexed_ifile<Key, Stream, Realm>
{
  indexed_file(std::unique_ptr<Stream> s, indexed_impl::blocks_index<Key> index)
    : indexed_impl::indexed_file_state<Key, Stream> { std::move(s), index }
  {}
};

template <class Key, class Stream, class Realm>
struct indexed_file<Key, Stream, Realm, true>
  : indexed_impl::indexed_ifile<Key, Stream, Realm>,
    indexed_impl::indexed_ofile<Key, Stream, Realm>
{
  indexed_file(std::unique_ptr<Stream> s, indexed_impl::blocks_index<Key> index, bool is_new)
    : indexed_impl::indexed_file_state<Key, Stream> { std::move(s), index },
      indexed_impl::indexed_ofile<Key, Stream, Realm>(is_new)
  {}
};

} // inline namespace abiv1

template <class Key, class Realm>
indexed_file<Key, std::fstream, Realm, true>
open_indexed_output_file(const std::string& filename, Realm r, bool overwrite=true)
{
  using std::ios_base;
  // refer to http://en.cppreference.com/w/cpp/io/basic_filebuf/open for
  // these cryptic flags...
  auto flag = ios_base::out | ios_base::in | ios_base::ate | (
    overwrite ? ios_base::trunc : ios_base::app);
  auto s = std::make_unique<std::fstream>(filename, flag);
  s->exceptions(ios_base::failbit | ios_base::badbit);
  if (static_cast<std::streamoff>(s->tellg()) == 0) {
    write_header(*s, r);
    return { std::move(s), indexed_impl::blocks_index<Key>(), true };
  } else {
    s->seekg(0);
    if (!check_file(*s, r))
      throw unknown_realm_error();
    auto index = indexed_impl::read_current_index<Key>(*s, r);
    return { std::move(s), std::move(index), false };
  }
}

template <class Key, class Realm>
indexed_file<Key, std::ifstream, Realm>
open_indexed_input_file(const std::string& filename, Realm r)
{
  auto s = std::make_unique<std::ifstream>(filename);
  s->exceptions(std::ios_base::failbit | std::ios_base::badbit);
  if (!check_file(*s, r))
    throw unknown_realm_error();
  auto index = indexed_impl::read_current_index<Key>(*s, r);
  return { std::move(s), std::move(index) };
}

} // namespace pbsf

#endif /* BS3_PBSF_INDEXED_FILE_HH */
