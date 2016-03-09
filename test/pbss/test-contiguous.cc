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

#include "checker.hh"

#include <vector>
#include <array>

struct whatever {};

int main()
{

  using pbss::is_memory_layout;

  static_assert(!is_memory_layout<whatever>(), "Default should be false");

  static_assert(is_memory_layout<char>(), "Primitives should be memory layout");
  static_assert(is_memory_layout<short>(), "Primitives should be memory layout");
  static_assert(is_memory_layout<int>(), "Primitives should be memory layout");
  static_assert(is_memory_layout<float>(), "Primitives should be memory layout");

  using pbss::homoseq_impl::is_contiguous_container;
  static_assert(!is_contiguous_container<whatever>(), "Default should be false");
  static_assert(is_contiguous_container<int[3]>(), "Arrays should be contiguous");
  static_assert(is_contiguous_container<std::string>(), "Strings should be contiguous");
  static_assert(is_contiguous_container<std::vector<int>>(), "Vectors should be contiguous");
  static_assert(is_contiguous_container<std::array<int, 4>>(), "Vectors should be contiguous");

  return 0;
}
