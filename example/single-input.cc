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

// example for single-input multi-output

// this example demonstrates how to go from RawData -> HitData + EsumData

// first include our library
#include <bs3/pbs.hh>

// definitions for these examples, will not be needed in real program
#include "defs.hh"

// for simplicity, related types are defined as int.

// and these functions are just placeholders; you need to fill actual
// algorithms here.
HitData find_hit(const RawData& r) { return r+1; }
EsumData esum(const RawData& r) { return r+2; }

// some cuts
bool raw_cut(const RawData& r) { return r>3; }
bool hit_cut(const HitData& h) { return h<8; }
bool esum_cut(const EsumData& e) { return e<6; }

int main()
{

  // alias namespace, save some typing
  namespace pu = pbsu;

  // catch and simply re-throw, ensures objects are destructed
  try {

    // open the input raw data file
    auto raw_file = open_sequential_input_file("raw.bs", ExampleRealm());
    // ExampleRealm is a pack of supported types.  In this example it
    // contains only int.
    // and this will be all RawData in input file
    auto raw_data = raw_file.read_one_type<RawData>();

    // add a cut on raw data
    auto cut_raw = pu::filter(raw_cut, raw_data);

    // map it to tuples of { HitData, EsumData }
    auto hit_and_esum = pu::map(pu::collect_result(find_hit, esum), cut_raw);

    // open output files
    auto hit_file = open_sequential_output_file("hit.bs", ExampleRealm());
    auto esum_file = open_sequential_output_file("esum.bs", ExampleRealm());

    // then write each part to corresponding output file
    pu::copy(hit_and_esum,
             pu::spread_writer(
               // add a cut on hit data only --- esum not affected
               pu::filtering_writer(hit_cut, hit_file.write_iterator()),
               esum_file.write_iterator()));

  } catch(...) {
    throw;
  }

  return 0;
}
