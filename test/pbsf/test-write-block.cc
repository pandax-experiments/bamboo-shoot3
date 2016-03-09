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

#include <cassert>
#include <bs3/pbsf/pbsf.hh>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int),
                   PBSF_REGISTER_TYPE(4, double));

int main()
{

  std::ostringstream out;

  write_block(out, TestRealm(), (int)44);

  assert(out.str() == (std::string{
         2, 0,                  // Content-Type = 2 (int)
         1, 0,                  // Content-Encoding = 1 (identity)
         (char)0x59, (char)0x0D, (char)0x24, (char)0x12, // Content-Checksum
         4,                     // Content-Length = 4
         44, 0, 0, 0}));

  return 0;
}
