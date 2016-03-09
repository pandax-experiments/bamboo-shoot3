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
#include <utility>

#include <iostream>

#include <bs3/pbsf/pbsf.hh>
#include <bs3/utils/range.hh>
#include <bs3/utils/range-util.hh>

PBSF_DECLARE_REALM(TestRealm, 42,
                   PBSF_REGISTER_TYPE(2, int),
                   PBSF_REGISTER_TYPE(4, double));

PBSF_DECLARE_REALM(WrongRealm, 43,
                   PBSF_REGISTER_TYPE(2, int),
                   PBSF_REGISTER_TYPE(4, double));

int main()
{

  const std::string filename = "test-indexed-output-file-artifact.bs";

  using pbsf::open_indexed_output_file;
  using pbsf::open_indexed_input_file;
  namespace pu = pbsu;

  {
    {
      // fill with simple data
      auto f = open_indexed_output_file<int>(filename, TestRealm(), true);
      auto oit = f.write_iterator();
      *oit++ = std::make_pair(2, 4.5);
      *oit++ = std::make_pair(3, 5.4);
      assert(f.size() == 2);
    }

    {
      // read back
      auto f = open_indexed_input_file<int>(filename, TestRealm());
      assert(f.size() == 2);

      {
        std::vector<std::pair<int, double> > v;
        for (auto p : f)
          v.emplace_back(p.first, p.second->as<double>());
        assert((v == std::vector<std::pair<int, double> > {{2, 4.5}, {3, 5.4}}));
      }

      {
        std::vector<std::pair<int, double> > v;
        for (auto p : pu::make_range(f.rbegin(), f.rend()))
          v.emplace_back(p.first, p.second->as<double>());
        assert((v == std::vector<std::pair<int, double> > {{3, 5.4}, {2, 4.5}}));
      }

      assert(f[2]->as<double>() == 4.5);
    }

    {
      // output file is readable
      auto f = open_indexed_output_file<int>(filename, TestRealm(), false);
      assert(f[2]->as<double>() == 4.5);
    }

    {
      // a second insert replaces previous one
      auto f = open_indexed_output_file<int>(filename, TestRealm(), false);
      f.insert(std::make_pair(2, 5.6));
      assert(f[2]->as<double>() == 5.6);
      assert(f.size() == 2);
    }

    {
      // key_missing_error
      auto f = open_indexed_input_file<int>(filename, TestRealm());
      try {
        (void)f[4];
        assert("key missing error not reported" && false);
      } catch (pbsf::key_missing_error&) {
        // pass
      }
    }

    {
      // key_mismatch_error
      try {
        auto f = open_indexed_input_file<double>(filename, TestRealm());
        assert("key mismatch error not reported" && false);
      } catch (pbsf::key_mismatch_error&) {
        // pass
      }
    }

    {
      // type_mismatch_error
      auto f = open_indexed_input_file<int>(filename, TestRealm());
      try {
        (void) f[2]->as<int>();
        assert("type mismatch error not reported" && false);
      } catch (pbsf::type_mismatch_error&) {
        // pass
      }
    }

    {
      // unknown_realm_error
      try {
        auto f = open_indexed_input_file<int>(filename, WrongRealm());
        assert("unknown realm error not reported" && false);
      } catch (pbsf::unknown_realm_error&) {
        // pass
      }

      try {
        auto f = open_indexed_output_file<int>(filename, WrongRealm(), false);
        assert("unknown realm error not reported" && false);
      } catch (pbsf::unknown_realm_error&) {
        // pass
      }
    }

    {
      // default to truncate existing file
      auto f = open_indexed_output_file<int>(filename, TestRealm());
      assert(f.find(2) == f.end());
      assert(f.size() == 0);
    }

    {
      // lower/upper_bound
      using T = std::vector<std::pair<int, double> >;
      T vals { {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6} };
      auto f = open_indexed_output_file<int>(filename, TestRealm());
      pu::copy(vals, f.write_iterator());
      T subset { {2, 3}, {3, 4}, {4, 5} };
      T read_subset;
      for (auto p : pu::make_range(f.lower_bound(2), f.upper_bound(4)))
        read_subset.emplace_back(p.first, p.second->as<double>());
      assert(subset == read_subset);
    }
  }

  return 0;
}
