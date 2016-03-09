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
#include <string>
#include <vector>

#include <bs3/pbss/pbss.hh>

template <class T>
void check_all(std::string str, std::vector<T> result)
{
  pbss::char_range_reader reader(&*str.begin(), &*str.begin() + str.size());
  auto all = pbss::parse_all<T>(reader);
  assert(std::vector<T>(all.begin(), all.end()) == result);
}

int main()
{

  check_all<char>("abcde", {'a', 'b', 'c', 'd', 'e'});

  return 0;
}
