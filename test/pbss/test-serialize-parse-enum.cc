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

enum class onebyte : uint8_t {
  ONE=1, TWO=2
};

enum class twobyte : uint16_t {
  ONETWO=0x0102, TWOONE=0x0201
};

int main()
{
  // should be single char
  check_serialize(onebyte::ONE, "\x1");
  check_serialize(onebyte::TWO, "\x2");

  // two char in little endian order
  check_serialize(twobyte::ONETWO, "\x2\x1");
  check_serialize(twobyte::TWOONE, "\x1\x2");

  // parse
  check_parse("\x1", onebyte::ONE);
  check_parse("\x2\x1", twobyte::ONETWO);

  // early eof error
  check_early_eof<onebyte>("");
  check_early_eof<twobyte>("a");

  // can parse const
  {
    std::istringstream in("fo");
    pbss::parse<const twobyte>(in);
  }

  // do not consume extra bytes
  check_extra_consume<twobyte>("ab");

  return 0;
}
