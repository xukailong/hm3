#pragma once
/// \file
///
/// Output stream operator for a ray.
#include <hm3/geometry/primitive/ray/ray.hpp>

namespace hm3::geometry::ray_primitive {

template <typename OStream, dim_t Nd>
OStream& operator<<(OStream& o, ray<Nd> const& r) {
  o << "[o: " << r.origin() << ", d: " << r.direction() << "]";
  return o;
}

}  // namespace hm3::geometry::ray_primitive
