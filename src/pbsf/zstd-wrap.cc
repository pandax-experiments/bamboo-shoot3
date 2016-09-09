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

#include <stdexcept>
#include <algorithm>

#include <zstd.h>

#include "zstd-wrap.hh"

namespace pbsf {

pbss::buffer zstd_compress(const pbss::buffer& src)
{
  zstd_block_size_t input_size = static_cast<zstd_block_size_t>(src.size());
  // dst start with a uncompressed block size
  pbss::buffer dst(sizeof(zstd_block_size_t) + static_cast<unsigned>(ZSTD_compressBound(input_size)));
  // copy the size into output buffer
  auto sizeptr = reinterpret_cast<const char*>(&input_size);
  std::copy(sizeptr, sizeptr+sizeof(zstd_block_size_t),
            dst.begin());
  zstd_block_size_t out_size = ZSTD_compress(
    (void *)((char *)&*dst.begin()+sizeof(zstd_block_size_t)),
    dst.size() - sizeof(zstd_block_size_t),
    (const void *)src.data(), src.size(), 3);
  dst.resize(static_cast<unsigned>(out_size) + sizeof(zstd_block_size_t));
  return dst;
}

pbss::buffer zstd_decompress(const pbss::buffer& src)
{
  zstd_block_size_t out_size {};
  std::copy(src.begin(), src.begin()+sizeof(zstd_block_size_t),
            reinterpret_cast<char*>(&out_size));

  pbss::buffer dst(static_cast<unsigned>(out_size));

  if (ZSTD_decompress(
        (void*)&*dst.begin(), (size_t) out_size,
        (const void *)((const char *)src.data() + sizeof(zstd_block_size_t)),
        src.size() - sizeof(zstd_block_size_t))
      !=0)
    throw std::runtime_error("Zstd decompress detected malformed data");
  return dst;
}

} // namespace pbsf
