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

#include <random>
#include <algorithm>
#include <cassert>
#include <bs3/pbsf/pbsf.hh>

int main()
{

  using pbsf::encode_block;
  using pbsf::decode_block;
  using pbsf::EncodedBlock;

  char env_entry[] = "PBSF_COMPRESSION=IDenTiTY";
  putenv(env_entry);

  {
    // always identity even if compressible
    pbss::buffer s(1<<20, 0);
    auto block = encode_block(1, pbss::buffer(s));
    assert("identity encoding is used"
           && block.contentEncoding == PBSF_ENCODING_IDENTITY
           && block.content.size() == s.size());
    assert("decoded block should match original data"
           && decode_block(EncodedBlock(block)) == s);
  }

  return 0;
}
