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
#include <fstream>

#include <bs3/pbsf/pbsf.hh>

#include <iostream>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int),
                   PBSF_REGISTER_TYPE(4, double));

int main()
{

  const char* filename = "test-range-api-fixture";
  using of = std::ofstream;

  {
    // open_sequential_input_file on valid file
    {
      of out(filename);
      out << std::string {'p', 'b', 's', '3', 42, 0, 0, 0};
    }
    pbsf::open_sequential_input_file(filename, TestRealm());
  }

  {
    // open_sequential_input_file on invalid file throws
    {
      of out(filename);
      // an empty one
    }
    try {
      pbsf::open_sequential_input_file(filename, TestRealm());
      assert("invalid file not reported" && false);
    } catch (...) {
      // Some exception must be thrown; no idea which one it may be
      // however.  But assert() does not throw, it abort()s.
    }
  }

  {
    // open_sequential_input_file reports unknown realm
    {
      of out(filename);
      out << std::string {'p', 'b', 's', '3', 44, 0, 0, 0};
    }
    try {
      pbsf::open_sequential_input_file(filename, TestRealm());
      assert("unknown realm error not reported" && false);
    } catch (const pbsf::unknown_realm_error&) {
      // good
    }
  }

  {
    // open_sequential_input_file failing to open
    try {
      pbsf::open_sequential_input_file("test-range-api-file-should-not-exist", TestRealm());
      assert("failure to open not reported" && true);
    } catch (...) {
      // good
    }
  }

  {
    // read_one_type<Tp> returns range containing Tp
    {
      of out(filename);
      write_header(out, TestRealm());
      write_block(out, TestRealm(), (int)42);
      write_block(out, TestRealm(), (int)42);
      write_block(out, TestRealm(), (int)42);
    }
    size_t n = 0;
    auto f = pbsf::open_sequential_input_file(filename, TestRealm());
    for (int x : pbsf::read_one_type<int>(f))
      ++n, assert(x == 42);
    assert(n == 3);
  }

  {
    // open_sequential_output_file creates a file that can be read back by
    // open_sequential_input_file
    pbsf::open_sequential_output_file(filename, TestRealm());
    pbsf::open_sequential_input_file(filename, TestRealm());
  }

  {
    // write_iterator simply wraps over iterator construction
    {
      auto f = pbsf::open_sequential_output_file(filename, TestRealm());
      std::fill_n(write_iterator(f), 3, (int)42);
    }
    size_t n = 0;
    auto f = pbsf::open_sequential_input_file(filename, TestRealm());
    for (int x : pbsf::read_one_type<int>(f))
      ++n, assert(x == 42);
    assert(n == 3);
  }

  return 0;
}
