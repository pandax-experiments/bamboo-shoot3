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

#include <bs3/utils/range.hh>

#include <cassert>
#include <vector>
#include <sstream>
#include <iterator>

namespace adl {

struct adl_be {
  char* begin;
  char* end;
};

char* begin(adl_be r)
{
  return r.begin;
}

char* end(adl_be r)
{
  return r.end;
}

}

int main()
{

  namespace pu = pbsu;

  {
    // from plain array
    char msg[] = "hello\n";
    auto range = pu::make_range(msg);
    for (char x : range)
      (void)x;
  }

  {
    // from container
    std::vector<char> a;
    auto range = pu::make_range(a);
    for (char x : range)
      (void)x;
  }

  {
    // from pair of istream iterator
    std::istringstream in("foo");
    auto range = pu::make_range(std::istream_iterator<char>(in),
                                std::istream_iterator<char>());
    for (char x : range)
      (void)x;
  }

  {
    // from temporary
    auto f = [](pu::range<typename std::vector<int>::iterator> r) {
      for (int x : r)
        assert(x==1);
    };
    f(pu::make_range(std::vector<int>(5, 1)));
  }

  {
    // from const reference
    std::vector<char> a;
    auto range = pu::make_range(static_cast<const std::vector<char>&>(a));
    for (char x : range)
      (void)x;
  }

  {
    // from types with ADL-defined begin() and end()
    char s[] = "hello";
    for (char x : pu::make_range(adl::adl_be{s, s+5}))
      (void)x;
  }

  {
    // const-qualified begin() and end()
    std::vector<char> a;
    const auto r = pu::make_range(a);
    (void) r.begin();
    (void) r.end();
  }

  // operator bool
  {
    std::vector<char> empty;
    std::vector<char> nonempty{'a'};
    assert(!pu::make_range(empty));
    assert(!!pu::make_range(nonempty));
  }

  // .empty()
  {
    std::vector<char> empty;
    std::vector<char> nonempty{'a'};
    assert(pu::make_range(empty).empty());
    assert(!pu::make_range(nonempty).empty());
  }

  // .size()
  {
    std::vector<int> a;
    assert(pu::make_range(a).size() == 0);
    a = {1, 2, 3};
    assert(pu::make_range(a).size() == 3);
  }

  // operator []
  {
    std::vector<int> a {1, 2, 3};
    assert(pu::make_range(a)[1] == 2);
    // const overload
    const auto range = pu::make_range(a);
    assert(range[1] == 2);
  }

  return 0;
}
