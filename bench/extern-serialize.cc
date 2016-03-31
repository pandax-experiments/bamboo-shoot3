#include <bs3/pbs.hh>

#include "hitdata.hh"

template pbss::buffer pbss::serialize_to_buffer(const HitData&);
template pbss::buffer pbss::serialize_to_buffer(const HitData_tuple&);
template HitData pbss::parse_from_buffer<HitData>(const pbss::buffer&);
template HitData_tuple pbss::parse_from_buffer<HitData_tuple>(const pbss::buffer&);
template HitData_tailadd pbss::parse_from_buffer<HitData_tailadd>(const pbss::buffer&);
template HitData_mismatch pbss::parse_from_buffer<HitData_mismatch>(const pbss::buffer&);
