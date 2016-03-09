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

#include <bs3/utils/misc.hh>

int main()
{

  {
    using pbsu::to_signed;
    using pbsu::to_unsigned;
    // sign conversions
    assert(to_signed((unsigned int)1)==(signed int)1);
    assert(to_unsigned((unsigned int)1)==(signed int)1);
    try {
      (void) to_signed((unsigned int)0xFFFFFFFF);
      assert("out of range error not reported" && false);
    } catch (std::out_of_range&) {
      // pass
    }
    try {
      (void) to_unsigned(-1);
      assert("out of range error not reported" && false);
    } catch (std::out_of_range&) {
      // pass
    }
  }

  return 0;
}
