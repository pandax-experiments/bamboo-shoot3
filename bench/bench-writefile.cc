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

#include <chrono>
#include <utility>
#include <sstream>
#include <random>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>

#include <cstdint>
#include <vector>
#include <map>
#include <string>

#define BSIC_RAND_SIZE_MAX 0

#include <bs3/pbss/pbss.hh>
#include <bs3/pbsf/pbsf.hh>

#include "hitdata.hh"
#include "hitdata-gen.hh"

#include "extern-serialize.hh"

PBSF_DECLARE_REALM(
  BenchRealm, 42,
  PBSF_REGISTER_TYPE(1, HitData));

#define NHITS 200
#define NPMTS 200
#define NSAMPLES 300

int main(int argc, char** argv)
{

  using namespace std::chrono;
  using std::cout;

  high_resolution_clock clock;

  auto hitdata = bsic_rgen(bsic_rgen_tag<HitData>{});
  for (int ipmt=0; ipmt<NPMTS; ++ipmt) {
    auto pmthit = bsic_rgen(bsic_rgen_tag<PmtHit>{});
    pmthit.hits.reserve(NHITS);
    for (int ihit=0; ihit<NHITS; ++ihit)
      pmthit.hits.emplace_back(SingleHit());
    hitdata.pmtHits.emplace_back(std::move(pmthit));
  }

  std::string filename(argc>1 ? argv[1] : "/dev/null");

  {
    auto start = clock.now();

    auto out = open_sequential_output_file(filename, BenchRealm());
    std::fill_n(out.write_iterator(), NSAMPLES, hitdata);

    auto dur = clock.now() - start;
    auto time = duration_cast<milliseconds>(dur).count();
    cout << NSAMPLES << " objs written in " << time << " ms\n";
  }

  {
    auto in = open_sequential_input_file(filename, BenchRealm());

    auto start = clock.now();

    for (const HitData& hit : in.read_one_type<HitData>())
      (void)hit;

    auto dur = clock.now() - start;
    auto time = duration_cast<milliseconds>(dur).count();
    cout << "read back in " << time << " ms\n";
  }

  return 0;
}
