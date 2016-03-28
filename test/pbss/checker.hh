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

#ifndef BS3_TEST_CHECKER_HH
#define BS3_TEST_CHECKER_HH

#include <cassert>

#include <sstream>
#include <string>

#include <bs3/pbss/pbss.hh>

template <class T>
void check_serialize(const T& val, std::string res)
{
  assert(pbss::serialize_to_string(val) == res);
  assert(pbss::serialize_to_buffer(val) == pbss::buffer(res.begin(), res.end()));
}

template <class T>
void check_parse(std::string str, const T& result)
{
  assert(pbss::parse_from_string<T>(str) == result);
  assert(pbss::parse_from_buffer<T>({str.begin(), str.end()}) == result);
}

template <class T>
void check_early_eof(std::string str)
{
  std::istringstream in(str);
  try {
    pbss::parse<T>(in);
    assert("Expected early_eof_error but it did not throw" && false);
  } catch(const pbss::early_eof_error&) {
    return;
  }
}

template <class T>
void check_extra_consume(std::string str)
{
  std::istringstream in(str+"pad");
  pbss::parse<T>(in);
  assert("Parse shall not consume extra bytes than needed" && in.rdbuf()->in_avail() == 3);
}

#define check_fixed_size(v, size)                                       \
  do {                                                                  \
    using pbss::fixed_size;                                             \
    static_assert(decltype(fixed_size(v, pbss::adl_ns_tag()))::value == size, "Fixed size must match"); \
    check_static_size(v, size);                                         \
  } while (false)

#define check_static_size(v, size)                                      \
  do {                                                                  \
    using pbss::static_size;                                            \
    static_assert(static_size(v, pbss::adl_ns_tag()) == size, "Static size must match"); \
    check_aot_size(v, size);                                            \
  } while (false)

template <class T>
void check_aot_size(const T& v, std::size_t size)
{
  using pbss::aot_size;
  assert("AOT size must match" && aot_size(v, pbss::adl_ns_tag()) == size);
}

#endif /* BS3_TEST_CHECKER_HH */
