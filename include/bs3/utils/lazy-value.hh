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

#ifndef BS3_UTILS_LAZY_VALUE_HH
#define BS3_UTILS_LAZY_VALUE_HH

// lazy value as will be returned by a function_called_with_no_arguments().
// Force evaluation by operator*.

#include <type_traits>
#include <memory>

#include "optional.hh"

namespace pbsu {

inline
namespace lazyv_abiv1 {

template <class Computation>
struct lazy_value {

  using value_type = typename std::result_of<Computation()>::type;
  using reference = value_type&;
  using pointer = value_type*;

  mutable Computation computation;

private:
  mutable optional<value_type> result;

public:

  lazy_value() = default;

  lazy_value(Computation comp)
    : computation(comp)
  {}

  reference operator*() const
  {
    if (!result)
      result.emplace(computation());
    return *result;
  }

  pointer operator->() const
  {
    return std::addressof(*(*this));
  }

};

} // inline namespace lazyv_abiv1

} // namespace pbsu

#endif /* BS3_UTILS_LAZY_VALUE_HH */
