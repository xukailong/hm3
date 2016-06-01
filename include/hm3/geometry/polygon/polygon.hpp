#pragma once
/// \file
///
/// Polygon data-type
#include <hm3/geometry/dimension/dimensional.hpp>
#include <hm3/geometry/point.hpp>
#include <hm3/geometry/polygon/concept.hpp>
#include <hm3/utility/inline_vector.hpp>

namespace hm3 {
namespace geometry {

template <dim_t Nd, ppidx_t MaxNp>
struct polygon : dimensional<Nd>, inline_vector<point<Nd>, MaxNp> {
  using self = inline_vector<point<Nd>, MaxNp>;
  using self::self;
  using self::operator=;
  polygon()           = default;

  template <typename Shape, CONCEPT_REQUIRES_(Polygon<Shape>{})>
  polygon(Shape const& s) {
    RANGES_FOR (auto&& c, vertices(s)) { this->push_back(c); }
  }

  static constexpr sint_t max_points() noexcept { return MaxNp; }
};

template <dim_t Nd, ppidx_t MaxNp>  //
bool empty(polygon<Nd, MaxNp> const& p) {
  return p.empty();
}

template <dim_t Nd, ppidx_t MaxNp>
bool operator==(polygon<Nd, MaxNp> const& a,
                polygon<Nd, MaxNp> const& b) noexcept {
  return equal(a, b);
}

template <dim_t Nd, ppidx_t MaxNp>
bool operator!=(polygon<Nd, MaxNp> const& a,
                polygon<Nd, MaxNp> const& b) noexcept {
  return !(a == b);
}

/// Polygon vertices
template <dim_t Nd, ppidx_t MaxNp>
constexpr auto vertices(polygon<Nd, MaxNp> p) noexcept {
  return p;
}

/// Polygon vertices
template <dim_t Nd, ppidx_t MaxNp>
constexpr auto vertex(polygon<Nd, MaxNp> p, suint_t v) noexcept {
  return p[v];
}

/// Vertex indices
template <dim_t Nd, ppidx_t MaxNp>
auto vertex_indices(polygon<Nd, MaxNp> const& p) noexcept {
  return view::iota(std::size_t{0}, p.size());
}

}  // namespace geometry
}  // namespace hm3
