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

#include <gipfeli.h>

#include "gipfeli-wrap.hh"

namespace pbsf {

pbss::buffer gipfeli_compress(const pbss::buffer& src)
{
  gipfeli_block_size_t input_size = static_cast<gipfeli_block_size_t>(src.size());
  namespace uc = util::compression;
  uc::Compressor* compressor =
      uc::NewGipfeliCompressor();
  // dst start with a uncompressed block size
  pbss::buffer dst(sizeof(gipfeli_block_size_t) + static_cast<unsigned>(compressor->MaxCompressedLength(input_size)));
  // copy the size into output buffer
  auto sizeptr = reinterpret_cast<const char*>(&input_size);
  std::copy(sizeptr, sizeptr+sizeof(gipfeli_block_size_t),
            dst.begin());
  uc::ByteArraySource c_src((const char *)src.data(), src.size());
  uc::UncheckedByteArraySink c_sink((char *)&*dst.begin()+sizeof(gipfeli_block_size_t));
  gipfeli_block_size_t out_size = compressor->CompressStream(
    &c_src, &c_sink);
  dst.resize(static_cast<unsigned>(out_size) + sizeof(gipfeli_block_size_t));
  delete compressor;
  return dst;
}

pbss::buffer gipfeli_decompress(const pbss::buffer& src)
{
  gipfeli_block_size_t out_size {};
  std::copy(src.begin(), src.begin()+sizeof(gipfeli_block_size_t),
            reinterpret_cast<char*>(&out_size));

  pbss::buffer dst(static_cast<unsigned>(out_size));

  namespace uc = util::compression;
  uc::Compressor* compressor =
      uc::NewGipfeliCompressor();
  uc::ByteArraySource c_src((const char *)((char*)src.data()+sizeof(gipfeli_block_size_t)), src.size()-sizeof(gipfeli_block_size_t));
  uc::UncheckedByteArraySink c_sink((char *)&*dst.begin());
  if (!compressor->UncompressStream(
        &c_src, &c_sink))
    throw std::runtime_error("GIPFeli UncompressStream detected malformed data");
  delete compressor;
  return dst;
}

} // namespace pbsf
