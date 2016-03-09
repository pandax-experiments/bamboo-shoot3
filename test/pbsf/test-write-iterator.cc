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
#include <string>
#include <sstream>
#include <algorithm>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int32_t),
                   PBSF_REGISTER_TYPE(4, double));

#define write(out, v) write_block(out, TestRealm(), (v))

int main()
{

  using s = std::ostringstream;

  s a, b;

  write(a, (int32_t)42);
  write(a, (int32_t)42);
  write(a, (int32_t)42);
  write(a, (double)42);
  write(a, (double)42);

  using w_iter = pbsf::heterogeneous_write_iterator<TestRealm>;

  std::fill_n(w_iter(b), 3, (int32_t)42);
  std::fill_n(w_iter(b), 2, (double)42);

  assert(a.str() == b.str());

  return 0;
}
