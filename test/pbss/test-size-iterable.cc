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
#include <string>

int main()
{

  // elements has fixed size
  check_aot_size(std::vector<char>{}, 1);
  check_aot_size(std::vector<char>{'a', 'b', 'c'}, 4);
  check_aot_size(std::vector<uint16_t>{0, 1, 2}, 7);

  check_aot_size(std::vector<uint64_t>(0x82, 0), /*varuint size*/2 + 0x82*8);

  // elements has only aot size
  check_aot_size(std::vector<std::string> {"foo", "bar"},
                 1 + 4 + 4);

  {
    std::string strs[] = {"foo", "bar"};
    check_aot_size(strs, 9);    // same as above but for C array
  }

  return 0;
}
