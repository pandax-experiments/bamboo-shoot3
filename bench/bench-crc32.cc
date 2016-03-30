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

#include <string>
#include <algorithm>
#include <chrono>
#include <iostream>

#include <bs3/pbsf/crc-32.hh>

std::chrono::duration<unsigned long long, std::micro>
constexpr operator""_us(unsigned long long x)
{
  return std::chrono::duration<unsigned long long, std::micro>{x};
}

int main()
{

  using pbsf::crc32c_sse;
  using pbsf::crc32c_generic;

  std::chrono::high_resolution_clock clock;

  for (int i_run=0; i_run<16; ++i_run) {

    std::string input((5<<20) + 15, 0);
    {
      char _ = 0;
      for (char& x : input)
        x = ++_;
    }

    if (i_run&1) {
      auto start = clock.now();
      auto crc = crc32c_sse(input.data(), input.size());
      auto dur = clock.now() - start;
      std::cout << std::hex << crc << std::dec
                << " in " << (dur/1_us) << "us by sse, "
                << (double(input.size()) / double(dur/1_us))
                << "MB/s\n";
    } else {
      auto start = clock.now();
      auto crc = crc32c_generic(input.data(), input.size());
      auto dur = clock.now() - start;
      std::cout << std::hex << crc << std::dec
                << " in " << (dur/1_us) << "us by sw, "
                << (double(input.size()) / double(dur/1_us))
                << "MB/s\n";
    }

  };

  return 0;
}
