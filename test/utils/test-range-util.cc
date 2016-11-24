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
#include <sstream>
#include <iterator>
#include <vector>

#include <bs3/utils/range-util.hh>

int main()
{

  using pbsu::copy;

  {
    // copy
    const char* pieces[] = {"foo", "bar", "baz"};
    std::ostringstream out;
    copy(pieces, std::ostream_iterator<const char*>(out, "\n"));
    assert(out.str() == "foo\nbar\nbaz\n");
  }

  {
    // zip
    using pbsu::zip;
    int a[] = {1, 2, 3};
    int b[] = {4, 3, 2, 1};

    auto r = zip(a, b);

    {
      // iterator operations

      auto x = r.begin();
      auto y = x;
      ++y;

      static_assert(std::is_same<
                      typename decltype(x)::iterator_category,
                      std::random_access_iterator_tag
                    >::value,
                    "most common iterator tag should be random");

      // comparison
      assert(x!=y);
      assert(!(x==y));
      assert(x<y);
      assert(x<=y);
      assert(!(x>y));
      assert(!(x>=y));

      --y;
      assert(x==y);

      y++, y--;
      assert(x==y);

      y+=2;
      assert(x<y);
      y-=2;
      assert(x==y);

      assert(x<y+1);
      assert(x<1+y);

      ++y;
      assert(x==y-1);
      assert(x-(-1)==y);

      assert(y-x==1);

      (y+=1)-=2;
      assert(x==y);

      {
        const auto replace = std::make_tuple(3, 4);
        x->operator=(replace);
        assert(a[0]==3 && b[0]==4);
      }

      {
        const auto replace = std::make_tuple(3, 4);
        x[0] = replace;
        assert(a[0]==3 && b[0]==4);
      }

    } // end iterator operations

    {
      // range as a whole

      size_t i=0;
      for (auto tup : r)
        assert(std::get<0>(tup)==a[i] && std::get<1>(tup)==b[i]), ++i;
      assert(i==3);
    }

    {
      // should be happy with input iterator ranges
      using pbsu::make_range;
      std::istringstream a("1 2 3 4"), b("4 5 6");
      auto ra = make_range(std::istream_iterator<int>(a), std::istream_iterator<int>());
      auto rb = make_range(std::istream_iterator<int>(b), std::istream_iterator<int>());
      int ia=0, ib=3;
      for (auto pack : zip(ra, rb))
        assert(std::get<0>(pack)==++ia && std::get<1>(pack)==++ib);
      assert(ia==3 && ib==6);
    }

    {
      // starts empty
      std::vector<int> a;
      std::vector<int> b {1, 2, 3};
      for (auto pack : zip(a, b))
        (void)pack, assert("range should be empty" && false);
    }

  } // end zip

  {
    // map
    using pbsu::map;
    auto addone = [](int x) { return 1+x; };
    int src[] = { 1, 2, 3 };
    auto mapped = map(addone, src);
    assert((std::vector<int>(mapped.begin(), mapped.end()) == std::vector<int> { 2, 3, 4 }));

    // FIXME test map in other flavors --- but that just duplicates test
    // code of mapping iterators
  }

  {
    // filter
    using pbsu::filter;
    auto odd = [](int x) -> bool { return x&1; };
    int arr[] = { 1, 2, 3, 4, 5 };
    auto odds = filter(odd, arr);
    assert((std::vector<int>(odds.begin(), odds.end()) == std::vector<int> { 1, 3, 5 }));
  }

  {
    // reduce
    using pbsu::reduce;

    std::vector<int> empty;
    int single[] = { 1 };
    int multi[] = { 1, 2, 3 };

    auto add = [](int a, int b) { return a+b; };

    {
      // with init
      assert(reduce(add, 42, empty)==42);
      assert(reduce(add, 42, single)==43);
      assert(reduce(add, 42, multi)==48);
    }

    {
      // no init
      try {
        (void) reduce(add, empty);
        assert("out_of_range not thrown" && false);
      } catch (const std::out_of_range&) {
        // pass
      }
      assert(reduce(add, single)==1);
      assert(reduce(add, multi)==6);
    }

    {
      // prvalue
      assert(reduce(add, std::vector<int>{1, 2, 3})==6);
    }

  }

  {
    // map_indexd
    using pbsu::map_indexed;

    {
      int a[] = {1, 2, 4};
      auto r = map_indexed([](int x, size_t i) { return x*int(i); }, a);
      assert(std::vector<size_t>(r.begin(), r.end()) == (std::vector<size_t> {0, 2, 8}));
    }
  }

  return 0;
}
