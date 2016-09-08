#
#   Copyright 2016 Carl Lei
#
#   This file is part of Bamboo Shoot 3.
#
#   Bamboo Shoot 3 is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   Bamboo Shoot 3 is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with Bamboo Shoot 3.  If not, see <http://www.gnu.org/licenses/>.
#
#   Carl Lei <xecycle@gmail.com>

include_directories(${BambooShoot3_SOURCE_DIR}/src/pbsf)
link_libraries(pbsf)

pbs_deftest(test-lzo-wrap)
pbs_deftest(test-lz4-wrap)
pbs_deftest(test-gipfeli-wrap)

pbs_deftest(test-crc32)
pbs_deftest(test-valid-file)
pbs_deftest(test-write-header)
pbs_deftest(test-write-block)
pbs_deftest(test-read-iterator)
pbs_deftest(test-write-iterator)
pbs_deftest(test-encode-block-default)
pbs_deftest(test-encode-block-lzo)
pbs_deftest(test-encode-block-lz4)
pbs_deftest(test-encode-block-lz4hc)
pbs_deftest(test-encode-block-identity)
pbs_deftest(test-encode-block-unknown)

pbs_deftest(test-lookup)

pbs_deftest(test-range-api)
pbs_deftest(test-indexed)
