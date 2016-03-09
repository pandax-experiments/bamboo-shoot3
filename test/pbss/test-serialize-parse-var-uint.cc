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

  pbss::var_uint<size_t> n;

  // 1 byte
  n.v = 0;
  check_serialize(n, {0});
  check_parse({0}, n);
  n.v = 0x7f;
  check_serialize(n, "\x7f");
  check_parse("\x7f", n);

  // 2 byte
  n.v = 0x82;
  check_serialize(n, "\x82\x1");
  check_parse("\x82\x1", n);

  // a random one
  n.v = 11071323323165186345ull;
  check_serialize(n, {(char)169,(char)178,(char)136,(char)246,(char)246,(char)188,(char)207,(char)210,(char)153,(char)1});
  check_parse({(char)169,(char)178,(char)136,(char)246,(char)246,(char)188,(char)207,(char)210,(char)153,(char)1}, n);

  // max of 10 byte
  n.v = 0xFFFFFFFFFFFFFFFF;
  check_serialize(n, "\xff\xff\xff\xff\xff\xff\xff\xff\xff\x1");
  check_parse("\xff\xff\xff\xff\xff\xff\xff\xff\xff\x1", n);

  // early eof
  check_early_eof<pbss::var_uint<size_t>>("");
  check_early_eof<pbss::var_uint<size_t>>("\x80");

  // extra consume
  check_extra_consume<pbss::var_uint<size_t>>({0});
  check_extra_consume<pbss::var_uint<size_t>>("\x80\x1");

  return 0;
}
