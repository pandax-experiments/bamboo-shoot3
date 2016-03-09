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

#ifndef BS3_PBSF_FILE_HEADER_HH
#define BS3_PBSF_FILE_HEADER_HH

#include <bs3/pbss/pbss.hh>

#include "defs.hh"
#include "realm.hh"

namespace pbsf {

template <uint32_t id, class... entries>
bool check_file(std::istream& stream, realm<id, entries...>)
{
  using pbss::parse;
  return FileHeader{magic, id} == parse<FileHeader>(stream);
}

template <uint32_t id, class... entries>
void write_header(std::ostream& stream, realm<id, entries...>)
{
  using pbss::serialize;
  serialize(stream, FileHeader{magic, id});
}

}

#endif /* BS3_PBSF_FILE_HEADER_HH */
