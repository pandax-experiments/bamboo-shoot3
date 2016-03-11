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

#include <lz4.h>
#include <lz4hc.h>

#include "lz4-wrap.hh"

namespace pbsf {

std::string lz4_compress(const std::string& src)
{
  if (src.size() > LZ4_MAX_INPUT_SIZE)
    throw std::runtime_error("lz4_compress: block too large for LZ4 compression");
  lz4_block_size_t input_size = static_cast<lz4_block_size_t>(src.size());
  std::string dst(sizeof(lz4_block_size_t)+static_cast<unsigned>(LZ4_compressBound(input_size)), 0);

  auto sizeptr = reinterpret_cast<const char*>(&input_size);
  std::copy(sizeptr, sizeptr+sizeof(lz4_block_size_t),
            dst.begin());

  lz4_block_size_t out_size = LZ4_compress_default(
    src.data(), &*dst.begin()+sizeof(lz4_block_size_t),
    static_cast<int>(src.size()), static_cast<int>(dst.size()));
  dst.resize(static_cast<unsigned>(out_size) + sizeof(lz4_block_size_t));

  return dst;
}

std::string lz4hc_compress(const std::string& src)
{
  if (src.size() > LZ4_MAX_INPUT_SIZE)
    throw std::runtime_error("lz4_compress: block too large for LZ4 compression");
  lz4_block_size_t input_size = static_cast<lz4_block_size_t>(src.size());
  std::string dst(sizeof(lz4_block_size_t)+static_cast<unsigned>(LZ4_compressBound(input_size)), 0);

  auto sizeptr = reinterpret_cast<const char*>(&input_size);
  std::copy(sizeptr, sizeptr+sizeof(lz4_block_size_t),
            dst.begin());

  lz4_block_size_t out_size = LZ4_compress_HC(
    src.data(), &*dst.begin()+sizeof(lz4_block_size_t),
    static_cast<int>(src.size()), static_cast<int>(dst.size()),
    0);
  dst.resize(static_cast<unsigned>(out_size) + sizeof(lz4_block_size_t));

  return dst;
}

std::string lz4_decompress(const std::string& src)
{
  lz4_block_size_t out_size {};
  std::copy(src.begin(), src.begin()+sizeof(lz4_block_size_t),
            reinterpret_cast<char*>(&out_size));

  std::string dst(static_cast<unsigned>(out_size), 0);
  if (LZ4_decompress_fast(src.data()+sizeof(lz4_block_size_t), &*dst.begin(), out_size) < 0)
    throw std::runtime_error("LZ4_decompress_fast detected malformed data");

  return dst;
}

} // namespace pbsf
