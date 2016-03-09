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

#include <random>
#include <type_traits>

// range for random size used when generating containers
#ifndef BSIC_RAND_SIZE_MAX
#  define BSIC_RAND_SIZE_MAX 8
#endif

namespace {

template <class T>
struct bsic_rgen_tag {};

std::random_device rd;
std::mt19937 gen {rd()};

template <class Int>
typename std::enable_if<std::is_integral<Int>::value, Int>::type
bsic_rgen(bsic_rgen_tag<Int>)
{
  static std::uniform_int_distribution<Int> dist;
  return dist(gen);
}

template <class Real>
typename std::enable_if<std::is_floating_point<Real>::value, Real>::type
bsic_rgen(bsic_rgen_tag<Real>)
{
  static std::uniform_real_distribution<Real> dist;
  return dist(gen);
}

template <class Enum>
typename std::enable_if<std::is_enum<Enum>::value, Enum>::type
bsic_rgen(bsic_rgen_tag<Enum>)
{
  return static_cast<Enum>(bsic_rgen(bsic_rgen_tag<typename std::underlying_type<Enum>::type>{}));
}

template <class UInt>
pbss::var_uint<UInt> bsic_rgen(bsic_rgen_tag<pbss::var_uint<UInt>>)
{
  return {bsic_rgen(bsic_rgen_tag<UInt>{})};
}

size_t bsic_rand_size()
{
  static std::uniform_int_distribution<size_t> dist(0, BSIC_RAND_SIZE_MAX);
  return dist(gen);
}

template <class T>
std::vector<T> bsic_rgen(bsic_rgen_tag<std::vector<T>>)
{
  std::vector<T> vec;
  size_t size = bsic_rand_size();
  vec.reserve(size);
  for (size_t i=0; i<size; ++i)
    vec.emplace_back(bsic_rgen(bsic_rgen_tag<T>{}));
  return vec;
}

template <class K, class V>
std::map<K, V> bsic_rgen(bsic_rgen_tag<std::map<K, V>>)
{
  std::map<K, V> map;
  size_t size = bsic_rand_size();
  for (size_t i=0; i<size; ++i)
    map.emplace(bsic_rgen(bsic_rgen_tag<K>{}), bsic_rgen(bsic_rgen_tag<V>{}));
  return map;
}

template <class Char,
          class Traits = std::char_traits<Char>,
          class Allocator = std::allocator<Char> >
std::basic_string<Char, Traits, Allocator>
bsic_rgen(bsic_rgen_tag<std::basic_string<Char, Traits, Allocator> >)
{
  std::string str(bsic_rand_size(), 0);
  for (auto &c : str)
    c = bsic_rgen(bsic_rgen_tag<char>{});
  return str;
}

} // end unnamed namespace
