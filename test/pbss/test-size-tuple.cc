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

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&all_fixed::a),
    PBSS_TUPLE_MEMBER(&all_fixed::b));
};

struct some_aot {
  int8_t a;
  std::vector<char> b;

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&some_aot::a),
    PBSS_TUPLE_MEMBER(&some_aot::b));
};

int main()
{

  check_fixed_size(all_fixed(), 9);

  check_aot_size(some_aot{1, std::vector<char>(0x82, 0)}, 1+2+0x82);

  return 0;
}
