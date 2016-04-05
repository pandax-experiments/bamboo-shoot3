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

template <class F>
double time_us(size_t count, F&& f)
{
  using clock = std::chrono::high_resolution_clock;

  std::chrono::duration<double, std::nano> dur {};

  for (auto _count=count; _count; --_count) {
    auto start = clock::now();
    auto x = ((F&&)f)();
    dur += clock::now() - start;
    (void)x;
  }

  return std::chrono::duration_cast<
    std::chrono::duration<double, std::micro> >(dur)
    .count() / double(count);
}

int main()
{

  using std::make_pair;
  using std::cout;
  using std::endl;
  using std::string;
  using std::ostringstream;
  using pbss::aot_size;

  const double MB = 1<<20;

  for (size_t nhits : std::initializer_list<size_t> { 5, 10, 30, 300, 1000, 3000 }) {

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
      pbss::buffer dest(out.size());
      auto time = time_us(NSAMPLES, [&]() {
        std::char_traits<char>::copy(
          reinterpret_cast<char*>(&*dest.begin()),
          reinterpret_cast<const char*>(&*out.begin()), out.size());
        return 0;
      });
      cout << "memcpy in "
           << time << " us, "
           << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
        ;
    }

    {
      auto time = time_us(NSAMPLES, [&]() {
        return pbss::serialize_to_buffer(hitdata);
      });
      cout << "serialize in "
           << time << " us, "
           << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
        ;
    }

    {
      auto time = time_us(NSAMPLES, [&]() {
        return pbss::parse_from_buffer<HitData>(out);
      });
      cout << "parsed in "
           << time << " us, "
           << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
        ;
    }

    {
      auto time = time_us(NSAMPLES, [&]() {
        return pbss::parse_from_buffer<HitData_tailadd>(out);
      });
      cout << "tailadd parsed in "
           << time << " us, "
           << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
        ;
    }

    {
      auto time = time_us(NSAMPLES, [&]() {
        return pbss::parse_from_buffer<HitData_mismatch>(out);
      });
      cout << "mismatch parsed in "
           << time << " us, "
           << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
           << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
        ;
    }

    {
      // tuple
      auto hitdata = bsic_rgen(bsic_rgen_tag<HitData_tuple>{});
      for (int ipmt=0; ipmt<NPMTS; ++ipmt) {
        auto pmthit = bsic_rgen(bsic_rgen_tag<PmtHit_tuple>{});
        pmthit.hits.reserve(nhits);
        for (size_t ihit=0; ihit<nhits; ++ihit)
          pmthit.hits.emplace_back(bsic_rgen(bsic_rgen_tag<SingleHit_tuple>{}));
        hitdata.pmtHits.emplace_back(std::move(pmthit));
      }

      auto out = pbss::serialize_to_buffer(hitdata);
      auto size = out.size();

      {
        auto time = time_us(NSAMPLES, [&]() {
          return pbss::serialize_to_buffer(hitdata);
        });
        cout << "serialize(tuple) in "
             << time << " us, "
             << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
             << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
          ;
      }

      {
        auto time = time_us(NSAMPLES, [&]() {
          return pbss::parse_from_buffer<HitData_tuple>(out);
        });
        cout << "parsed(tuple) in "
             << time << " us, "
             << "real " << ((double)size / MB) / (time / 1e6) << " MiB/s, "
             << "effective " << ((double)valid_size(nhits) / MB) / (time / 1e6) << "MiB/s\n"
          ;
      }

    }

  }
  return 0;
}
