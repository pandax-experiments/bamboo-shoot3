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

#include <bs3/utils/functional.hh>

template <class H, class T>
struct Cons {
  H h;
  T t;

  constexpr bool operator==(const Cons& other) const
  {
    return h==other.h && t==other.t;
  }

};

template <class H, class T>
Cons<H, T>
constexpr cons(const H& h, const T& t)
{
  return { h, t };
}

struct op_cons {
  template <class H, class T>
  Cons<H, T>
  constexpr operator()(const H& h, const T& t) const
  {
    return { h, t };
  }
};

int main()
{

  using pbsu::spread;
  using pbsu::pick;
  using pbsu::call_with;
  using pbsu::collect_result;
  using pbsu::compose;
  using pbsu::reverse_call;
  using pbsu::pipe;

  {
    // spread
    const auto f = spread([](int a, int b) { return a+b; });
    assert(f(std::make_tuple(1, 2)) == 3);
  }

  {
    // spread constexpr
    struct add {
      constexpr int operator()(int a, int b) const { return a+b; }
    };
    static_assert(spread(add())(std::make_tuple(1, 2))==3, "spread can be constexpr");
  }

  {
    // static fold
    {
      // right
      using pbsu::static_foldr;
      constexpr auto list = static_foldr(op_cons(), (uint32_t)1, (float)2, (char)3);
      static_assert(list == cons((uint32_t)1, cons((float)2, (char)3)),
                    "cons list from foldr");
    }
    {
      // left
      using pbsu::static_foldl;
      constexpr auto list = static_foldl(op_cons(), (uint32_t)1, (float)2, (char)3);
      static_assert(list == cons(cons((uint32_t)1, (float)2), (char)3),
                    "cons list from foldl");
    }
  }

  {
    // pick
    const auto f = pick<2, 4>([](int a, int b) { return a+b; });
    assert(f(std::make_tuple(1, 2, 3, 4, 5)) == 8);
  }

  {
    // pick constexpr
    // and defaults to identity
    static_assert(pick<1>()(std::make_tuple(1, 2))==2,
                  "pick can be constexpr");
  }

  {
    // call_with

    {
      // simple
      auto addone = [](int a) { return 1+a; };
      assert(call_with(1)(addone)==2);
      // const qualified
      const auto f = call_with(1);
      assert(f(addone)==2);
    }

    {
      // lvalue reference
      auto addone_lv = [](int& a) { return a+=1; };
      int x = 1;
      call_with(x)(addone_lv);
      assert(x==2);
    }

  }

  {
    // collect_result
    const auto f = collect_result([](int a) { return 1+a; },
                                  [](int a) { return 2*a; });
    assert(f(2)==std::make_tuple(3, 4));
  }

  {
    // "identity"... well really need to test this?
    constexpr auto identity = pbsu::constref_forward();
    assert(identity(42)==42);
    static_assert(identity(42)==42, "identity can be constexpr");
  }

  {
    // compose
    const auto composed = compose(
      [](int x) { return x+1; },
      [](int x) { return x*3; },
      [](int x) { return x-2; });
    assert(composed(5)==10);

    const auto single = compose([](int x) { return x+x; });
    assert(single(3)==6);
  }

  {
    // compose constexpr
    struct addone { constexpr int operator()(int x) const { return x+1; } };
    struct doubled { constexpr int operator()(int x) const { return x+x; } };

    constexpr auto x = compose(addone(), doubled())(2);
    static_assert(x==5, "compose should be constexpr");
  }

  {
    // reverse call
    struct foo {
      constexpr int operator()(int a, int b, int c) const { return a-b-c; }
    };
    static_assert(reverse_call(foo(), 1, 2, 3)==0, "3-2-1==0");
    assert(reverse_call([](int x) { return x+1; }, 1)==2);
  }

  {
    // pipe
    struct addone { constexpr int operator()(int x) const { return x+1; } };
    struct doubled { constexpr int operator()(int x) const { return x+x; } };
    static_assert(pipe(addone(), doubled())(2)==6, "(2+1)*2==6");
  }

  return 0;
}
