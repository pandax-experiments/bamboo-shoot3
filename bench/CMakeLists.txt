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

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/test/types)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native")

add_custom_command(OUTPUT hitdata-generated.hh hitdata-gen.hh
  COMMAND ${CMAKE_BINARY_DIR}/util/bsic ${CMAKE_CURRENT_SOURCE_DIR}/hitdata.bsi
  -o hitdata-generated.hh
  -g hitdata-gen.hh
  DEPENDS bsic hitdata.bsi)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(bench-hitdata bench-hitdata.cc
  ${CMAKE_CURRENT_BINARY_DIR}/hitdata-generated.hh
  ${CMAKE_CURRENT_BINARY_DIR}/hitdata-gen.hh)

add_executable(bench-writefile bench-writefile.cc
  ${CMAKE_CURRENT_BINARY_DIR}/hitdata-generated.hh
  ${CMAKE_CURRENT_BINARY_DIR}/hitdata-gen.hh)
target_link_libraries(bench-writefile pbsf)

add_executable(bench-crc32 bench-crc32.cc)
target_link_libraries(bench-crc32 pbsf)
