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

#ifndef BS3_UTILS_RANGE_UTIL_HH
#define BS3_UTILS_RANGE_UTIL_HH

// utilities on ranges and ranges with iterators

#include <algorithm>
#include <tuple>
#include <memory>
#include <numeric>
#include <stdexcept>

#include "type-traits.hh"
#include "range.hh"
#include "iter-util.hh"

namespace pbsu {

inline
namespace range_abiv1 {

namespace range_util_impl {

using std::begin;
using std::end;

template <class Range, class OutputIterator>
auto copy(Range&& range, OutputIterator&& it)
  -> decltype(std::copy(begin(range), end(range), std::forward<OutputIterator>(it)))
{
  return std::copy(begin(range), end(range), std::forward<OutputIterator>(it));
}

} // namespace range_util_impl

using range_util_impl::copy;

namespace range_util_impl {

// will be used in many non-member operators, declaring in this ns
using noop = int[];

// well, if any member compares equal.
// short-circuits.
template <class Tuple, size_t cmp_index = 0>
typename std::enable_if<std::tuple_size<Tuple>::value == cmp_index, bool>::type
constexpr tuple_any_equal(const Tuple&, const Tuple&)
{
  return false;
}

template <class Tuple, size_t cmp_index = 0>
typename std::enable_if<std::tuple_size<Tuple>::value != cmp_index, bool>::type
constexpr tuple_any_equal(const Tuple& a, const Tuple& b)
{
  return std::get<cmp_index>(a) == std::get<cmp_index>(b)
    || tuple_any_equal<Tuple, cmp_index+1>(a, b);
}

template <class Seq, class... Iterator>
struct zip_iterator_impl;

template <size_t... i, class... Iterator>
struct zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...> {

  typedef typename most_derived_common_base<
    typename std::iterator_traits<Iterator>::iterator_category...
  >::type
  iterator_category;
  typedef std::tuple<typename std::iterator_traits<Iterator>::value_type...> value_type;
  typedef std::tuple<typename std::iterator_traits<Iterator>::reference...> reference;
  typedef std::unique_ptr<reference> pointer;
  typedef std::ptrdiff_t difference_type;

  typedef std::tuple<Iterator...> iterator_pack;

  iterator_pack begins;
  iterator_pack ends;

  zip_iterator_impl(iterator_pack b, iterator_pack e)
    : begins(b), ends(e)
  {
    _check_end();
  }

  // when any one reaches end, put all to end
  void _check_end()
  {
    if (tuple_any_equal(begins, ends))
      begins = ends;
  }

  // all iterators must have *i
  reference operator*() const
  {
    return std::forward_as_tuple(*std::get<i>(begins)...);
  }

  // and ++i
  zip_iterator_impl& operator++()
  {
    (void) noop { (++std::get<i>(begins), 0)... };
    _check_end();
    return *this;
  }

  // wrap newly-created reference tuple in unique_ptr for operator->
  pointer operator->()
  {
    return pointer(new reference(*(*this)));
  }

  // conveniently me++
  zip_iterator_impl operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

  // other operators, subject to sfinae, are non-members

  // but operator[] cannot be member
  //
  // having Int as template parameter successfully defers check of i+n,
  // no need to include my own type in template parameter
  template <class Int>
  auto operator[](Int n) -> decltype(*(*this+n))
  {
    return *(*this+n);
  }

};

template <class... Iterator>
using zip_iterator = zip_iterator_impl<
  std_bp::make_index_sequence<sizeof...(Iterator)>,
  Iterator...
>;

// delegate to tuple comparison which already has sfinae

template <size_t... i, class... Iterator>
auto operator==(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
                const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins == b.begins)
{
  return a.begins == b.begins;
}

template <size_t... i, class... Iterator>
auto operator!=(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
                const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins != b.begins)
{
  return a.begins != b.begins;
}

template <size_t... i, class... Iterator>
auto operator<(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
               const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins < b.begins)
{
  return a.begins < b.begins;
}

template <size_t... i, class... Iterator>
auto operator<=(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
                const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins <= b.begins)
{
  return a.begins <= b.begins;
}

template <size_t... i, class... Iterator>
auto operator>(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
               const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins > b.begins)
{
  return a.begins > b.begins;
}

template <size_t... i, class... Iterator>
auto operator>=(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a,
                const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b)
  -> decltype(a.begins >= b.begins)
{
  return a.begins >= b.begins;
}

// decrement on bidirectionals
template <size_t... i, class... Iterator>
auto operator--(zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& me)
  -> decltype(noop { (--std::get<i>(me.begins), 0)... },
              std::declval<zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>&>())
{
  (void) noop { (--std::get<i>(me.begins), 0)... };
  return me;
}

template <size_t... i, class... Iterator>
auto operator--(zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& me, int)
  -> typename std::remove_reference<decltype(--me)>::type
{
  auto copy = me;
  --me;
  return copy;
}

// i+=n, i-=n, i+n, i-n, b-a on random access iterators
template <class Int, size_t... i, class... Iterator>
auto operator+=(zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& me,
                Int n)
  -> decltype(noop { (std::get<i>(me.begins)+=n, 0)... }, me)
{
  (void) noop { (std::get<i>(me.begins)+=n, 0)... };
  me._check_end();
  return me;
}

template <class Int, size_t... i, class... Iterator>
auto operator-=(zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& me,
                Int n)
  -> decltype(noop { (std::get<i>(me.begins)-=n, 0)... }, me)
{
  (void) noop { (std::get<i>(me.begins)-=n, 0)... };
  return me;
}

template <class Int, size_t... i, class... Iterator>
auto operator+(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& iter,
               Int n)
  -> typename std::remove_reference<decltype(std::declval<typename std::decay<decltype(iter)>::type&>() += n)>::type
{
  auto copy = iter;
  copy += n;
  return copy;
}

template <class Int, size_t... i, class... Iterator>
auto operator+(Int n,
               const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& iter)
  -> decltype(iter+n)
{
  return iter+n;
}

template <class Int, size_t... i, class... Iterator>
auto operator-(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& iter,
               Int n)
  -> typename std::remove_reference<decltype(std::declval<typename std::decay<decltype(iter)>::type&>() -= n)>::type
{
  auto copy = iter;
  copy -= n;
  return copy;
}

template <size_t... i, class... Iterator>
auto operator-(const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& b,
               const zip_iterator_impl<std_bp::index_sequence<i...>, Iterator...>& a)
  -> decltype(noop { (std::get<i>(b.begins)-std::get<i>(a.begins), 0)... },
              std::get<0>(b.begins) - std::get<0>(a.begins))
{
  return std::get<0>(b.begins) - std::get<0>(a.begins);
}

template <class... Range>
auto zip(Range&&... ranges) -> range<zip_iterator<decltype(begin(ranges), end(ranges))...>>
{
  using pack = std::tuple<decltype(begin(ranges))...>;
  return {
    { pack{begin(ranges)...}, pack{end(ranges)...} }, // begins
    { pack{end(ranges)...}, pack{end(ranges)...} }    // ends
  };
}

} // namespace range_util_impl

template <class... Range>
auto zip(Range&&... ranges) -> decltype(range_util_impl::zip(std::forward<Range>(ranges)...))
{
  return range_util_impl::zip(std::forward<Range>(ranges)...);
}

namespace range_util_impl {
// have using std::begin here

template <class Function, class Range>
auto map(Function f, Range&& r)
  -> decltype(make_range(make_mapping_iterator(f, begin(r)),
                         make_mapping_iterator(f, end(r))))
{
  return {
    make_mapping_iterator(f, begin(r)),
    make_mapping_iterator(f, end(r))
  };
}

template <class Function, class Range>
auto map_skipping(Function f, Range&& r)
  -> decltype(make_range(make_skipping_mapping_iterator(f, begin(r)),
                         make_skipping_mapping_iterator(f, end(r))))
{
  return {
    make_skipping_mapping_iterator(f, begin(r)),
    make_skipping_mapping_iterator(f, end(r))
  };
}

template <class Function, class Range>
auto map_ephemeral(Function f, Range&& r)
  -> decltype(make_range(make_ephemeral_mapping_iterator(f, begin(r)),
                         make_ephemeral_mapping_iterator(f, end(r))))
{
  return {
    make_ephemeral_mapping_iterator(f, begin(r)),
    make_ephemeral_mapping_iterator(f, end(r))
  };
}

} // namespace range_util_impl

using range_util_impl::map;

namespace range_util_impl {

template <class Function, class Range>
auto filter(Function f, Range&& r)
  -> decltype(make_range(make_filtering_iterator(f, begin(r), end(r)),
                         make_filtering_iterator(f, end(r), end(r))))
{
  return {
    make_filtering_iterator(f, begin(r), end(r)),
    make_filtering_iterator(f, end(r), end(r))
  };
}

} // namespace range_util_impl

using range_util_impl::filter;

namespace range_util_impl {

template <class Function, class T, class Range>
auto reduce(Function&& f, T&& init, Range&& r)
  -> decltype(std::accumulate(begin(r), end(r), std::forward<T>(init), std::forward<Function>(f)))
{
  return std::accumulate(begin(r), end(r), std::forward<T>(init), std::forward<Function>(f));
}

template <class Function, class Range>
auto reduce(Function&& f, Range&& r)
  -> decltype(std::accumulate(begin(r), end(r), *begin(r), std::forward<Function>(f)))
{
  if (begin(r) == end(r))
    throw std::out_of_range("reduce without initial value must be called on non-empty range");
  auto first = begin(r);
  auto&& init = *first;
  ++first;
  return std::accumulate(first, end(r), std::move(init), std::forward<Function>(f));
}

} // namespace range_util_impl

using range_util_impl::reduce;

template <class Function, class Range>
auto map_indexed(Function&& f, Range&& r)
  -> decltype(map(spread(std::forward<Function>(f)),
                  zip(std::forward<Range>(r), std::declval<range<integral_iterator<size_t> > >())))
{
  return map(spread(std::forward<Function>(f)),
             zip(std::forward<Range>(r), make_range(
                   integral_iterator<size_t>(0),
                   integral_iterator<size_t>(std::numeric_limits<size_t>::max()))));
}

} // inline namespace range_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_RANGE_UTIL_HH */
