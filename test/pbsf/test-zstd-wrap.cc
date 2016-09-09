/*

    Copyright 2016 Xun Chen

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

    Xun Chen <xun.revive@gmail.com>

*/

#include <cassert>
#include "zstd-wrap.hh"

int main()
{

  using pbsf::zstd_compress;
  using pbsf::zstd_decompress;

  pbss::buffer raw(2<<20, 'a');
  assert(zstd_decompress(zstd_compress(raw)) == raw);

  return 0;
}
