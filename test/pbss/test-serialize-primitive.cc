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

#include <limits>

int main()
{
  // unsigned int
  check_serialize('a', "a");  // char
  check_serialize((uint16_t)0x0201, "\x1\x2"); // uint16
  check_serialize((uint32_t)0x04030201, "\x1\x2\x3\x4"); // uint32
  check_serialize((uint64_t)0x0807060504030201, "\x1\x2\x3\x4\x5\x6\x7\x8"); // uint64

  // signed counterparts
  check_serialize((int8_t)-1, "\xff");
  check_serialize((int16_t)-1, "\xff\xff");
  check_serialize((int32_t)-1, "\xff\xff\xff\xff");
  check_serialize((int64_t)-1, "\xff\xff\xff\xff\xff\xff\xff\xff");

  // float & double
  check_serialize((float)4.5, {0, 0, '\x90', '\x40'});
  check_serialize((double)4.5, {0, 0, 0, 0, 0, 0, '\x12', '\x40'});
  check_serialize(std::numeric_limits<float>::infinity(), {0, 0, '\x80', '\x7f'});
  check_serialize(std::numeric_limits<double>::infinity(), {0, 0, 0, 0, 0, 0, '\xf0', '\x7f'});
  check_serialize(-std::numeric_limits<float>::infinity(), {0, 0, '\x80', '\xff'});
  check_serialize(-std::numeric_limits<double>::infinity(), {0, 0, 0, 0, 0, 0, '\xf0', '\xff'});

  return 0;
}
