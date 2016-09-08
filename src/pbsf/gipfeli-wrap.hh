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

#ifndef BS3_UTILS_GIPFELI_WRAP_HH
#define BS3_UTILS_GIPFELI_WRAP_HH

#include <bs3/pbss/pbss.hh>

namespace pbsf {

typedef int gipfeli_block_size_t;

pbss::buffer gipfeli_compress(const pbss::buffer&);
pbss::buffer gipfeli_decompress(const pbss::buffer&);

} // namespace pbsf

#endif /* BS3_UTILS_GIPFELI_WRAP_HH */
