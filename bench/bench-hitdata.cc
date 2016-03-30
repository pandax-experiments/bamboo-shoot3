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

#include <cstdint>
#include <vector>
#include <map>
#include <string>

#define BSIC_RAND_SIZE_MAX 0

#include <bs3/pbss/pbss.hh>

#include "hitdata.hh"
#include "hitdata-gen.hh"

#include "extern-serialize.hh"

#define NPMTS 200

constexpr size_t valid_size(size_t nhits)
{
  return NPMTS*((nhits*69)+8);
}

// baseline to compare against: byte-by-byte copy
// at -O2 GCC does not vectorize this loop so it should be a fair comparison
__attribute__((optimize("O2")))
void plain_memcpy(char* dest, const char* src, size_t n)
{
  while (n--)
    *dest++ = *src++;
}

int main()
{

  using namespace std::chrono;
  using std::make_pair;
  using std::cout;
  using std::endl;
  using std::string;
  using std::ostringstream;
  using pbss::aot_size;

  high_resolution_clock clock;

  for (size_t nhits : { 5, 10, 30, 300, 1000, 3000 }) {

    auto NSAMPLES = (60000 / nhits) +1;

    cout << nhits << ":\n";

    auto hitdata = bsic_rgen(bsic_rgen_tag<HitData>{});
    for (int ipmt=0; ipmt<NPMTS; ++ipmt) {
      auto pmthit = bsic_rgen(bsic_rgen_tag<PmtHit>{});
      pmthit.hits.reserve(nhits);
      for (size_t ihit=0; ihit<nhits; ++ihit)
        pmthit.hits.emplace_back(bsic_rgen(bsic_rgen_tag<SingleHit>{}));
      hitdata.pmtHits.emplace_back(std::move(pmthit));
    }

    auto out = pbss::serialize_to_buffer(hitdata);
    auto size = out.size();

    cout << size << " bytes, amp factor " << (double)size/(double)valid_size(nhits) << "\n";

    {
      duration<uint64_t, std::nano> dur{0};
      pbss::buffer dest(out.size());
      for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
        auto start = clock.now();
        (void)plain_memcpy((char*)&*dest.begin(), (char*)&*out.begin(), out.size());
        dur += clock.now() - start;
      }
      auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
      cout << "plain_memcpy in "
           << time << " us, "
           << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
        ;
    }

    {
      duration<uint64_t, std::nano> dur{0};
      pbss::buffer dest(out.size());
      for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
        auto start = clock.now();
        (void)std::char_traits<char>::copy((char*)&*dest.begin(), (char*)&*out.begin(), out.size());
        dur += clock.now() - start;
      }
      auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
      cout << "memcpy in "
           << time << " us, "
           << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
        ;
    }

    {
      duration<uint64_t, std::nano> dur{0};
      for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
        auto start = clock.now();
        (void)pbss::serialize_to_buffer(hitdata);
        dur += clock.now() - start;
      }
      auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
      cout << "serialize in "
           << time << " us, "
           << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
        ;
    }

    {
      duration<uint64_t, std::nano> dur{0};
      for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
        auto start = clock.now();
        (void)pbss::parse_from_buffer<HitData>(out);
        dur += clock.now() - start;
      }
      auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
      cout << "parsed in "
           << time << " us, "
           << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / (1<<20)) / ((double)time / 1e6) << " MiB/s\n"
        ;
    }

  }
  return 0;
}
