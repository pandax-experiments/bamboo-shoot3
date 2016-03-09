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

  check_static_size(pbss::var_uint<size_t>{0}, 1);
  check_static_size(pbss::var_uint<size_t>{1}, 1);
  check_static_size(pbss::var_uint<size_t>{0x7f}, 1);

  check_static_size(pbss::var_uint<size_t>{0x82}, 2);

  check_static_size(pbss::var_uint<size_t>{0xFFFFFFFFFFFFFFFF}, 10);

  pbss::var_uint<size_t> x{0x82};
  check_aot_size(x, 2);

  return 0;
}
