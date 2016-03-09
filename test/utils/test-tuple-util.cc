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

#include <cassert>

#include <bs3/utils/tuple-util.hh>

int main()
{

  using pbsu::spread_call;
  using pbsu::pick_call;

  auto add = [](int a, long b) { return a+b; };

  // prvalue
  assert((spread_call(add, std::make_tuple(1, 2)) == 3));

  {
    // lvalue reference
    auto t = std::make_tuple(1, 2);
    (void) spread_call(add, t);
  }

  // prvalue
  assert((pick_call<2, 4>(add, std::make_tuple(1, 2, 3, 4, 5)) == 8));

  {
    // lvalue reference
    auto t = std::make_tuple(1, 2, 3, 4, 5);
    (void) pick_call<2, 4>(add, t);
  }

  {
    // map_tuple
    using pbsu::map_tuple;
    struct {
      int operator()(int) { return 1; }
      double operator()(double) { return 2; }
    } variadic;
    auto mapped = map_tuple(variadic, std::tuple<int, double>{});
    assert((mapped == std::tuple<int, double> { 1, 2 }));
  }

  return 0;
}
