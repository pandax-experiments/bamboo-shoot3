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

set(PBSF_SOURCES data-block.cc crc-32.cc lzo-wrap.cc zstd-wrap.cc)

set_source_files_properties(zstd-wrap.cc PROPERTIES COMPILE_FLAGS "-Wno-error=conversion -Wno-conversion")

add_library(pbsf STATIC ${PBSF_SOURCES})
add_library(pbsf_s SHARED ${PBSF_SOURCES})
set_target_properties(pbsf_s PROPERTIES SOVERSION 3)
target_link_libraries(pbsf PRIVATE pbsfdep-zstd pbsfdep-lzo)
target_link_libraries(pbsf_s PRIVATE pbsfdep-zstd pbsfdep-lzo)

install(TARGETS pbsf pbsf_s
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
