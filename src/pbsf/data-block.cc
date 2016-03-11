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

#include <cstdlib>
#include <cctype>

#include "lzo-wrap.hh"
#include "lz4-wrap.hh"

#include <bs3/pbsf/data-block.hh>

namespace pbsf {

int16_t env_preferred_encoding()
{
  static auto choice = ([]() -> int16_t {
    auto ppref = getenv("PBSF_COMPRESSION");
    if (!ppref) return PBSF_ENCODING_LZ4;

    std::string pref(ppref);
    for (char& ch : pref)
      ch = static_cast<char>(tolower(ch));

    if (pref == "identity") return PBSF_ENCODING_IDENTITY;
    if (pref == "lzo") return PBSF_ENCODING_LZO;
    if (pref == "lz4") return PBSF_ENCODING_LZ4;
    return PBSF_ENCODING_LZ4;
  })();

  return choice;
}

EncodedBlock encode_block(int16_t id, std::string&& raw, int16_t encoding)
{
  switch (encoding) {

  case PBSF_ENCODING_IDENTITY: {
    auto crc = crc32c(raw);
    return { id, PBSF_ENCODING_IDENTITY, crc, std::move(raw) };
  }

  case PBSF_ENCODING_LZO: {
    auto compressed = lzo_compress(raw);
    if (compressed.size() > raw.size()) {
      auto crc = crc32c(raw);
      return { id, PBSF_ENCODING_IDENTITY, crc, std::move(raw) };
    } else {
      auto crc = crc32c(compressed);
      return { id, PBSF_ENCODING_LZO, crc, std::move(compressed) };
    }
  }

  case PBSF_ENCODING_LZ4: {
    auto compressed = lz4_compress(raw);
    if (compressed.size() > raw.size()) {
      auto crc = crc32c(raw);
      return { id, PBSF_ENCODING_IDENTITY, crc, std::move(raw) };
    } else {
      auto crc = crc32c(compressed);
      return { id, PBSF_ENCODING_LZ4, crc, std::move(compressed) };
    }
  }

  default:
    return encode_block(id, std::move(raw), PBSF_ENCODING_LZ4);

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
  case PBSF_ENCODING_LZ4:
    return lz4_decompress(block.content);
  default:
    throw unknown_encoding_error();
  }
}

} // namespace pbsf
