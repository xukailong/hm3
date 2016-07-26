#pragma once
/// \file
///
/// Bounding lengths of a plane polygon.
#include <hm3/geometry/discrete/algorithm/bounding_length.hpp>
#include <hm3/geometry/discrete/primitive/polygon/bounding_volume.hpp>

namespace hm3 {
namespace geometry {
namespace discrete {
namespace polygon_primitive {

/// Bounding length of the polygon \p p along the \p d spatial component.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr num_t bounding_length(P&& p, dim_t d) noexcept {
  num_t min_ = 0., max_ = 0.;
  for (auto&& v : vertices(p)) {
    min_ = std::min(min_, v(d));
    max_ = std::max(max_, v(d));
  }
  if (math::approx(min_, max_)) {
    min_ -= math::eps;
    max_ += math::eps;
  }
  return max_ - min_;
  //  return bounding_length(axis_aligned_bounding_box(p), d);
}

/// Bounding lengths of the polygon \p p along all spatial components.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr vector<Nd> all_bounding_lengths(P&& p) noexcept {
  return all_bounding_lengths(axis_aligned_bounding_box(p));
}

/// Max bounding length of the polygon \p p.
template <typename P, typename UP = uncvref_t<P>, dim_t Nd = UP::dimension(),
          CONCEPT_REQUIRES_(Polygon<UP, Nd>{})>
constexpr num_t max_bounding_length(P&& p) noexcept {
  return all_bounding_lengths(p)().array().maxCoeff();
}

}  // namespace polygon_primitive
}  // namespace discrete
}  // namespace geometry
}  // namespace hm3