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

#ifndef BS3_PBSF_DEFS_HH
#define BS3_PBSF_DEFS_HH

#define PBSF_ENCODING_IDENTITY 1
#define PBSF_ENCODING_LZO 2

#include <cstdint>
#include <stdexcept>

#include <bs3/pbss/pbss.hh>

namespace pbsf {

const uint32_t magic = (uint32_t)'p'|((uint32_t)'b'|((uint32_t)'s'|((uint32_t)'3')<<8)<<8)<<8;

class unknown_encoding_error : public std::runtime_error {
public:
  unknown_encoding_error(const char* msg = "Unknown Content-Encoding of data block")
    : std::runtime_error(msg)
  {}
};

class bad_checksum_error : public std::runtime_error {
public:
  bad_checksum_error(const char* msg = "Data checksum mismatch")
    : std::runtime_error(msg)
  {}
};

class unknown_realm_error : public std::runtime_error {
public:
  unknown_realm_error(const char* msg = "Unknown realm")
    : std::runtime_error(msg)
  {}
};

class type_mismatch_error : public std::runtime_error {
public:
  type_mismatch_error(const char* msg = "Type mismatch")
    : std::runtime_error(msg)
  {}
};

class key_mismatch_error : public std::runtime_error {
public:
  key_mismatch_error(const char* msg = "Expected key type does not match")
    : std::runtime_error(msg)
  {}
};

class key_missing_error : public std::out_of_range {
public:
  key_missing_error(const char* msg = "Provided key was not found")
    : std::out_of_range(msg)
  {}
};

inline
namespace abiv1 {

struct FileHeader {
  uint32_t magic;
  uint32_t realm;

  bool operator==(const FileHeader& other) const
  {
    return (magic==other.magic) && (realm==other.realm);
  }

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&FileHeader::magic),
    PBSS_TUPLE_MEMBER(&FileHeader::realm));
};

struct EncodedBlock {
  int16_t contentType;
  int16_t contentEncoding;
  uint32_t contentChecksum;
  std::string content;

  bool operator==(const EncodedBlock& other) const
  {
    return (contentType==other.contentType)
      && (contentEncoding==other.contentEncoding)
      && (contentChecksum==other.contentChecksum)
      && (content==other.content);
  }

  PBSS_TUPLE_MEMBERS(
    PBSS_TUPLE_MEMBER(&EncodedBlock::contentType),
    PBSS_TUPLE_MEMBER(&EncodedBlock::contentEncoding),
    PBSS_TUPLE_MEMBER(&EncodedBlock::contentChecksum),
    PBSS_TUPLE_MEMBER(&EncodedBlock::content));
};

} // inline namespace abiv1

}

#endif /* BS3_PBSF_DEFS_HH */
