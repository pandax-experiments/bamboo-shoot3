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

cmake_minimum_required(VERSION 2.6)

project(BambooShoot3)

set(BambooShoot3_VERSION "r8")
set(PROJECT_VERSION ${BambooShoot3_VERSION})
set(BambooShoot3_VERSION_MAJOR ${BambooShoot3_VERSION})
set(PROJECT_VERSION_MAJOR ${BambooShoot3_VERSION})

set(BambooShoot3_BUILD_CXXSTD_FLAGS "-std=c++14"
  CACHE STRING "Flag for specifying language standard")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${BambooShoot3_BUILD_CXXSTD_FLAGS} -Wpedantic -Wall -Wextra -Wconversion -Werror")

# external deps
set(DEPS_LZO_PATH ${PROJECT_SOURCE_DIR}/deps/lzo)
set(DEPS_GIPFELI_PATH ${PROJECT_SOURCE_DIR}/deps/gipfeli)
set(DEPS_ZSTD_PATH ${PROJECT_SOURCE_DIR}/deps/zstd)

# tests
set(CMAKE_CXX_FLAGS_TESTING "-msse4.2 -g -fstrict-aliasing"
  CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_TESTING "-msse4.2 -g"
  CACHE STRING "" FORCE)

# native optimized build
set(CMAKE_CXX_FLAGS_NATIVE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -mtune=native"
  CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_NATIVE "${CMAKE_C_FLAGS_RELEASE} -march=native -mtune=native"
  CACHE STRING "" FORCE)

# optimized debug
set(CMAKE_CXX_FLAGS_OPTDEBUG "${CMAKE_CXX_FLAGS_NATIVE} -g"
  CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_OPTDEBUG "${CMAKE_C_FLAGS_NATIVE} -g"
  CACHE STRING "" FORCE)

# AVX_release
set(CMAKE_CXX_FLAGS_AVX_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx"
  CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_AVX_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mavx"
  CACHE STRING "" FORCE)

enable_testing()
add_subdirectory(test)

# coverage
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_TESTING} --coverage" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_TESTING} --coverage" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "--coverage" CACHE STRING "" FORCE)

install(DIRECTORY include/bs3 DESTINATION include)

# src, bsi sources for generated code
add_subdirectory(src)

# util
add_subdirectory(util)

# benchmark
add_subdirectory(bench EXCLUDE_FROM_ALL)

# examples
add_subdirectory(example EXCLUDE_FROM_ALL)

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ")
include(CPack)
