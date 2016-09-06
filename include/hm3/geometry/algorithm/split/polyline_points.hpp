#pragma once
/// \file
///
/// Split polyline at a point.
#include <hm3/geometry/algorithm/distance.hpp>
#include <hm3/geometry/algorithm/intersection/segment_point.hpp>
#include <hm3/geometry/algorithm/split.hpp>
#include <hm3/geometry/primitive/polyline/polyline.hpp>
#include <hm3/geometry/primitive/segment/segment.hpp>
#include <hm3/utility/range.hpp>
#include <hm3/utility/small_vector.hpp>
#include <hm3/utility/variant.hpp>
#include <hm3/utility/vector.hpp>

namespace hm3::geometry::polyline_primitive {

/// Splits the polyline \p pl at the points \p ps.
template <dim_t Nd, typename S, typename Points,
          typename Ret   = /*TODO: small_*/ vector<polyline<Nd, S>>,
          typename Point = uncvref_t<range_value_t<Points>>,
          CONCEPT_REQUIRES_(Range<Points>{} and Same<Point, point<Nd>>{})>
inline auto split(polyline<Nd, S> const& pl, Points&& ps) -> Ret {
  HM3_ASSERT(face_size(pl) > 0,
             "polyline cannot be empty (must at least contain one segment)!");
  using p_t  = point<Nd>;
  using pl_t = polyline<Nd, S>;

  Ret pls;
  pls.push_back(pl_t{});
  const suint_t no_ips      = size(ps);
  const suint_t no_segments = face_size(pl);
  small_vector<suint_t, 4> points_in_segment;
  small_vector<num_t, 4> points_in_segment_line_parameter;

  for (auto&& sidx : face_indices(pl)) {
    points_in_segment.clear();
    points_in_segment_line_parameter.clear();

    auto s = face(pl, sidx);
    pls.back().push_back(s.x(0));

    // find the points in the segment
    for (auto&& pidx : view::ints(0_su, no_ips)) {
      auto&& p = ps[pidx];
      auto ir  = intersection(s, p);
      visit(
       [&](auto&& v) {
         using T = uncvref_t<decltype(v)>;
         if
           constexpr(Same<T, p_t>{}) { points_in_segment.push_back(pidx); }
       },
       ir);
    }

    if (!points_in_segment.empty()) {
      // sort points by value of the line parameter
      auto sl = s.line();
      for (auto&& pidx : points_in_segment) {
        points_in_segment_line_parameter.push_back(
         parameter(sl, ps[pidx]).value());
      }
      HM3_ASSERT(
       size(points_in_segment) == size(points_in_segment_line_parameter), "");
      ranges::sort(
       view::zip(points_in_segment, points_in_segment_line_parameter),
       [](auto&& a, auto&& b) { return a.second < b.second; });

      // insert the points into the polyline
      const auto no_points = size(points_in_segment);
      for (suint_t pidx = 0; pidx < no_points; ++pidx) {
        auto&& p = ps[points_in_segment[pidx]];
        if (p == s.x(0) or (p == s.x(1) and sidx == no_segments - 1)) {
          continue;
        }
        pls.back().push_back(p);
        pls.push_back(pl_t{});
        if (p == s.x(1)) { continue; }
        pls.back().push_back(p);
      }
    }

    if (sidx == no_segments - 1) { pls.back().push_back(s.x(1)); }
  }
  return pls;
}

}  // namespace hm3::geometry::polyline_primitive