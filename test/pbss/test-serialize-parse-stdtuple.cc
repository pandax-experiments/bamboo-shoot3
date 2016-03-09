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

int main()
{
  // serialize
  check_serialize(std::make_tuple('a'), "a");
  check_serialize(std::make_pair('a', 'b'), "ab");
  check_serialize(std::make_tuple('a', 'b', 'c'), "abc");

  // parse
  check_parse("a", std::make_tuple('a'));
  check_parse("ab", std::make_pair('a', 'b'));
  check_parse("abc", std::make_tuple('a', 'b', 'c'));

  // early eof
  check_early_eof<std::pair<char, char>>("");
  check_early_eof<std::pair<char, char>>("a");
  check_early_eof<std::tuple<char, char, char>>("ab");

  // can parse const
  {
    (void)pbss::parse_from_string<const std::pair<char, char>>("ab");
    (void)pbss::parse_from_string<const std::tuple<char, char, char>>("abc");
  }

  // do not consume extra bytes
  check_extra_consume<std::pair<char, char>>("ab");
  check_extra_consume<std::tuple<char, char, char>>("abc");

  return 0;
}
