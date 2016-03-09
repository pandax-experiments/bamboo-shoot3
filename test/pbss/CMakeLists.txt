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

pbs_deftest(test-serialize-primitive)
pbs_deftest(test-parse-primitive)
pbs_deftest(test-serialize-parse-enum)
pbs_deftest(test-serialize-parse-pair)
pbs_deftest(test-serialize-parse-var-uint)
pbs_deftest(test-serialize-iterable)
pbs_deftest(test-parse-container)
pbs_deftest(test-serialize-parse-struct)
pbs_deftest(test-tuple)

pbs_deftest(test-size-helpers)

pbs_deftest(test-size-primitive)
pbs_deftest(test-size-enum)
pbs_deftest(test-size-pair)
pbs_deftest(test-size-var-uint)
pbs_deftest(test-size-iterable)
pbs_deftest(test-size-struct)
pbs_deftest(test-size-tuple)

pbs_deftest(test-contiguous)

pbs_deftest(test-parse-iterator)
