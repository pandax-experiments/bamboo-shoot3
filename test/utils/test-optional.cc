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
#include <bs3/utils/optional.hh>

// utility structs to check usage of special members

// default or nullopt construction
struct disallow_construct {
  disallow_construct() { assert("this must not be constructed" && false); }
};

// only one constructor available
struct provided_ctor {
  provided_ctor() = delete;
  provided_ctor(int, double, char) {}
  provided_ctor(const provided_ctor&) = delete;
  provided_ctor(provided_ctor&&) = delete;
};

// does not use copy
struct barfing_copyct {
  barfing_copyct() = default;
  barfing_copyct(const barfing_copyct&)
  {
    assert("copy ctor must not be used" && false);
  }
};

struct tdible { int a; double b; };
struct ntdible { ~ntdible() {} };

struct check_dt {
  bool* destructed;
  check_dt() : destructed(0) {}
  check_dt(bool& target)
    : destructed(&target)
  {}
  ~check_dt() { if (destructed) *destructed = true; }
};

struct check_const {
  std::true_type is_const() const
  {
    return {};
  }
  std::false_type is_const()
  {
    return {};
  }
};

struct check_move {
  bool moved;
  check_move() : moved(false) {}
  check_move(check_move&& other) : moved(false) { other.moved=true; }

  check_move& operator=(check_move&& other)
  {
    moved = false;
    other.moved = true;
    return *this;
  }
};

struct check_move_ct : check_move {
  check_move_ct() = default;
  check_move_ct(check_move_ct&&) = default;
  check_move_ct& operator=(check_move_ct&&)
  {
    assert("must not move assign" && false);
    return *this;
  }
};

struct check_move_assign : check_move {
  check_move_assign() = default;
  check_move_assign& operator=(check_move_assign&&) = default;
  check_move_assign(check_move_assign&&)
  {
    assert("must not move construct" && false);
  }
};

struct barfing_copy_assign {
  barfing_copy_assign() = default;
  barfing_copy_assign(const barfing_copy_assign&) = default;
  barfing_copy_assign& operator=(const barfing_copy_assign&)
  {
    assert("copy assign operator must not be used" && false);
    return *this;
  }
};

struct check_value_category {

  bool is_lref() & { return true; }
  bool is_lref() const& { return false; }
  bool is_lref() && { return false; }
  bool is_lref() const&& { return false; }

  bool is_const_lref() & { return false; }
  bool is_const_lref() const& { return true; }
  bool is_const_lref() && { return false; }
  bool is_const_lref() const&& { return false; }

  bool is_rref() & { return false; }
  bool is_rref() const& { return false; }
  bool is_rref() && { return true; }
  bool is_rref() const&& { return false; }

  bool is_const_rref() & { return false; }
  bool is_const_rref() const& { return false; }
  bool is_const_rref() && { return false; }
  bool is_const_rref() const&& { return true; }

};

int main()
{

  using pbsu::optional;
  using pbsu::nullopt;
  using pbsu::in_place;

  {
    optional<disallow_construct> a;
    optional<disallow_construct> b { nullopt };

    assert(!a && !b);
  }

  {
    // in_place init
    {
      optional<provided_ctor> a { in_place, 1, 2, 'a' };
      assert(a);
    }

    {
      optional<barfing_copyct> a { in_place };
      assert(a);
    }
  }

  {
    // trivially destructible if contained type is
    static_assert(std::is_trivially_destructible<optional<tdible>>::value,
                  "optional<trivially_destructible> should be trivially destructible");
    optional<tdible> td;
    (void)td;
    static_assert(!std::is_trivially_destructible<optional<ntdible>>::value,
                  "optional<non_trivially_destructible> should not be trivially destructible");
    optional<ntdible> ntd;
    (void)ntd;
  }

  {
    // does call type destructor if has value

    bool destructed = false;
    {
      optional<check_dt> a { in_place, destructed };
    }
    assert(destructed);

    destructed = false;
    {
      optional<check_dt> b;
    }
    assert(!destructed);
  }

  {
    // init by value and copy
    optional<int> a(3);         // by rvalue
    optional<int> b = a;
    const int x = 3;
    optional<int> y { x };      // by const lref
    assert(*a==3 && *b==3 && *y==3);
  }

  {
    // emplace
    {
      // pass-over
      struct foo {
        int a; double b;
        foo(int _a, double _b)
          : a(_a), b(_b)
        {}
      };
      optional<foo> x;
      x.emplace(1, 2);
      assert(x->a==1 && x->b==2);
    }

    {
      // destruct value first if has one
      bool destructed;
      optional<check_dt> x { in_place, destructed };
      x.emplace();
      assert(destructed);
    }

    // TODO value categories preserved --- but too many code...
  }

  {
    // observers for all value categories

    {
      // lref
      optional<check_value_category> a { in_place };
      assert("operator* on lref should return lref" && (*a).is_lref());
      optional<check_value_category> b { in_place };
      assert("operator-> on non-const should not return const" &&
             !b->is_const_lref());
    }

    {
      // const lref
      const optional<check_value_category> a { in_place };
      assert("operator* on const lref should return const lref" && (*a).is_const_lref());
      const optional<check_value_category> b { in_place };
      assert("operator-> on const should return const" &&
             b->is_const_lref());
    }

    {
      // rref
      assert("operator* on rref should return rref" &&
             (*optional<check_value_category>()).is_rref());
    }

    {
      // const rref
#ifndef BS3_DETECTED_GCC_48
      using co = const optional<check_value_category>;
      assert("operator* on const rref should return const rref" &&
             (*(co { {} })).is_const_rref());
#endif
    }
  }

  {
    // move init and assign

    {
      // from value
      check_move src;
      optional<check_move> dest = std::move(src);
      assert(src.moved);
      assert(dest && !dest->moved);
    }

    // construct from optional
    {
      // has value --- move value
      optional<check_move> src { in_place };
      assert(src);
      optional<check_move> dest = std::move(src);
      assert(src && src->moved);
      assert(dest && !dest->moved);
    }

    {
      // no value --- noop
      optional<check_move> src;
      assert(!src);
      optional<check_move> dest = std::move(src);
      assert(!dest && !src);
    }

    // assign from value
    {
      // no value --- move construct
      optional<check_move_ct> dest;
      check_move_ct src;
      dest = std::move(src);
      assert(src.moved && dest && !dest->moved);
    }

    {
      // has value --- move assign
      optional<check_move_assign> dest { in_place };
      check_move_assign src;
      dest = std::move(src);
      assert(src.moved && dest && !dest->moved);
    }

    // assign from optional
    {
      // neither has value --- noop
      optional<check_move> src, dest;
      assert(!dest && !src);
      dest = std::move(src);
      assert(!dest && !src);
    }

    {
      // l has value but r has not --- destruct l
      bool destructed = false;
      optional<check_dt> lhs { in_place, destructed };
      assert(lhs);
      optional<check_dt> rhs;
      assert(!rhs);
      lhs = std::move(rhs);
      assert(destructed && !lhs && !rhs);
    }

    {
      // r has value but l has not --- copy construct
      optional<barfing_copy_assign> lhs;
      optional<barfing_copy_assign> rhs = barfing_copy_assign {};
      lhs = std::move(rhs);
      assert(lhs && rhs);
    }

    {
      // both have value --- copy assign value
      optional<barfing_copyct> lhs { in_place };
      optional<barfing_copyct> rhs { in_place };
      lhs = std::move(rhs);
      assert(lhs && rhs);
    }    
  }

  {
    // copy assignment
    {
      // neither has value --- noop
      optional<int> a, b;
      assert(!a && !b);
      a = b;
      assert(!a && !b);
    }

    {
      // l has value but r has not --- destruct l
      bool destructed = false;
      optional<check_dt> lhs { in_place, destructed };
      assert(lhs);
      optional<check_dt> rhs;
      assert(!rhs);
      lhs = rhs;
      assert(destructed && !lhs && !rhs);
    }

    {
      // r has value but l has not --- copy construct
      optional<barfing_copy_assign> lhs;
      optional<barfing_copy_assign> rhs = barfing_copy_assign {};
      lhs = rhs;
      assert(lhs && rhs);
    }

    {
      // both have value --- copy assign value
      optional<barfing_copyct> lhs { in_place };
      optional<barfing_copyct> rhs { in_place };
      lhs = rhs;
      assert(lhs && rhs);
    }
  }

  // TODO test swap

  {
    // comparisons
    using oi = optional<int>;

    // null == null
    assert(nullopt == oi());
    assert(oi() == nullopt);
    assert(oi() == oi());
    // null != value
    assert(!(nullopt == oi(0)));
    assert(!(oi(0) == nullopt));
    assert(!(oi(0) == oi()));
    assert(!(oi() == oi(0)));
    assert(!(oi() == 0));
    assert(!(0 == oi()));
    assert(nullopt != oi(0));
    assert(oi(0) != nullopt);
    assert(oi(0) != oi());
    assert(oi() != oi(0));
    assert(oi() != 0);
    assert(0 != oi());
    // null <= null
    assert(nullopt <= oi());
    assert(nullopt >= oi());
    assert(oi() <= nullopt);
    assert(oi() >= nullopt);
    assert(oi() <= oi());
    assert(oi() >= oi());
    // null < value
    assert(nullopt < oi(0));
    assert(oi() < oi(0));
    assert(oi() < 0);
    assert(oi(0) > nullopt);
    assert(!(oi() < nullopt));
    assert(!(oi(0) < nullopt));
    assert(!(nullopt > oi()));
    assert(!(nullopt > oi(0)));
    assert(oi(0) > oi());
    assert(0 > oi());
    // null <= value
    assert(nullopt <= oi(0));
    assert(oi() <= oi(0));
    assert(oi() <= 0);
    assert(oi(0) >= nullopt);
    assert(oi(0) >= oi());
    assert(0 >= oi());
    // value op value
    assert(oi(0) == oi(0));
    assert(!(oi(0) == oi(1)));
    assert(oi(0) == 0);
    assert(0 == oi(0));
    assert(!(oi(0) == 1));
    assert(!(1 == oi(0)));
    assert(oi(0) != oi(1));
    assert(!(oi(0) != oi(0)));
    assert(oi(0) != 1);
    assert(1 != oi(0));
    assert(oi(0) < oi(1));
    assert(oi(0) <= oi(1));
    assert(oi(1) > oi(0));
    assert(oi(1) >= oi(0));
    assert(oi(0) < 1);
    assert(0 < oi(1));
    assert(oi(0) <= 1);
    assert(0 <= oi(1));
    assert(oi(1) > 0);
    assert(oi(1) >= 0);
    assert(1 > oi(0));
    assert(1 >= oi(0));
  }

  {
    // do not attempt to destruct contained value in case its constructor
    // throwed
    struct throwing {
      bool should_throw;
      throwing() : should_throw(false) {}
      throwing(int) : should_throw(true) { throw 1; }
      throwing(const throwing&) : should_throw(true) { throw 1; }
      throwing(throwing&&) : should_throw(true) { throw 1; }
      ~throwing() { if (should_throw) throw 1.0; }
    };
    try {
      optional<throwing> a { in_place, 1 };
      assert("did not throw?" && false);
    } catch(int) {
      // pass
    }
    try {
      optional<throwing> src { in_place };
      optional<throwing> a { src };
      assert("did not throw?" && false);
    } catch(int) {
      // pass
    }
    try {
      optional<throwing> src { in_place };
      optional<throwing> a { std::move(src) };
      assert("did not throw?" && false);
    } catch(int) {
      // pass
    }
  }

  // FIXME non-copyable or non-movable types in an optional should make the
  // optional (detected as) non-copyable or non-movable --- refer to
  // libstdc++ enable-special-members

  {
    // make_optional
    // NOTE constexpr constructors are not available in this impl
    auto x = pbsu::make_optional(3);
    assert(*x==3);
  }

  return 0;
}
