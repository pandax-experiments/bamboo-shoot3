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

#include <iostream>

#include <bs3/pbs.hh>

#include "defs.hh"

int main(int argc, char *argv[])
{

  namespace pu = pbsu;

  try {

    for (std::string file : pu::make_range(argv+1, argv+argc)) {
      std::cout << file << ":\n";
      copy(open_sequential_input_file(file, ExampleRealm()).read_one_type<int>(),
           std::ostream_iterator<int>(std::cout, "\n"));
      std::cout << '\n';
    }

  } catch(...) {
    throw;
  }

  return 0;
}
