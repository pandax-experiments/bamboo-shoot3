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

#include <stdexcept>
#include <algorithm>

#include <minilzo.h>

#include "lzo-wrap.hh"

namespace pbsf {

struct lzo_init_helper {
  lzo_init_helper()
  {
    if (lzo_init() != LZO_E_OK)
      throw std::runtime_error("lzo_init() failed");
  }
} static_lzo_init_object;

constexpr size_t max_compressed_size(size_t size)
{
  return size + size/16 + 64 + 3;
}

std::string lzo_compress(const std::string& src)
{
  lzo_block_size_t input_size = src.size();
  std::string dst(sizeof(lzo_block_size_t)+max_compressed_size(input_size), 0);

  auto sizeptr = reinterpret_cast<const char*>(&input_size);
  std::copy(sizeptr, sizeptr+sizeof(lzo_block_size_t),
            dst.begin());

  lzo_block_size_t out_size {};

  char thread_local workmem alignas(lzo_align_t) [LZO1X_1_MEM_COMPRESS];

  if (LZO_E_OK != lzo1x_1_compress(
        (unsigned char*)&*src.begin(), src.size(),
        (unsigned char*)&dst[sizeof(lzo_block_size_t)], &out_size, workmem))
    throw std::runtime_error("lzo1x_1_compress() failed");

  dst.resize(out_size + sizeof(lzo_block_size_t));

  return dst;
}

std::string lzo_decompress(const std::string& src)
{
  lzo_block_size_t out_size {};
  std::copy(src.begin(), src.begin()+sizeof(lzo_block_size_t),
            reinterpret_cast<char*>(&out_size));

  std::string dst(out_size, 0);
  lzo_block_size_t decompressed_size = out_size;

  auto status = lzo1x_decompress(
    (unsigned char*)&src[sizeof(lzo_block_size_t)], src.size()-sizeof(lzo_block_size_t),
    (unsigned char*)&*dst.begin(), &decompressed_size,
    0);
  if (!(status == LZO_E_OK && decompressed_size == out_size))
    throw std::runtime_error("lzo1x_decompress() failed");

  return dst;
}

} // namespace pbsf
