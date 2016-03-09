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

#define NHITS 200
#define NPMTS 200
#define NSAMPLES 500

const size_t valid_size = NPMTS*((NHITS*69)+8);

// baseline to compare against: byte-by-byte copy
// at -O2 GCC does not vectorize this loop so it should be a fair comparison
__attribute__((optimize("O2")))
void plain_memcpy(char* dest, const char* src, size_t n)
{
  while (n--)
    *dest++ = *src++;
}

__attribute__((optimize("no-inline")))
void do_serialize(const HitData& hitdata)
{
  (void)pbss::serialize_to_string(hitdata);
}

__attribute__((optimize("no-inline")))
void do_parse(const std::string& str)
{
  (void)pbss::parse_from_string<HitData>(str);
}

int main()
{

  using namespace std::chrono;
  using std::make_pair;
  using std::cout;
  using std::endl;
  using std::string;
  using std::ostringstream;
  using pbss::serialize_to_string;
  using pbss::parse_from_string;
  using pbss::aot_size;

  high_resolution_clock clock;

  auto hitdata = bsic_rgen(bsic_rgen_tag<HitData>{});
  for (int ipmt=0; ipmt<NPMTS; ++ipmt) {
    auto pmthit = bsic_rgen(bsic_rgen_tag<PmtHit>{});
    pmthit.hits.reserve(NHITS);
    for (int ihit=0; ihit<NHITS; ++ihit)
      pmthit.hits.emplace_back(bsic_rgen(bsic_rgen_tag<SingleHit>{}));
    hitdata.pmtHits.emplace_back(std::move(pmthit));
  }

  std::string out = serialize_to_string(hitdata);
  auto size = out.size();

  cout << size << " bytes, amp factor " << (double)size/valid_size << "\n";

  {
    duration<uint64_t, std::nano> dur{0};
    std::string dest(out.size(), 0);
    for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
      auto start = clock.now();
      (void)plain_memcpy(&*dest.begin(), &*out.begin(), out.size());
      dur += clock.now() - start;
    }
    auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
    cout << "plain_memcpy in "
         << time << " us, "
         << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
         << "effective " << ((double)valid_size / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
      ;
  }

  {
    duration<uint64_t, std::nano> dur{0};
    std::string dest(out.size(), 0);
    for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
      auto start = clock.now();
      (void)std::char_traits<char>::copy(&*dest.begin(), &*out.begin(), out.size());
      dur += clock.now() - start;
    }
    auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
    cout << "memcpy in "
         << time << " us, "
         << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
         << "effective " << ((double)valid_size / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
      ;
  }

  {
    duration<uint64_t, std::nano> dur{0};
    for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
      auto start = clock.now();
      do_serialize(hitdata);
      dur += clock.now() - start;
    }
    auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
    cout << "serialize in "
         << time << " us, "
         << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
         << "effective " << ((double)valid_size / (1<<20)) / ((double)time / 1e6) << "MiB/s\n"
      ;
  }

  {
    duration<uint64_t, std::nano> dur{0};
    for (size_t isamp=0; isamp<NSAMPLES; ++isamp) {
      auto start = clock.now();
      do_parse(out);
      dur += clock.now() - start;
    }
    auto time = duration_cast<microseconds>(dur).count()/NSAMPLES;
    cout << "parsed in "
         << time << " us, "
         << "real " << ((double)size / (1<<20)) / ((double)time / 1e6) << " MiB/s, "
         << "effective " << ((double)valid_size / (1<<20)) / ((double)time / 1e6) << " MiB/s\n"
      ;
  }

  return 0;
}
