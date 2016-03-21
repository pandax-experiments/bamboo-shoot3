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
namespace optional_abiv2 {

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
class optional_holder {

protected:

  struct empty_t {};

  union {
    empty_t empty;
    T value;
  };
  bool has_value = false;

  template <class... Arg>
  void construct(Arg&&... arg)
    noexcept(std::is_nothrow_constructible<T, Arg...>::value)
  {
    ::new (std::addressof(value)) T((Arg&&)arg...);
    has_value = true;
  }

  void destruct()
  {
    value.~T();
    has_value = false;
  }

public:

  ~optional_holder()
  {
    if (has_value)
      value.~T();
  }

  constexpr optional_holder() noexcept
    : empty()
  {}

  optional_holder(const optional_holder& other)
    : empty(), has_value(other.has_value)
  {
    if (has_value)
      this->construct(other.value);
  }

  optional_holder(optional_holder&& other)
    : empty(), has_value(other.has_value)
  {
    if (has_value)
      this->construct((T&&)other.value);
  }

  template <class... Arg>
  optional_holder(in_place_t, Arg&&... arg)
    noexcept(std::is_nothrow_constructible<T, Arg...>::value)
    : value((Arg&&)arg...), has_value(true)
  {}

};

template <class T>
class optional_holder<T, true> {

protected:

  struct empty_t {};

  union {
    empty_t empty;
    T value;
  };
  bool has_value = false;

  template <class... Arg>
  void construct(Arg&&... arg)
    noexcept(std::is_nothrow_constructible<T, Arg...>::value)
  {
    ::new (std::addressof(value)) T((Arg&&)arg...);
    has_value = true;
  }

  void destruct()
  {
    value.~T();
    has_value = false;
  }

public:

  constexpr optional_holder() noexcept
    : empty()
  {}

  optional_holder(const optional_holder& other)
    : empty(), has_value(other.has_value)
  {
    if (has_value)
      this->construct(other.value);
  }

  optional_holder(optional_holder&& other)
    : empty(), has_value(other.has_value)
  {
    if (has_value)
      this->construct((T&&)other.value);
  }

  template <class... Arg>
  constexpr optional_holder(in_place_t, Arg&&... arg)
    noexcept(std::is_nothrow_constructible<T, Arg...>::value)
    : value((Arg&&)arg...), has_value(true)
  {}

};

} // namespace optional_impl

template <class T>
class optional
  : public optional_impl::optional_holder<T> {

public:

  typedef T value_type;

  constexpr explicit operator bool() const noexcept
  {
    return this->has_value;
  }

  value_type& operator*() &
  {
    return this->value;
  }

  constexpr const value_type& operator*() const&
  {
    return this->value;
  }

  value_type&& operator*() &&
  {
    return std::move(this->value);
  }

#ifndef BS3_DETECTED_GCC_48
  constexpr const value_type&& operator*() const&&
  {
    return std::move(this->value);
  }
#endif

  value_type* operator->()
  {
    return std::addressof(this->value);
  }

  const value_type* operator->() const
  {
    return std::addressof(this->value);
  }

private:
  using base = optional_impl::optional_holder<T>;
public:

  // only in_place_t
  using base::base;

  constexpr optional() noexcept = default;
  optional(const optional&) = default;
  optional(optional&&) = default;

  constexpr optional(nullopt_t) noexcept {}

  constexpr optional(const value_type& v)
    : base(in_place, v)
  {}

  constexpr optional(value_type&& v)
    : base(in_place, std::move(v))
  {}

  optional& operator=(nullopt_t)
  {
    if (this->has_value)
      this->destruct();
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
    if (this->has_value)
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

} // inline namespace optional_abiv2

} // namespace pbsu

#endif /* BS3_UTILS_OPTIONAL_HH */
