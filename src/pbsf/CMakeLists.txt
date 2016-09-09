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
#   Xun Chen <xun.gmail.com>

include_directories(${DEPS_LZO_PATH})
include_directories(${DEPS_GIPFELI_PATH})
include_directories(${DEPS_ZSTD_PATH}/lib ${DEPS_ZSTD_PATH}/lib/common)

aux_source_directory(${DEPS_ZSTD_PATH}/lib/common ZSTD_COMMON_FILES)
aux_source_directory(${DEPS_ZSTD_PATH}/lib/compress ZSTD_COMPRESS_FILES)
aux_source_directory(${DEPS_ZSTD_PATH}/lib/decompress ZSTD_DECOMPRESS_FILES)
aux_source_directory(${DEPS_ZSTD_PATH}/lib/dictBuilder ZSTD_DICTBUILDER_FILES)

set(PBSF_SOURCES
  data-block.cc crc-32.cc
  ${DEPS_LZO_PATH}/minilzo.c lzo-wrap.cc
  ${DEPS_GIPFELI_PATH}/decompress.cc ${DEPS_GIPFELI_PATH}/entropy.cc ${DEPS_GIPFELI_PATH}/entropy_code_builder.cc ${DEPS_GIPFELI_PATH}/lz77.cc ${DEPS_GIPFELI_PATH}/gipfeli-internal.cc gipfeli-wrap.cc
  ${ZSTD_COMMON_FILES} ${ZSTD_COMPRESS_FILES} ${ZSTD_DECOMPRESS_FILES} ${ZSTD_DICTBUILDER_FILES} zstd-wrap.cc)

set_source_files_properties(${DEPS_GIPFELI_PATH}/decompress.cc ${DEPS_GIPFELI_PATH}/entropy.cc ${DEPS_GIPFELI_PATH}/entropy_code_builder.cc ${DEPS_GIPFELI_PATH}/lz77.cc ${DEPS_GIPFELI_PATH}/gipfeli-internal.cc gipfeli-wrap.cc PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing -Wno-extra -Wno-conversion -Wno-sign-compare")

set_source_files_properties(zstd-wrap.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion")

add_library(pbsf STATIC ${PBSF_SOURCES})
add_library(pbsf_s SHARED ${PBSF_SOURCES})
set_target_properties(pbsf_s PROPERTIES SOVERSION 2)

install(TARGETS pbsf pbsf_s
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)
