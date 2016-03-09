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

// example for multi-input single-output

// this example demonstrates how to go from HitData + EsumData -> Signal

// first include our library
#include <bs3/pbs.hh>

// definitions for these examples, will not be needed in real program
#include "defs.hh"

// for simplicity, related types are defined as int.

// and these functions are just placeholders; you need to fill actual
// algorithms here.
Signal reconstruct(const HitData& hit, const EsumData& esum)
{
  return hit + esum;
}

// some cuts
bool cut(const Signal& sig)
{
  return sig>3;
}

int main()
{

  // alias namespace, save some typing
  namespace pu = pbsu;

  // catch and simply re-throw, ensures objects are destructed
  try {

    // open inputs
    auto hit_file = open_sequential_input_file("hit.bs", ExampleRealm());
    auto esum_file = open_sequential_input_file("esum.bs", ExampleRealm());

    // read in parallel
    auto hit_and_esum = pu::zip(hit_file.read_one_type<HitData>(),
                                esum_file.read_one_type<EsumData>());

    // do computation
    auto signals = pu::map(pu::spread(reconstruct), hit_and_esum);

    // cut on signal
    auto good_signals = pu::filter(cut, signals);

    // write output
    pu::copy(good_signals,
             open_sequential_output_file("sig.bs", ExampleRealm()).write_iterator());

  } catch(...) {
    throw;
  }

  return 0;
}
