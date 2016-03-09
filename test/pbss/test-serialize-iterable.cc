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
#include <string>
#include <array>
#include <list>

int main()
{

  // C array
  {
    int16_t arr[3] = {1, 2, -1};
    check_serialize(arr, {3, 1, 0, 2, 0, '\xff', '\xff'});
  }

  // vector
  check_serialize(std::vector<char> {'a', 'b', 'c'}, "\x3""abc");
  // 2-byte length
  // 0x82 == 0x80 + 2, 0x80 goes to next byte as 1, and 2|0x80 == 0x82
  check_serialize(std::vector<char>(0x82, 0),
                  std::string("\x82\x1")+std::string(0x82, 0));
  // zero length
  check_serialize(std::vector<char>(), {0});

  // string
  check_serialize(std::string("abc"), "\x3""abc");
  check_serialize(std::string(), {0});

  // static array
  check_serialize(std::array<char, 3>{{'a', 'b', 'c'}}, "\x3""abc");
  check_serialize(std::array<char, 0>{{}}, {0});

  // list
  check_serialize(std::list<char>{'a', 'b', 'c'}, "\x3""abc");

  return 0;
}
