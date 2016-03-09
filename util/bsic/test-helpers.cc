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

#include <sstream>
#include <cassert>

#define BSIC_TEST_TYPE(type)                                            \
  do {                                                                  \
    auto value = bsic_rgen(bsic_rgen_tag<type>{});                      \
    std::ostringstream out;                                             \
    pbss::serialize(out, value);                                        \
    std::istringstream in(out.str());                                   \
    assert(value == pbss::parse<type>(in));                             \
  } while (false)
