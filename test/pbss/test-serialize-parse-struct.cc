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

#include "checker.hh"

struct simple {
  int16_t a;
  double b;

  bool operator==(const simple& other) const
  {
    return a==other.a && b==other.b;
  }

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &simple::a),
    PBSS_TAG_MEMBER(2, &simple::b));
};

struct has_default {
  char v;
  has_default() : v(42) {}

  bool operator==(const has_default& other) const
  {
    return v == other.v;
  }

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &has_default::v));
};

struct has_non_fixed_member {
  std::string v;

  bool operator==(const has_non_fixed_member& other) const
  {
    return v == other.v;
  }

  PBSS_TAGGED_STRUCT(
    PBSS_TAG_MEMBER(1, &has_non_fixed_member::v));
};

int main()
{

  check_serialize(simple{1, 2}, {
    1, 2, 1, 0,
    2, 8, 0, 0, 0, 0, 0, 0, 0, 0x40,
    0});

  check_parse({
    1, 2, 1, 0,
    2, 8, 0, 0, 0, 0, 0, 0, 0, 0x40,
    0},
    simple{1, 2});

  // can be in reverse order
  check_parse({
    2, 8, 0, 0, 0, 0, 0, 0, 0, 0x40,
    1, 2, 1, 0,
    0},
    simple{1, 2});

  // ignores unrecognized field
  check_parse({
    1, 2, 1, 0,
    3, 4, 0, 0, 0, 0,
    2, 8, 0, 0, 0, 0, 0, 0, 0, 0x40,
    0},
    simple{1, 2});

  // missing fields are same as initialized by default constructor
  check_parse({0}, has_default());

  // sizes of non-fixed size members are treated differently
  check_serialize(has_non_fixed_member{"a"}, {
    1, 2, 1, 'a',
    0});

  check_parse({
    1, 2, 1, 'a',
    0},
  has_non_fixed_member{"a"});

  // early eof
  // simply nothing
  check_early_eof<simple>("");
  // no field size
  check_early_eof<simple>("\x1");
  // var uint size did not finish
  check_early_eof<simple>("\x1\x80");
  // no content
  check_early_eof<simple>("\x1\x2");
  // partial content
  check_early_eof<simple>("\x1\x2\x3");
  // missing end tag
  check_early_eof<simple>("\x1\x2""ab""\x2\x8""12345678");
  // var uint size for fixed size members are optimized, test other cases
  // separately for early eof in var uint
  check_early_eof<has_non_fixed_member>("\x1\x80");

  // extra consume
  check_extra_consume<simple>(std::string("\x1\x2""ab""\x2\x8""12345678") + std::string({0}));

  return 0;
}
