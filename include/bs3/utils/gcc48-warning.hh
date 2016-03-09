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

#ifndef BS3_GCC48_WARNING_HH
#define BS3_GCC48_WARNING_HH

#if !defined(__clang__) && defined(__GNUC__) && (__GNUC__<=4) && (__GNUC_MINOR__<=8)

#pragma message "You seem to be using GCC<=4.8.  Because of some bugs in your GCC that we cannot workaround, some features are disabled, which means your build of Bamboo Shoot 3 will be incomplete.  Enabled features will still work."
#define BS3_DETECTED_GCC_48

#endif  // GCC 4.8 check

#endif /* BS3_GCC48_WARNING_HH */
