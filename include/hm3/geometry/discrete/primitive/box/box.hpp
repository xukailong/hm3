#pragma once
/// \file
///
/// Box: AABB with constant length.
#include <hm3/geometry/discrete/primitive/point/point.hpp>
#include <hm3/geometry/discrete/primitive/aabb/aabb.hpp>
#include <hm3/geometry/discrete/primitive/aabb/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/aabb/is_box.hpp>
#include <hm3/geometry/discrete/primitive/aabb/ostream.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace box_primitive {

/// Box: AABB with constant length.
template <dim_t Nd>  //
struct box : ranked<Nd, Nd> {
  using point_t           = point<Nd>;
  using aabb_t            = aabb<Nd>;
  using vertex_index_type = dim_t;

  point_t centroid_;
  num_t length_;

  box()                     = default;
  constexpr box(box const&) = default;
  constexpr box(box&&)      = default;
  constexpr box& operator=(box const&) = default;
  constexpr box& operator=(box&&) = default;

  /// Box from \p x_centroid and \p x_length.
  constexpr box(point_t x_centroid, num_t length)
   : centroid_(x_centroid), length_(length) {
    HM3_ASSERT(length > 0., "box length is {} !> 0.!", length);
  }

  /// Box from AABB \p b
  constexpr box(aabb_t const& b) : box(centroid(b), bounding_length(b, 0)) {
    HM3_ASSERT(is_box(b), "AABB not a Box: {}", b);
  }

  /// Box from \p x_min and \p x_max.
  constexpr box(point_t const& x_min, point_t const& x_max)
   : box(aabb_t(x_min, x_max)) {}

  /// Box from centroid and lengths.
  static constexpr box<Nd> at(point_t const& x_c, point_t const& lengths) {
    return box<Nd>(aabb_t::at(x_c, lengths));
  }

  /// Unit box.
  static constexpr box<Nd> unit() noexcept {
    return box<Nd>(point_t::constant(0.5), 1.);
  }
};

template <dim_t Nd>
bool operator==(box<Nd> const& l, box<Nd> const& r) noexcept {
  return math::approx(l.length_, r.length_) && l.centroid_ == r.centroid_;
}

template <dim_t Nd>
bool operator!=(box<Nd> const& l, box<Nd> const& r) noexcept {
  return !(l == r);
}

/// Minimum vertex
template <dim_t Nd> constexpr point<Nd> x_min(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() - point<Nd>::c(.5 * s.length_)};
}

/// Maximum vertex
template <dim_t Nd> constexpr point<Nd> x_max(box<Nd> const& s) noexcept {
  const auto xc = centroid(s);
  return point<Nd>{xc() + point<Nd>::c(.5 * s.length_)};
}

template <dim_t Nd> constexpr bool is_box(box<Nd> const&) noexcept {
  return true;
}

}  // namespace box_primitive

using box_primitive::box;

}  // namespace discrete
}  // namespace geometry
}  // namespace hm3