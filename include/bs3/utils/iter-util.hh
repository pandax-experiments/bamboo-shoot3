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

#ifndef BS3_UTILS_ITER_UTIL_HH
#define BS3_UTILS_ITER_UTIL_HH

// iterator utilities directly or indirectly useful on ranges

#include <iterator>
#include <utility>
#include <type_traits>
#include <tuple>
#include <memory>

#include "type-traits.hh"
#include "bp-14.hh"
#include "misc.hh"
#include "functional.hh"
#include "optional.hh"

namespace pbsu {

inline
namespace iter_abiv1 {

template <class RealType>
struct output_iterator_mixin {

  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  RealType& operator*()
  {
    return *static_cast<RealType*>(this);
  }

  RealType& operator++()
  {
    return *static_cast<RealType*>(this);
  }

  RealType& operator++(int)
  {
    return *static_cast<RealType*>(this);
  }

};

template <class Function, class OutputIterator,
          // one check here, *iter=value below
          class=decltype(++std::declval<OutputIterator&>())>
struct transforming_output_iterator
  : output_iterator_mixin<transforming_output_iterator<Function, OutputIterator> > {

private:
  Function fn;
  OutputIterator iter;

public:

  transforming_output_iterator(Function f, OutputIterator i)
    : fn(f), iter(i)
  {}

  transforming_output_iterator& operator++()
  {
    ++iter;
    return *this;
  }

  transforming_output_iterator operator++(int)
  {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  template <class T, class=decltype(*iter=fn(std::declval<T>()))>
  transforming_output_iterator& operator=(T&& v)
  {
    *iter = fn(std::forward<T>(v));
    return *this;
  }

};

template <class Function, class OutputIterator>
transforming_output_iterator<typename std::decay<Function>::type,
                             typename std::decay<OutputIterator>::type>
transformed_writer(Function&& f, OutputIterator&& it)
{
  return { std::forward<Function>(f), std::forward<OutputIterator>(it) };
}

namespace iter_util_impl {

template <class Tuple, size_t... i>
void increment_iter_tuple(Tuple& iters, std_bp::index_sequence<i...>)
{
  using noop = int[];
  (void) noop { (++std::get<i>(iters), 0)... };
}

template <class Tuple, class T, size_t... i>
void write_to_iter_tuple(Tuple& iters, T&& v, std_bp::index_sequence<i...>)
{
  using noop = int[];
  (void) noop { (*std::get<i>(iters) = std::forward<T>(v), 0)... };
}

} // namespace iter_util_impl

template <class... OutputIterator>
struct multiplex_output_iterator
  : output_iterator_mixin<multiplex_output_iterator<OutputIterator...> > {

  static const size_t iterators_count = sizeof...(OutputIterator);

  multiplex_output_iterator(OutputIterator... i)
    : iters{i...}
  {}

  template <class T>
  multiplex_output_iterator& operator=(T&& v)
  {
    iter_util_impl::write_to_iter_tuple(iters, std::forward<T>(v),
                                        std_bp::make_index_sequence<iterators_count>());
    return *this;
  }

  multiplex_output_iterator& operator++()
  {
    iter_util_impl::increment_iter_tuple(iters,
                                         std_bp::make_index_sequence<iterators_count>());
    return *this;
  }

  multiplex_output_iterator operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

private:
  std::tuple<OutputIterator...> iters;

};

template <class... OutputIterator>
multiplex_output_iterator<typename std::decay<OutputIterator>::type...>
multiplex_writer(OutputIterator&&... i)
{
  return { std::forward<OutputIterator>(i)... };
}

namespace iter_util_impl {

template <size_t... i, class... Iterator>
auto spread_writer(std_bp::index_sequence<i...>, Iterator&&... it)
  -> decltype(
    multiplex_writer(
      transformed_writer(pick<i>(constref_forward()), std::forward<Iterator>(it))...))
{
  return multiplex_writer(
    transformed_writer(pick<i>(constref_forward()), std::forward<Iterator>(it))...);
}

} // namespace iter_util_impl

// writes input tuple to multiple destinations, by index
template <class... Iterator>
auto spread_writer(Iterator&&... it)
  -> decltype(
    iter_util_impl::spread_writer(
      std_bp::make_index_sequence<sizeof...(Iterator)>(),
      std::forward<Iterator>(it)...))
{
  return iter_util_impl::spread_writer(
    std_bp::make_index_sequence<sizeof...(Iterator)>(),
    std::forward<Iterator>(it)...);
}

template <class Predicate, class OutputIterator>
struct filtering_output_iterator
  : output_iterator_mixin<filtering_output_iterator<Predicate, OutputIterator> > {

private:
  Predicate pred;
  OutputIterator iter;

public:

  filtering_output_iterator(Predicate p, OutputIterator it)
    : pred(p), iter(it)
  {}

  template <class T>
  filtering_output_iterator& operator=(T&& v)
  {
    if (pred(v))
      *iter=std::forward<T>(v), ++iter;
    return *this;
  }

};

template <class Predicate, class OutputIterator>
filtering_output_iterator<typename std::decay<Predicate>::type,
                          typename std::decay<OutputIterator>::type>
filtering_writer(Predicate&& pred, OutputIterator&& out)
{
  return { std::forward<Predicate>(pred), std::forward<OutputIterator>(out) };
}

template <class Function, class Iterator, bool skipUnused=false>
struct mapping_iterator {

  typedef std::input_iterator_tag iterator_category;
  typedef typename std::iterator_traits<Iterator>::difference_type difference_type;

  typedef typename std::iterator_traits<Iterator>::value_type source_type;
  typedef typename std::iterator_traits<Iterator>::reference source_reference;
  typedef Function function_type;
  typedef typename std::result_of<Function(source_reference)>::type value_type;

  typedef const value_type& reference;
  typedef const value_type* pointer;

private:

  struct holder_t : public ebo_function<Function>::type {

    using F = typename ebo_function<Function>::type;

    Iterator it;
    optional<value_type> value;

    holder_t(Function f, Iterator i)
      : F(f), it(i)
    {}

  } mutable holder;

public:

  mapping_iterator(Function f, Iterator i)
    : holder(f, i)
  {}

  reference operator*() const
  {
    if (!holder.value)
      holder.value.emplace(holder(*holder.it));
    return *holder.value;
  }

  mapping_iterator& operator++()
  {
    if (!skipUnused)
      (void)*(*this);
    holder.value = nullopt;
    ++holder.it;
    return *this;
  }

  pointer operator->() const
  {
    return &(*(*this));
  }

  mapping_iterator operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() == std::declval<I>()))
  operator==(const mapping_iterator& other) const
  {
    return holder.it == other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() != std::declval<I>()))
  operator!=(const mapping_iterator& other) const
  {
    return holder.it != other.holder.it;
  }

};

template <class Function, class Iterator>
mapping_iterator<typename std::decay<Function>::type,
                 typename std::decay<Iterator>::type>
make_mapping_iterator(Function&& f, Iterator&& it)
{
  return { std::forward<Function>(f), std::forward<Iterator>(it) };
}

template <class Function, class Iterator>
mapping_iterator<typename std::decay<Function>::type,
                 typename std::decay<Iterator>::type,
                 true>
make_skipping_mapping_iterator(Function&& f, Iterator&& it)
{
  return { std::forward<Function>(f), std::forward<Iterator>(it) };
}

template <class Function, class Iterator>
struct ephemeral_mapping_iterator {

  typedef typename std::iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename std::iterator_traits<Iterator>::difference_type difference_type;

  typedef typename std::iterator_traits<Iterator>::value_type source_type;
  typedef typename std::iterator_traits<Iterator>::reference source_reference;
  typedef typename ebo_function<Function>::type function_type;
  typedef typename std::result_of<Function(source_reference)>::type value_type;

  typedef value_type reference;
  typedef std::unique_ptr<value_type> pointer;

private:

  struct holder_t : public ebo_function<Function>::type {

    using F = typename ebo_function<Function>::type;

    Iterator it;

    holder_t(Function f, Iterator i)
      : F(f), it(i)
    {}

  } mutable holder;

public:

  ephemeral_mapping_iterator(Function f, Iterator i)
    : holder(f, i)
  {}

  reference operator*() const
  {
    return holder(*holder.it);
  }

  ephemeral_mapping_iterator& operator++()
  {
    ++holder.it;
    return *this;
  }

  pointer operator->() const
  {
    return pointer(new value_type(*(*this)));
  }

  ephemeral_mapping_iterator operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() == std::declval<I>()))
  operator==(const ephemeral_mapping_iterator& other) const
  {
    return holder.it == other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() != std::declval<I>()))
  operator!=(const ephemeral_mapping_iterator& other) const
  {
    return holder.it != other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() < std::declval<I>()))
  operator<(const ephemeral_mapping_iterator& other) const
  {
    return holder.it < other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() <= std::declval<I>()))
  operator<=(const ephemeral_mapping_iterator& other) const
  {
    return holder.it <= other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() > std::declval<I>()))
  operator>(const ephemeral_mapping_iterator& other) const
  {
    return holder.it > other.holder.it;
  }

  template <class I = Iterator>
  decltype(implicit_bool(std::declval<I>() >= std::declval<I>()))
  operator>=(const ephemeral_mapping_iterator& other) const
  {
    return holder.it >= other.holder.it;
  }

  template <class I = Iterator>
  decltype(--std::declval<I&>(), std::declval<ephemeral_mapping_iterator&>())
  operator--()
  {
    --holder.it;
    return *this;
  }

  template <class I = Iterator, class=decltype(--std::declval<I&>())>
  ephemeral_mapping_iterator operator--(int)
  {
    auto copy = *this;
    --*this;
    return copy;
  }

  template <class Int>
  auto operator+=(Int n) -> decltype(holder.it+=n, std::declval<ephemeral_mapping_iterator&>())
  {
    holder.it+=n;
    return *this;
  }

  template <class Int>
  auto operator-=(Int n) -> decltype(holder.it-=n, std::declval<ephemeral_mapping_iterator&>())
  {
    holder.it-=n;
    return *this;
  }

  template <class Int, class=decltype(holder.it+=std::declval<Int>())>
  ephemeral_mapping_iterator operator+(Int n) const
  {
    auto copy = *this;
    copy += n;
    return copy;
  }

  template <class Int, class=decltype(holder.it-=std::declval<Int>())>
  ephemeral_mapping_iterator operator-(Int n) const
  {
    auto copy = *this;
    copy -= n;
    return copy;
  }

  template <class I = Iterator>
  decltype(std::declval<I>() - std::declval<I>())
  operator-(const ephemeral_mapping_iterator& other) const
  {
    return holder.it - other.holder.it;
  }

  template <class Int>
  auto operator[](Int n) -> decltype(*(*this+n)) const
  {
    return *(*this+n);
  }

};

template <class Function, class Iterator>
ephemeral_mapping_iterator<typename std::decay<Function>::type,
                           typename std::decay<Iterator>::type>
make_ephemeral_mapping_iterator(Function&& f, Iterator&& it)
{
  return { std::forward<Function>(f), std::forward<Iterator>(it) };
}

template <class Function, class Iterator>
struct filtering_iterator {

  typedef typename most_derived_common_base<
    std::forward_iterator_tag,
    typename std::iterator_traits<Iterator>::iterator_category
  >::type
  iterator_category;

  typedef typename ebo_function<Function>::type predicate_type;

  typedef typename std::iterator_traits<Iterator>::difference_type difference_type;
  typedef typename std::iterator_traits<Iterator>::value_type value_type;
  typedef typename std::iterator_traits<Iterator>::reference reference;
  typedef typename std::iterator_traits<Iterator>::pointer pointer;

private:

  struct holder_t : public ebo_function<Function>::type {

    using F = typename ebo_function<Function>::type;

    Iterator it, end;

    holder_t(Function f, Iterator i, Iterator e)
      : F(f), it(i), end(e)
    {}

  } mutable holder;

  void forward_skip()
  {
    while (holder.it!=holder.end && !holder(*holder.it))
      ++holder.it;
  }

public:

  filtering_iterator(Function _f, Iterator _it, Iterator _end)
    : holder(_f, _it, _end)
  {
    forward_skip();
  }

  reference operator*() const
  {
    return *holder.it;
  }

  filtering_iterator& operator++()
  {
    ++holder.it;
    forward_skip();
    return *this;
  }

  pointer operator->() const
  {
    return holder.it;
  }

  filtering_iterator operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

  bool operator==(const filtering_iterator& other) const
  {
    return holder.it == other.holder.it;
  }

  bool operator!=(const filtering_iterator& other) const
  {
    return holder.it != other.holder.it;
  }

};

template <class Function, class Iterator>
filtering_iterator<typename std::decay<Function>::type,
                   typename std::decay<Iterator>::type>
make_filtering_iterator(Function&& f, Iterator&& it,
                        typename std::decay<Iterator>::type&& end)
{
  return {
    std::forward<Function>(f),
    std::forward<Iterator>(it),
    std::forward<typename std::decay<Iterator>::type>(end)
  };
}

template <class Int>
struct integral_iterator {

  static_assert(std::is_integral<Int>::value,
                "integral_iterator must be used on integral types");

  typedef Int value_type;
  typedef const Int& reference;
  typedef const Int* pointer;
  typedef typename std::make_signed<Int>::type difference_type;
  typedef std::random_access_iterator_tag iterator_category;

private:
  Int value;

public:

  integral_iterator()
    : value()
  {}

  explicit integral_iterator(Int v)
    : value(v)
  {}

  // input iterator ops

  reference operator*() const
  {
    return value;
  }

  pointer operator->() const    // really needed?
  {
    return &value;
  }

  integral_iterator& operator++()
  {
    ++value;
    return *this;
  }

  integral_iterator operator++(int)
  {
    auto copy = *this;
    ++*this;
    return copy;
  }

  bool operator==(const integral_iterator& other) const
  {
    return value == other.value;
  }

  bool operator!=(const integral_iterator& other) const
  {
    return value != other.value;
  }

  // above satisfies forward iterator

  // then bidirectional iterator ops

  integral_iterator& operator--()
  {
    --value;
    return *this;
  }

  integral_iterator operator--(int)
  {
    auto copy = *this;
    --*this;
    return copy;
  }

  // random iterator ops

  integral_iterator& operator+=(difference_type n)
  {
    value += (Int)n;
    return *this;
  }

  integral_iterator& operator-=(difference_type n)
  {
    value -= (Int)n;
    return *this;
  }

  integral_iterator operator+(difference_type n) const
  {
    return integral_iterator(value+(Int)n);
  }

  integral_iterator operator-(difference_type n) const
  {
    return integral_iterator(value-(Int)n);
  }

  difference_type operator-(const integral_iterator& other) const
  {
    return difference_type(value - other.value);
  }

  value_type operator[](difference_type n) const
  {
    return value + (Int)n;
  }

  bool operator<(const integral_iterator& other) const
  {
    return value < other.value;
  }

  bool operator<=(const integral_iterator& other) const
  {
    return value <= other.value;
  }

  bool operator>(const integral_iterator& other) const
  {
    return value > other.value;
  }

  bool operator>=(const integral_iterator& other) const
  {
    return value >= other.value;
  }

};

// std::insert_iterator requires member typedef value_type and only writes
// one type.

template <class Container>
struct variadic_insert_iterator : output_iterator_mixin<variadic_insert_iterator<Container> > {

private:
  Container* p_cont;

public:

  variadic_insert_iterator(Container& cont)
    : p_cont(&cont)
  {}

  template <class T>
  variadic_insert_iterator& operator=(T&& v)
  {
    p_cont->insert((T&&)v);
    return *this;
  }

};

} // inline namespace iter_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_ITER_UTIL_HH */
