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

struct all_fixed {
  int8_t a;
  uint64_t b;

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &all_fixed::a),
    PBSS_TAG_MEMBER(2, &all_fixed::b));
};

struct some_aot {
  int8_t a;
  std::vector<char> b;

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &some_aot::a),
    PBSS_TAG_MEMBER(2, &some_aot::b));
};

int main()
{

  // a: 1(tag) + 1(size) + 1(itself) = 3
  // b: 1(tag) + 1(size) + 8(itself) = 10
  // + 1(tail) = 14
  check_fixed_size(all_fixed(), 14);

  // a: 1(tag) + 1(size) + 1(itself) = 3
  // b: 1(tag) + 2(size) + 0x84(itself) = 0x87
  // + 1(tail) = 0x8b
  using pbss::aot_size;
  check_aot_size(some_aot{1, std::vector<char>(0x82, 0)}, 0x8b);

  return 0;
}
