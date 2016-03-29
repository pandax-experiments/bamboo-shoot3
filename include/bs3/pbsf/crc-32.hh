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

#ifndef PBS_PBSF_CRC32_HH
#define PBS_PBSF_CRC32_HH

#include <string>
#include <cstdint>

#include <bs3/pbss/pbss.hh>

namespace pbsf {

uint32_t crc32c(const char*, size_t);
uint32_t crc32c_generic(const char*, size_t);

#ifdef __SSE4_2__

uint32_t crc32c_sse(const char*, size_t);

#endif // __SSE4_2__

inline uint32_t crc32c(const std::string& str)
{
  return crc32c(&*str.begin(), str.size());
}

inline uint32_t crc32c(const pbss::buffer& buf)
{
  return crc32c(reinterpret_cast<const char*>(&*buf.begin()), buf.size());
}

} // namespace pbsf

#endif /* PBS_PBSF_CRC32_HH */
