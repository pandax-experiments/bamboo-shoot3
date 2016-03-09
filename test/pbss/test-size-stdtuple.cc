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

namespace adl {

struct has_fixed {};

std::integral_constant<std::size_t, 3> fixed_size(const has_fixed&, pbss::adl_ns_tag);

struct has_static {};

constexpr std::size_t static_size(const has_static&, pbss::adl_ns_tag)
{
  return 2;
}

struct has_aot {};

std::size_t aot_size(const has_aot&, pbss::adl_ns_tag)
{
  return 1;
}

}

int main()
{

  check_fixed_size(std::make_pair('a', (uint16_t)0), 3);
  check_fixed_size(std::make_pair('a', adl::has_fixed()), 4);

  check_static_size(std::make_pair('a', adl::has_static()), 3);

  check_aot_size(std::make_pair('a', adl::has_aot()), 2);

  check_static_size(std::make_tuple(adl::has_fixed(), adl::has_static()), 5);

  return 0;
}
