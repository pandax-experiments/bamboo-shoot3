#ifndef BS3_BENCH_EXTERN_SERIALIZE_HH
#define BS3_BENCH_EXTERN_SERIALIZE_HH

extern template pbss::buffer pbss::serialize_to_buffer(const HitData&);
extern template HitData pbss::parse_from_buffer<HitData>(const pbss::buffer&);
extern template HitData_tailadd pbss::parse_from_buffer<HitData_tailadd>(const pbss::buffer&);
extern template HitData_mismatch pbss::parse_from_buffer<HitData_mismatch>(const pbss::buffer&);

#endif /* BS3_BENCH_EXTERN_SERIALIZE_HH */
