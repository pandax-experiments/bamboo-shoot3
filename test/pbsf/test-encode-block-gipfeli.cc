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

#include <random>
#include <algorithm>
#include <cassert>
#include <bs3/pbsf/pbsf.hh>

int main()
{

  using pbsf::encode_block;
  using pbsf::decode_block;
  using pbsf::EncodedBlock;

  // an explicitly put GIPFELI of course also use gipfeli
  char env_entry[] = "PBSF_COMPRESSION=GIPFELI";
  putenv(env_entry);

  {
    // random strings are not compressible
    pbss::buffer s(1<<20);
    std::random_device rd;
    std::mt19937 gen {rd()};
    std::uniform_int_distribution<char> dist;
    std::generate(s.begin(), s.end(), [&]() { return dist(gen); });
    auto block = encode_block(1, pbss::buffer(s));
    assert("uncompressible data use identity encoding"
           && block.contentEncoding == PBSF_ENCODING_IDENTITY);
    assert("decoded block should match original data"
           && decode_block(EncodedBlock(block)) == s);
  }

  {
    // compressible strings are compressed (GIPFELI by default)
    pbss::buffer s(1<<20, 0);
    auto block = encode_block(1, pbss::buffer(s));
    assert("compressible data should be compressed"
           && block.contentEncoding == PBSF_ENCODING_GIPFELI
           && block.content.size() < s.size());
    assert("decoded block should match original data"
           && decode_block(EncodedBlock(block)) == s);
  }

  return 0;
}
