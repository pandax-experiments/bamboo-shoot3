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

#ifndef BS3_UTILS_FUNCTIONAL_HH
#define BS3_UTILS_FUNCTIONAL_HH

// functional utilities

#include <type_traits>

#include "type-traits.hh"
#include "tuple-util.hh"
#include "misc.hh"

namespace pbsu {

inline
namespace functional_abiv1 {

// "identity"
struct constref_forward {
  template <class T>
  constexpr const T& operator()(const T& v) const
  {
    return v;
  }
};

namespace fn_impl {

template <class Function>
struct function_wrapper {
  Function f;

  constexpr function_wrapper(Function fn)
    : f(fn)
  {}

  template <class... T>
  constexpr auto operator()(T&&... v) const -> decltype(f(std::forward<T>(v)...))
  {
    return f(std::forward<T>(v)...);
  }
};

} // namespace fn_impl

template <class Function>
using ebo_function = std::conditional<
  std::is_class<Function>::value,
  Function,
  fn_impl::function_wrapper<Function>
>;

// fold(binary_operator, values...)

namespace fn_impl {

template <size_t arity>
struct static_foldr_impl_t {
  template <class BinOp, class H, class... T>
  static constexpr auto call(BinOp&& op, H&& h, T&&... t)
    -> decltype(op((H&&)h, static_foldr_impl_t<arity-1>::call(op, (T&&)t...)))
  {
    static_assert(sizeof...(t)+1 == arity, "static_foldr: arguments count mismatch");
    return op((H&&)h, static_foldr_impl_t<arity-1>::call(op, (T&&)t...));
  }
};

template <>
struct static_foldr_impl_t<1> {
  template <class BinOp, class T>
  typename std::decay<T>::type
  static constexpr call(BinOp&&, T&& t)
  {
    return (T&&)t;
  }
};

} // namespace fn_impl

template <class BinOp, class... T>
constexpr auto static_foldr(BinOp&& op, T&&... v)
  -> decltype(fn_impl::static_foldr_impl_t<sizeof...(v)>::call(op, (T&&)v...))
{
  return fn_impl::static_foldr_impl_t<sizeof...(v)>::call(op, (T&&)v...);
}

namespace fn_impl {

template <size_t arity>
struct static_foldl_impl_t {
  template <class BinOp, class F, class S, class... T>
  static constexpr auto call(BinOp&& op, F&& f, S&& s, T&&... t)
    -> decltype(static_foldl_impl_t<arity-1>::call(op, op((F&&)f, (S&&)s), (T&&)t...))
  {
    static_assert(sizeof...(t)+2 == arity, "static_foldl: arguments count mismatch");
    return static_foldl_impl_t<arity-1>::call(op, op((F&&)f, (S&&)s), (T&&)t...);
  }
};

template <>
struct static_foldl_impl_t<1> {
  template <class BinOp, class T>
  typename std::decay<T>::type
  static constexpr call(BinOp&&, T&& t)
  {
    return (T&&)t;
  }
};

} // namespace fn_impl

template <class BinOp, class... T>
constexpr auto static_foldl(BinOp&& op, T&&... v)
  -> decltype(fn_impl::static_foldl_impl_t<sizeof...(v)>::call(op, (T&&)v...))
{
  return fn_impl::static_foldl_impl_t<sizeof...(v)>::call(op, (T&&)v...);
}

namespace fn_impl {
// call fn with arguments in reverse

template <class RestSequence, class ReversedSequence>
struct reverse_call_t;

template <class... Reversed>
struct reverse_call_t<type_sequence<>, type_sequence<Reversed...>> {
  template <class Function>
  static constexpr auto call(Function&& f, Reversed... args)
    -> decltype(f(std::forward<Reversed>(args)...))
  {
    return f(std::forward<Reversed>(args)...);
  }
};

template <class First, class... Rest, class... Reversed>
struct reverse_call_t<type_sequence<First, Rest...>, type_sequence<Reversed...>> {
  template <class Function>
  static constexpr auto call(Function&& f,
                             First&& first, Rest&&... rest, Reversed&&... reversed)
    -> decltype(
      reverse_call_t<type_sequence<Rest...>, type_sequence<First, Reversed...>>::call(
        std::forward<Function>(f), std::declval<Rest>()...,
        std::declval<First>(), std::declval<Reversed>()...))
  // above works, but the following decltype expression, which is an exact
  // copy of what is inside the function body, fails in GCC.  Both works in
  // clang.  This looks like a bug in GCC.
  //
  // -> decltype(reverse_call_t<type_sequence<Rest...>, type_sequence<First, Reversed...>>::call(
  //               std::forward<Function>(f), std::forward<Rest>(rest)...,
  //               std::forward<First>(first), std::forward<Reversed>(reversed)...))
  {
    return reverse_call_t<type_sequence<Rest...>, type_sequence<First, Reversed...>>::call(
      std::forward<Function>(f), std::forward<Rest>(rest)...,
      std::forward<First>(first), std::forward<Reversed>(reversed)...);
  }
};

} // namespace fn_impl

template <class Function, class... T>
constexpr auto reverse_call(Function&& f, T&&... args)
  -> decltype(
    fn_impl::reverse_call_t<type_sequence<T...>, type_sequence<>>::call(
      std::forward<Function>(f), std::forward<T>(args)...))
{
  return fn_impl::reverse_call_t<type_sequence<T...>, type_sequence<>>::call(
    std::forward<Function>(f), std::forward<T>(args)...);
}

namespace fn_impl {
// (f . g) x = f $ g x

template <class F, class G>
struct compose_2_t : private ebo_function<F>::type, private ebo_function<G>::type {

private:
  typedef typename ebo_function<F>::type f;
  typedef typename ebo_function<G>::type g;

public:

  constexpr compose_2_t(F _f, G _g)
    : f(_f), g(_g)
  {}

  template <class... T>
  constexpr auto operator()(T&&... values) const
    -> decltype(std::declval<const F&>()(
                  std::declval<const G&>()(
                    std::forward<T>(values)...)))
  {
    return (*static_cast<const f*>(this))(
             (*static_cast<const g*>(this))(
               std::forward<T>(values)...));
  }

};

// then, because decltype(auto) is not yet in C++11, this is struct instead
// of recursive function

template <class...>
struct compose_t;

template <class Function>
struct compose_t<Function> : private ebo_function<Function>::type {

private:
  typedef typename ebo_function<Function>::type f;

public:
  constexpr compose_t(Function _f)
    : f(_f)
  {}

  using f::operator();

};

template <class First, class... Rest>
struct compose_t<First, Rest...> : compose_2_t<First, compose_t<Rest...>>
{
private:
  typedef compose_2_t<First, compose_t<Rest...>> comp2_t;

public:
  constexpr compose_t(First f, Rest... r)
    : comp2_t(f, {r...})
  {}
};

} // namespace fn_impl

template <class... Function>
fn_impl::compose_t<typename std::decay<Function>::type...>
constexpr compose(Function&&... f)
{
  return { std::forward<Function>(f)... };
}

namespace fn_impl {
struct compose_wrapper {
  template <class... T>
  constexpr auto operator()(T&&... v) const -> decltype(compose(std::forward<T>(v)...))
  {
    return compose(std::forward<T>(v)...);
  }
};
}

template <class... Function>
constexpr auto pipe(Function&&... f)
  -> decltype(reverse_call(fn_impl::compose_wrapper(), std::forward<Function>(f)...))
{
  return reverse_call(fn_impl::compose_wrapper(), std::forward<Function>(f)...);
}

namespace fn_impl {

template <class Function>
struct spread_t : private ebo_function<Function>::type {

  typedef typename ebo_function<Function>::type function_type;

  constexpr spread_t(Function f)
    : function_type(f)
  {}

  template <class Tuple>
  constexpr auto operator()(Tuple&& t) const
    -> decltype(spread_call(std::declval<const Function&>(), std::forward<Tuple>(t)))
  {
    return spread_call(*static_cast<const function_type*>(this), std::forward<Tuple>(t));
  }
};

} // namespace fn_impl

// like partially-applied spread_call(f).
template <class Function>
fn_impl::spread_t<typename std::decay<Function>::type>
constexpr spread(Function&& f)
{
  return { f };
}

namespace fn_impl {

template <class Function, size_t... i>
struct pick_t : private ebo_function<Function>::type {

  typedef typename ebo_function<Function>::type function_type;

  constexpr pick_t(Function f)
    : function_type(f)
  {}

  template <class Tuple>
  constexpr auto operator()(Tuple&& t) const
    -> decltype(pick_call<i...>(std::declval<const Function&>(), std::forward<Tuple>(t)))
  {
    return pick_call<i...>(*static_cast<const Function*>(this), std::forward<Tuple>(t));
  }
};

} // namespace fn_impl

// like partially-applied pick_call<i...>(f).
template <size_t... i, class Function>
fn_impl::pick_t<typename std::decay<Function>::type, i...>
constexpr pick(Function&& f)
{
  return { f };
}

// and defaults to identity if picking single element
template <size_t i>
fn_impl::pick_t<constref_forward, i>
constexpr pick()
{
  return { {} };
}

namespace fn_impl {

template <class T>
struct call_with_t {
  T v;

  template <class Function>
  auto operator()(Function f) const -> decltype(f(v))
  {
    return f(v);
  }
};

} // namespace fn_impl

// flipped function apply, curried
template <class T>
fn_impl::call_with_t<T> call_with(T&& v)
{
  return { v };
}

namespace fn_impl {

template <class Tuple>
struct collect_result_t {
  Tuple fns;

  template <class T>
  auto operator()(T&& v) const
  // if any one fn modified v the results will be ... too surprising.
  // so const ref here.
    -> decltype(map_tuple(call_with<const T&>(v), fns))
  {
    return map_tuple(call_with<const T&>(v), fns);
  }
};

} // namespace fn_impl

// return a unary function, x -> tuple of fn(x) for each fn in fns
template <class... Function>
fn_impl::collect_result_t<std::tuple<Function...>>
collect_result(Function... fns)
{
  return { std::make_tuple(fns...) };
}

} // inline namespace functional_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_FUNCTIONAL_HH */
