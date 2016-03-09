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

#ifndef BS3_UTILS_OPTIONAL_HH
#define BS3_UTILS_OPTIONAL_HH

// a currently incomplete implementation of std::experimental::optional as
// specified by Library TS.  Some operations are not provided, or missing
// some overloads/constexpr-qualifications; but provided operations must
// match exactly the semantics required by that specification, unless
// limited by missing (C++14) features in C++11.

#include <type_traits>
#include <utility>
#include <new>

#include "gcc48-warning.hh"
#include "type-traits.hh"

namespace pbsu {

inline
namespace optional_abiv1 {

namespace optional_impl {

using std::swap;

// only declared, not defined; because it is used only for noexcept value
template <class T>
void adl_swap(T& a, T& b)
  noexcept(noexcept(swap(a, b)));

} // namespace optional_impl

struct nullopt_t {
  // used to construct nullopt_t
  class _construct {};
  constexpr nullopt_t(_construct)
  {}
};

constexpr nullopt_t nullopt { nullopt_t::_construct{} };

struct in_place_t {};

constexpr in_place_t in_place {};

template <class T>
class optional;

namespace optional_impl {

template <class T, bool=std::is_trivially_destructible<T>::value>
struct crtp_destruct_base {
  ~crtp_destruct_base()
  {
    (*static_cast<optional<T>*>(this)) = nullopt;
  }
};

template <class T>
struct crtp_destruct_base<T, true>
{};

} // namespace optional_impl

template <class T>
class optional
  : public optional_impl::crtp_destruct_base<T> {

public:

  typedef T value_type;

private:

  char storage alignas(value_type) [sizeof(value_type)];

  // All constructors start with has_value = false, then re-assign it to
  // true after construction of embedded value if any.  This way
  // constructor exceptions from value_type can be properly handled,
  // because dtor in base class is always run.
  bool has_value { false };

  template <class... Arg>
  void construct(Arg&&... arg)
    noexcept(std::is_nothrow_constructible<value_type, Arg...>())
  {
    ::new (storage) value_type(std::forward<Arg>(arg)...);
    has_value = true;
  }

  void destruct()
  {
    has_value = false;
    (*this)->~value_type();
  }

public:

  constexpr explicit operator bool() const noexcept
  {
    return has_value;
  }

  value_type& operator*() &
  {
    return reinterpret_cast<value_type&>(storage);
  }

  constexpr const value_type& operator*() const&
  {
    return reinterpret_cast<const value_type&>(storage);
  }

  value_type&& operator*() &&
  {
    return reinterpret_cast<value_type&&>(storage);
  }

#ifndef BS3_DETECTED_GCC_48
  constexpr const value_type&& operator*() const&&
  {
    return reinterpret_cast<const value_type&&>(storage);
  }
#endif

  value_type* operator->()
  {
    return reinterpret_cast<value_type*>(storage);
  }

  const value_type* operator->() const
  {
    return reinterpret_cast<const value_type*>(storage);
  }

  constexpr optional() noexcept = default;

  constexpr optional(nullopt_t) noexcept
  {}

  optional(const value_type& src)
  {
    this->construct(src);
  }

  optional(value_type&& src)
  {
    this->construct(std::move(src));
  }

  template <class... Arg>
  explicit optional(in_place_t, Arg&&... arg)
  {
    this->construct(std::forward<Arg>(arg)...);
  }

  optional(const optional& other)
  {
    if (other)
      this->construct(*other);
  }

  optional(optional&& other)
    noexcept(std::is_nothrow_move_constructible<value_type>())
  {
    if (other)
      this->construct(std::move(*other));
  }

  optional& operator=(nullopt_t)
  {
    if (has_value)
      destruct();
    return *this;
  }

  optional& operator=(const optional& other)
  {
    if (*this && other)
      *(*this) = *other;
    else {
      if (other)
        this->construct(*other);
      else (*this) = nullopt;
    }
    return *this;
  }

  optional& operator=(optional&& other)
    noexcept(std::is_nothrow_move_constructible<value_type>()
             && std::is_nothrow_move_assignable<value_type>())
  {
    if (*this && other)
      *(*this) = std::move(*other);
    else {
      if (other)
        this->construct(std::move(*other));
      else (*this) = nullopt;
    }
    return *this;
  }

  template <class U>
  typename std::enable_if<std::is_same<value_type, typename std::decay<U>::type>::value,
                          optional&>::type
  operator=(U&& u)
  {
    if (has_value)
      *(*this) = std::forward<U>(u);
    else this->construct(std::forward<U>(u));
    return *this;
  }

  template <class... Arg>
  typename std::enable_if<std::is_constructible<T, Arg...>::value>::type
  emplace(Arg&&... arg)
  {
    (*this) = nullopt;
    this->construct(std::forward<Arg>(arg)...);
  }

  void swap(optional& other)
    noexcept(std::is_nothrow_move_constructible<value_type>()
             && noexcept(optional_impl::adl_swap(std::declval<value_type&>(),
                                                 std::declval<value_type&>())))
  {
    using std::swap;

    if (*this && other)
      swap(*(*this), *other);
    else {
      if (*this)
        other.construct(std::move(*this)), this->destruct();
      else if (other)
        this->construct(std::move(*other)), other.destruct();
    }
  }

};

// comparison to nullopt
template <class T>
constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept
{
  return !x;
}

template <class T>
constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept
{
  return !x;
}

template <class T>
constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept
{
  return bool(x);
}

template <class T>
constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept
{
  return bool(x);
}

template <class T>
constexpr bool operator<(const optional<T>&, nullopt_t) noexcept
{
  return false;
}

template <class T>
constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept
{
  return bool(x);
}

template <class T>
constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept
{
  return !x;
}

template <class T>
constexpr bool operator<=(nullopt_t, const optional<T>&) noexcept
{
  return true;
}

template <class T>
constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept
{
  return bool(x);
}

template <class T>
constexpr bool operator>(nullopt_t, const optional<T>&) noexcept
{
  return false;
}

template <class T>
constexpr bool operator>=(const optional<T>&, nullopt_t) noexcept
{
  return true;
}

template <class T>
constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept
{
  return !x;
}

// comparison to another optional
template <class T>
constexpr auto operator==(const optional<T>& a, const optional<T>& b)
  -> decltype(implicit_bool(*a == *b))
{
  return a && b ? *a == *b : bool(a) == bool(b);
}

template <class T>
constexpr auto operator!=(const optional<T>& a, const optional<T>& b)
  -> decltype(a==b)
{
  return !(a==b);
}

template <class T>
constexpr auto operator<(const optional<T>& a, const optional<T>& b)
  -> decltype(implicit_bool(*a < *b))
{
  return a && b ? *a < *b : bool(a) < bool(b);
}

template <class T>
constexpr auto operator<=(const optional<T>& a, const optional<T>& b)
  -> decltype(a < b)
{
  return !(b < a);
}

template <class T>
constexpr auto operator>(const optional<T>& a, const optional<T>& b)
  -> decltype(a < b)
{
  return b < a;
}

template <class T>
constexpr auto operator>=(const optional<T>& a, const optional<T>& b)
  -> decltype(a < b)
{
  return !(a < b);
}

// comparison with value
template <class T>
constexpr auto operator==(const optional<T>& x, const T& v)
  -> decltype(implicit_bool(*x == v))
{
  return x && bool(*x==v);
}

template <class T>
constexpr auto operator==(const T& v, const optional<T>& x)
  -> decltype(implicit_bool(*x == v))
{
  return x && bool(*x==v);
}

template <class T>
constexpr auto operator!=(const optional<T>& x, const T& v)
  -> decltype(x==v)
{
  return !(x==v);
}

template <class T>
constexpr auto operator!=(const T& v, const optional<T>& x)
  -> decltype(x==v)
{
  return !(x==v);
}

template <class T>
constexpr auto operator<(const optional<T>& x, const T& v)
  -> decltype(implicit_bool(*x < v))
{
  return (!x) || bool(*x < v);
}

template <class T>
constexpr auto operator<(const T& v, const optional<T>& x)
  -> decltype(implicit_bool(v < *x))
{
  return x && bool(v < *x);
}

template <class T>
constexpr auto operator<=(const optional<T>& x, const T& v)
  -> decltype(v < x)
{
  return !(v < x);
}

template <class T>
constexpr auto operator<=(const T& v, const optional<T>& x)
  -> decltype(x < v)
{
  return !(x < v);
}

template <class T>
constexpr auto operator>(const optional<T>& x, const T& v)
  -> decltype(v < x)
{
  return v < x;
}

template <class T>
constexpr auto operator>(const T& v, const optional<T>& x)
  -> decltype(x < v)
{
  return x < v;
}

template <class T>
constexpr auto operator>=(const optional<T>& x, const T& v)
  -> decltype(x < v)
{
  return !(x < v);
}

template <class T>
constexpr auto operator>=(const T& v, const optional<T>& x)
  -> decltype(v < x)
{
  return !(v < x);
}

// helper make_optional

template <class T>
optional<typename std::decay<T>::type>
make_optional(T&& v)
{
  return { std::forward<T>(v) };
}

} // inline namespace optional_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_OPTIONAL_HH */
