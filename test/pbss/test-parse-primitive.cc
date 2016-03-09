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
  check_parse("a", 'a');
  check_parse("\x1\x2", (uint16_t)0x0201);
  check_parse("\x1\x2\x3\x4", (uint32_t)0x04030201);
  check_parse("\x1\x2\x3\x4\x5\x6\x7\x8", (uint64_t)0x0807060504030201);

  // signed
  check_parse("\xff", (int8_t)-1);
  check_parse("\xff\xff", (int16_t)-1);
  check_parse("\xff\xff\xff\xff", (int32_t)-1);
  check_parse("\xff\xff\xff\xff\xff\xff\xff\xff", (int64_t)-1);

  // float & double
  check_parse({0, 0, '\x90', '\x40'}, (float)4.5);
  check_parse({0, 0, 0, 0, 0, 0, '\x12', '\x40'}, (double)4.5);
  check_parse({0, 0, '\x80', '\x7f'}, std::numeric_limits<float>::infinity());
  check_parse({0, 0, 0, 0, 0, 0, '\xf0', '\x7f'}, std::numeric_limits<double>::infinity());
  check_parse({0, 0, '\x80', '\xff'}, -std::numeric_limits<float>::infinity());
  check_parse({0, 0, 0, 0, 0, 0, '\xf0', '\xff'}, -std::numeric_limits<double>::infinity());

  // early eof
  check_early_eof<char>("");
  check_early_eof<uint16_t>("a");

  // can parse const qualified type
  {
    std::istringstream in(std::string("a"));
    pbss::parse<const char>(in);
  }

  // do not consume extra bytes
  check_extra_consume<uint32_t>("abcd");

  return 0;
}
