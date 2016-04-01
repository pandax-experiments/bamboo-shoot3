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

#ifndef BS3_PBSF_RANGE_API_HH
#define BS3_PBSF_RANGE_API_HH

// range-based API for manipulating files

#include <fstream>
#include <string>
#include <utility>
#include <memory>

#include <bs3/utils/range.hh>

#include "data-block.hh"

namespace pbsf {

template <class T, class File>
pbsu::range<skipping_read_iterator<typename File::realm_type, T>>
read_one_type(File f);

template <class File>
heterogeneous_write_iterator<typename File::realm_type>
write_iterator(File f);

inline
namespace abiv1 {

template <class Stream, class Realm>
struct sequential_file {

  typedef Realm realm_type;
  typedef Stream stream_type;

  std::shared_ptr<Stream> stream_ptr;

  template <class T>
  pbsu::range<skipping_read_iterator<realm_type, T>>
  read_one_type()
  {
    return pbsf::read_one_type<T>(*this);
  }

  heterogeneous_write_iterator<realm_type>
  write_iterator()
  {
    return pbsf::write_iterator(*this);
  }

};

} // inline namespace abiv1

template <class Realm>
sequential_file<std::ifstream, Realm>
open_sequential_input_file(const std::string& filename, Realm r)
{
  auto s = std::make_shared<std::ifstream>(filename);
  s->exceptions(std::ios_base::failbit | std::ios_base::badbit);
  if (!check_file(*s, r))
    throw unknown_realm_error();
  return { s };
}

template <class T, class File>
pbsu::range<skipping_read_iterator<typename File::realm_type, T>>
read_one_type(File f)
{
  return { {*f.stream_ptr}, {} };
}

template <class Realm>
sequential_file<std::fstream, Realm>
open_sequential_output_file(const std::string& filename, Realm r, bool overwrite=true)
{
  using std::ios_base;
  auto flag = ios_base::out | ios_base::in | ios_base::ate | (
    overwrite ? ios_base::trunc : ios_base::app);
  auto s = std::make_shared<std::fstream>(filename, flag);
  s->exceptions(std::ios_base::failbit | std::ios_base::badbit);
  if (static_cast<std::streamoff>(s->tellg()) == 0) {
    write_header(*s, r);
    return { s };
  } else {
    s->seekg(0);
    if (!check_file(*s, r))
      throw unknown_realm_error();
    // Workaround a bug in system libraries on RHEL 6; I did not bother to
    // locate the bug.  It seems like, even with app flag set, one must
    // still manually seek to end.
    s->seekp(0, ios_base::end);
    return { s };
  }
}

template <class File>
heterogeneous_write_iterator<typename File::realm_type>
write_iterator(File f)
{
  return { *f.stream_ptr };
}

} // namespace pbsf

#endif /* BS3_PBSF_RANGE_API_HH */
