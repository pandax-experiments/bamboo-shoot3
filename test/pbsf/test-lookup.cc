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

#include <type_traits>
#include <bs3/pbsf/pbsf.hh>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int),
                   PBSF_REGISTER_TYPE(4, double));

struct abs1 {};
struct abs2 {};

PBSF_ABSTRACT_REALM(Abstract,
                    PBSF_REGISTER_TYPE(-9, abs1),
                    PBSF_REGISTER_TYPE(-33, abs2));

PBSF_DECLARE_REALM(ComposedRealm, 24,
                   TestRealm, Abstract,
                   PBSF_REGISTER_TYPE(7, float));

#define check_assoc(realm, id, type)                                    \
  do {                                                                  \
    using pbsf::lookup_id;                                              \
    using pbsf::lookup_type;                                            \
    static_assert(lookup_id<type>(realm()) == id, "lookup by type failed"); \
    static_assert(std::is_same<type, decltype(lookup_type<id>(realm()))>::value, \
                  "lookup by id failed");                               \
  } while (false)

int main()
{

  check_assoc(TestRealm, 2, int);
  check_assoc(TestRealm, 4, double);

  check_assoc(Abstract, -9, abs1);
  check_assoc(Abstract, -33, abs2);

  check_assoc(ComposedRealm, 2, int);
  check_assoc(ComposedRealm, 4, double);
  check_assoc(ComposedRealm, -9, abs1);
  check_assoc(ComposedRealm, -33, abs2);
  check_assoc(ComposedRealm, 7, float);

  return 0;
}
