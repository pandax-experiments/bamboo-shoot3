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

#include <string>
#include <sstream>
#include <cassert>

#include <bs3/pbsf/pbsf.hh>

PBSF_DECLARE_REALM(TestRealm, 0x04030201,
                   PBSF_REGISTER_TYPE(1, int));

bool check(const std::string& str)
{
  std::istringstream in(str);
  return pbsf::check_file(in, TestRealm());
}

int main()
{

  assert(check("pbs3\x1\x2\x3\x4"));
  assert("Bad magic" && !check("aaaa\x1\x2\x3\x4"));
  assert("Bad realm" && !check("pbs3\x2\x3\x4\x1"));
  assert("Both bad" && !check("aaaaaaaa"));

  return 0;
}
