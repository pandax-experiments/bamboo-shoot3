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

#include <bs3/pbss/pbss.hh>

struct has_fixed {};
std::integral_constant<size_t, 1> fixed_size(const has_fixed&, pbss::adl_ns_tag);

struct has_static {};
constexpr size_t static_size(const has_static&, pbss::adl_ns_tag) { return 1; }

struct has_aot {};
size_t aot_size(const has_aot&, pbss::adl_ns_tag) { return 1; }

int main()
{

  using pbss::has_no_fixed_size;

  static_assert(!decltype(has_no_fixed_size(has_fixed()))::value,
                "This type actually has fixed_size");
  static_assert(decltype(has_no_fixed_size(has_static()))::value,
                "This type has no fixed size");
  static_assert(decltype(has_no_fixed_size(has_aot()))::value,
                "This type has no fixed size");

  return 0;
}
