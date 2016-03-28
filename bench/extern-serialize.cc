#include <bs3/pbs.hh>

#include "hitdata.hh"

#pragma GCC optimize("-finline-limit=1200")

template std::string pbss::serialize_to_string(const HitData&);
