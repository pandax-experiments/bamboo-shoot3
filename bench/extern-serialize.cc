#include <bs3/pbs.hh>

#include "hitdata.hh"

#pragma GCC optimize("-finline-limit=1200")

template pbss::buffer pbss::serialize_to_buffer(const HitData&);
template HitData pbss::parse_from_buffer<HitData>(const pbss::buffer&);
template HitData_tailadd pbss::parse_from_buffer<HitData_tailadd>(const pbss::buffer&);
template HitData_mismatch pbss::parse_from_buffer<HitData_mismatch>(const pbss::buffer&);
