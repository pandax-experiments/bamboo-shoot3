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

#ifndef BS3_UTILS_MISC_HH
#define BS3_UTILS_MISC_HH

#include <type_traits>
#include <limits>
#include <stdexcept>

namespace pbsu {

template <class Num>
constexpr Num sumall(Num n)
{
  return n;
}

template <class Num, class ...T>
constexpr Num sumall(Num n, T... rest)
{
  return n + sumall(rest...);
}

// sign conversion helper
template <class Signed>
typename std::make_unsigned<Signed>::type
to_unsigned(Signed x)
{
  using Unsigned = typename std::make_unsigned<Signed>::type;
#if defined(DEBUG) || !defined(NDEBUG)
  if (x < 0)
    throw std::out_of_range("Cast of negative value to unsigned type");
#endif
  return static_cast<Unsigned>(x);
}

template <class Unsigned>
typename std::make_signed<Unsigned>::type
to_signed(Unsigned x)
{
  using Signed = typename std::make_signed<Unsigned>::type;
#if defined(DEBUG) || !defined(NDEBUG)
  if (x > std::numeric_limits<Signed>::max())
    throw std::out_of_range("Unsigned value too large to fit in signed type");
#endif
  return static_cast<Signed>(x);
}

} // namespace pbsu

#endif /* BS3_UTILS_MISC_HH */
