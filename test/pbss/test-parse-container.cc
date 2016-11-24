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
#include <set>
#include <list>

int main()
{

  // into vector
  check_parse("\x3""abc", std::vector<char>{'a', 'b', 'c'});
  check_parse({0}, std::vector<char>());

  // into array
  check_parse("\x3""abc", std::array<char, 3>{{'a', 'b', 'c'}});

  // into string
  check_parse("\x3""abc", std::string("abc"));
  check_parse({0}, std::string());

  // into set
  check_parse("\x5""cdaeb", std::set<char>{'a', 'b', 'c', 'd', 'e'});
  check_parse({0}, std::set<char>());

  // into list
  check_parse("\x3""abc", std::list<char>{'a', 'b', 'c'});
  check_parse({0}, std::list<char>());

  // 2-byte length
  // 0x82 == 0x80 + 2, 0x80 goes to next byte as 1, and 2|0x80 == 0x82
  check_parse(std::string("\x82\x1")+std::string(0x82, 0),
              std::vector<char>(0x82, 0));

  // early eof error
  // no length at all
  check_early_eof<std::vector<char>>("");
  // not enough elements
  check_early_eof<std::vector<char>>("\x2""a");

  // can parse const
  {
    std::istringstream in("\x1\x2");
    pbss::parse<const std::vector<char>>(in);
  }
  {
    std::istringstream in("\x1\x2");
    pbss::parse<const std::set<char>>(in);
  }

  // do not consume extra bytes
  check_extra_consume<std::vector<char>>("\x2""ab");
  // also for zero length
  check_extra_consume<std::vector<char>>({0});

  return 0;
}
