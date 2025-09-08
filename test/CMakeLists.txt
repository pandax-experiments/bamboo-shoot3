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

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

function(pbs_deftest name)
  add_executable(${name} EXCLUDE_FROM_ALL ${name}.cc)
  add_test(${name} ${name})
  add_dependencies(check ${name})
endfunction(pbs_deftest)

add_subdirectory(utils)
add_subdirectory(pbss)
add_subdirectory(pbsf)
