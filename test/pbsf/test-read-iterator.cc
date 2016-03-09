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
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int32_t),
                   PBSF_REGISTER_TYPE(4, double));

template <class T>
std::vector<T> read_all(std::istream& stream)
{
  using iter = pbsf::skipping_read_iterator<TestRealm, T>;
  return std::vector<T>(iter(stream), iter());
}

template <class T>
std::vector<T> read_all_from_string(const std::string& str)
{
  std::istringstream in(str);
  return read_all<T>(in);
}

template <class T>
void expect(const std::string& str, const std::vector<T>& result)
{
  assert(read_all_from_string<T>(str) == result);
}

#define write(v) write_block(out, TestRealm(), (v))

int main()
{

  using s = std::ostringstream;

  {
    // empty
    assert(read_all_from_string<int32_t>("") == std::vector<int32_t>());
  }

  {
    // all unknown
    s out;
    for (double x : {1, 2, 3})
      write(x);
    expect<int32_t>(out.str(), {});
  }

  {
    // singleton
    s out;
    write((int32_t)42);
    expect<int32_t>(out.str(), {42});
  }

  {
    // multiple
    s out;
    for (int32_t x : {4, 5, 6})
      write(x);
    expect<int32_t>(out.str(), {4, 5, 6});
  }

  {
    // some unknown
    s out;
    write((double)0);
    write((int32_t)42);
    write((int32_t)43);
    write((double)1);
    write((double)2);
    write((int32_t)44);
    write((double)3);
    expect<int32_t>(out.str(), {42, 43, 44});
  }

  {
    // checksum mismatch
    s out;
    write((int32_t)-1);
    auto good = out.str();
    auto bad = good.substr(0, good.size()-4) + std::string({0, 0, 0, 0});
    try {
      read_all_from_string<int32_t>(bad);
      assert("Checksum error not reported" && false);
    } catch (pbsf::bad_checksum_error&) {
      // test pass, do nothing
    }
  }

  {
    // unknown encoding
    s out;
    write((int32_t)0);
    auto bad = out.str();
    bad.replace(2, 2, "aa");
    try {
      read_all_from_string<int32_t>(bad);
      assert("Unknown encoding error not reported" && false);
    } catch (pbsf::unknown_encoding_error&) {
      // test pass, do nothing
    }
  }

  return 0;
}
