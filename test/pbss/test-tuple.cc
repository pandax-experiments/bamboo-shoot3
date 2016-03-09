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

struct simple {
  char a;
  int b;
  bool operator==(const simple& other) const
  {
    return a==other.a && b==other.b;
  }

  PBSS_TUPLE_MEMBERS(PBSS_TUPLE_MEMBER(&simple::a), PBSS_TUPLE_MEMBER(&simple::b));
};

int main()
{

  check_serialize(simple{'a', 0x04030201}, "a\x1\x2\x3\x4");

  check_parse("a\x1\x2\x3\x4", simple{'a', 0x04030201});

  // early eof
  // nothing
  check_early_eof<simple>("");
  // missing second field
  check_early_eof<simple>("a");
  // incomplete second field
  check_early_eof<simple>("a\x3\x4");

  // extra consume
  check_extra_consume<simple>(std::string("a\x1\x2\x3\x4"));

  return 0;
}
