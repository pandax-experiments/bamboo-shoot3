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

#include "lzo-wrap.hh"

#include <bs3/pbsf/data-block.hh>

namespace pbsf {

EncodedBlock encode_block(int16_t id, std::string&& raw)
{
  auto compressed = lzo_compress(raw);
  if (compressed.size() > raw.size()) {
    auto crc = crc32c(raw);
    return { id, PBSF_ENCODING_IDENTITY, crc, std::move(raw) };
  } else {
    auto crc = crc32c(compressed);
    return { id, PBSF_ENCODING_LZO, crc, std::move(compressed) };
  }
}

std::string decode_block(EncodedBlock&& block)
{
  if (block.contentChecksum != crc32c(block.content))
    throw bad_checksum_error();
  switch (block.contentEncoding) {
  case PBSF_ENCODING_IDENTITY:
    return std::move(block.content);
  case PBSF_ENCODING_LZO:
    return lzo_decompress(block.content);
  default:
    throw unknown_encoding_error();
  }
}

} // namespace pbsf
