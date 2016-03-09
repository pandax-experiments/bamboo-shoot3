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

#ifndef BS3_UTILS_RANGE_HH
#define BS3_UTILS_RANGE_HH

#include <iterator>
#include <utility>

#include "misc.hh"

namespace pbsu {

inline
namespace range_abiv1 {

template <class Iterator>
struct range {

  typedef Iterator iterator;
  typedef typename std::iterator_traits<Iterator>::value_type value_type;
  typedef typename std::iterator_traits<Iterator>::reference reference;
  typedef typename std::iterator_traits<Iterator>::pointer pointer;

  range(Iterator begin, Iterator end)
    : _begin(begin), _end(end)
  {}

  Iterator begin() const
  {
    return _begin;
  }

  Iterator end() const
  {
    return _end;
  }

private:
  Iterator _begin, _end;

public:

  // non-empty?
  template <class I = Iterator>
  explicit operator decltype(bool(std::declval<I>() != std::declval<I>()))() const
  {
    return _begin != _end;
  }

  template <class I = Iterator>
  decltype(bool(std::declval<I>() != std::declval<I>()))
  empty() const
  {
    return !*this;
  }

  template <class I = Iterator>
  typename std::make_unsigned<decltype(std::declval<I>() - std::declval<I>())>::type
  size() const
  {
    return to_unsigned(_end-_begin);
  }

  // typically operator[] on iterators are defined to accept
  // difference_type, but containers accept size_type.  ranges are like
  // containers.

  template <class I = Iterator>
  auto operator[](size_t i)
    -> decltype(std::declval<I&>()[i])
  {
    return _begin[to_signed(i)];
  }

  template <class I = Iterator>
  auto operator[](size_t i) const
    -> decltype(std::declval<const I&>()[i])
  {
    return _begin[to_signed(i)];
  }

};

namespace range_impl {   // using std::begin without pulling into parent ns

using std::begin;
using std::end;

template <class Collection>
auto make_range(Collection&& coll) -> range<decltype(begin(coll), end(coll))>
{
  return { begin(coll), end(coll) };
}

// and from pair of iterators
template <class Iterator>
range<Iterator> make_range(Iterator begin, Iterator end)
{
  return { begin, end };
}

} // namespace range_impl

} // inline namespace range_abiv1

using range_impl::make_range;

} // namespace pbsu

#endif /* BS3_UTILS_RANGE_HH */
