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

#ifndef BS3_PBSF_REALM_HH
#define BS3_PBSF_REALM_HH

namespace pbsf {

inline
namespace abiv1 {

template <class... entries>
struct abstract_realm : entries... {};

template <uint32_t id, class... entries>
struct realm : abstract_realm<entries...> {};

template <int16_t id, class ContentType>
struct type_entry {};

} // inline namespace abiv1

template <int16_t id, class T>
T lookup_type(type_entry<id, T>);

template <class T, int16_t id>
constexpr int16_t lookup_id(type_entry<id, T>)
{
  return id;
}

#define PBSF_ABSTRACT_REALM(name, ...) \
  using name = ::pbsf::abstract_realm<__VA_ARGS__>

#define PBSF_DECLARE_REALM(name, id, ...) \
  using name = ::pbsf::realm<id, __VA_ARGS__>

#define PBSF_REGISTER_TYPE(id, type) \
  ::pbsf::type_entry<id, type>

}

#endif /* BS3_PBSF_REALM_HH */
