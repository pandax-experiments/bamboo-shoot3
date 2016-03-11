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

include_directories(${DEPS_LZO_PATH})
include_directories(${DEPS_LZ4_PATH}/lib)

set(PBSF_SOURCES
  data-block.cc crc-32.cc
  ${DEPS_LZO_PATH}/minilzo.c lzo-wrap.cc
  ${DEPS_LZ4_PATH}/lib/lz4.c ${DEPS_LZ4_PATH}/lib/lz4hc.c lz4-wrap.cc)
add_library(pbsf STATIC ${PBSF_SOURCES})
add_library(pbsf_s SHARED ${PBSF_SOURCES})
set_target_properties(pbsf_s PROPERTIES SOVERSION 1)

install(TARGETS pbsf pbsf_s
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
