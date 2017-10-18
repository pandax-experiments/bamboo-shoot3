#
#   Copyright 2016 Carl Lei, Xun Chen
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
include_directories(${DEPS_ZSTD_PATH}/lib ${DEPS_ZSTD_PATH}/lib/common)

aux_source_directory(${DEPS_ZSTD_PATH}/lib/common ZSTD_COMMON_FILES)
aux_source_directory(${DEPS_ZSTD_PATH}/lib/compress ZSTD_COMPRESS_FILES)
aux_source_directory(${DEPS_ZSTD_PATH}/lib/decompress ZSTD_DECOMPRESS_FILES)

set(ZSTD_SOURCES ${ZSTD_COMMON_FILES} ${ZSTD_COMPRESS_FILES} ${ZSTD_DECOMPRESS_FILES})

add_library(zstd OBJECT ${ZSTD_SOURCES})
set_property(TARGET zstd PROPERTY POSITION_INDEPENDENT_CODE 1)

set(PBSF_SOURCES
  data-block.cc crc-32.cc
  ${DEPS_LZO_PATH}/minilzo.c lzo-wrap.cc
  zstd-wrap.cc)

set_source_files_properties(zstd-wrap.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion")

add_library(pbsf STATIC ${PBSF_SOURCES} $<TARGET_OBJECTS:zstd>)
add_library(pbsf_s SHARED ${PBSF_SOURCES} $<TARGET_OBJECTS:zstd>)
set_target_properties(pbsf_s PROPERTIES SOVERSION 3)

install(TARGETS pbsf pbsf_s
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
