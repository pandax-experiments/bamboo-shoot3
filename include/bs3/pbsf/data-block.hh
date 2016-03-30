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

#ifndef BS3_PBSF_DATA_BLOCK_HH
#define BS3_PBSF_DATA_BLOCK_HH

#include <bs3/pbss/pbss.hh>

#include <bs3/utils/iter-util.hh>

#include "crc-32.hh"
#include "defs.hh"
#include "realm.hh"

namespace pbsf {

int16_t env_preferred_encoding();

EncodedBlock encode_block(int16_t id, pbss::buffer&& raw,
                          int16_t encoding=env_preferred_encoding());

pbss::buffer decode_block(EncodedBlock&& block);

template <class T, class Realm>
void write_block(std::ostream& stream, Realm, const T& value)
{
  constexpr auto tid = lookup_id<T>(Realm());
  using pbss::serialize;
  serialize(stream, encode_block(tid, pbss::serialize_to_buffer(value)));
}

template <class Stream>
using block_read_iterator = pbss::parse_iterator<Stream, EncodedBlock>;

namespace iter_impl {

inline
namespace abiv1 {

template <int16_t id>
struct check_id {
  constexpr bool operator()(const EncodedBlock& block) const
  {
    return block.contentType == id;
  }
};

template <class T>
struct parse_from_block {
  constexpr T operator()(EncodedBlock& block) const
  {
    return pbss::parse_from_buffer<T>(decode_block(std::move(block)));
  }
};

} // inline namespace abiv1

} // namespace iter_impl

inline
namespace abiv1 {

template <class Realm, class T>
struct skipping_read_iterator
  : public pbsu::mapping_iterator<
      iter_impl::parse_from_block<T>,
      pbsu::filtering_iterator<
        iter_impl::check_id<lookup_id<T>(Realm())>, block_read_iterator<std::istream>>,
      true
    > {

private:

  using base = pbsu::mapping_iterator<
    iter_impl::parse_from_block<T>,
    pbsu::filtering_iterator<
      iter_impl::check_id<lookup_id<T>(Realm())>, block_read_iterator<std::istream>>,
    true
  >;

public:

  skipping_read_iterator()
    : base({}, { {}, {}, {} })
  {}

  skipping_read_iterator(std::istream& s)
    : base({}, { {}, s, {} })
  {}

};

template <class Realm>
struct heterogeneous_write_iterator
  : pbsu::output_iterator_mixin<heterogeneous_write_iterator<Realm> > {

private:
  std::ostream* stream_ptr;

public:

  heterogeneous_write_iterator(std::ostream& s)
    : stream_ptr(&s)
  {}

  template <class T>
  heterogeneous_write_iterator& operator=(const T& v)
  {
    write_block(*stream_ptr, Realm(), v);
    return *this;
  }

};

} // inline namespace abiv1

}

#endif /* BS3_PBSF_DATA_BLOCK_HH */
