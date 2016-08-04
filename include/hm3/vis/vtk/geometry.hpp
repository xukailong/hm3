#pragma once
/// \file
///
/// Maps different cell type geometries
#include <hm3/geometry/primitive/aabb.hpp>
#include <hm3/geometry/primitive/box.hpp>
#include <hm3/geometry/primitive/polygon.hpp>
#include <hm3/utility/variant.hpp>

namespace hm3 {
namespace vis {
namespace vtk {

template <dim_t Nd> struct supported_geometries {
  using type
   = std::experimental::variant<geometry::aabb<Nd>, geometry::box<Nd>>;
};

template <> struct supported_geometries<2_u> {
  using type
   = std::experimental::variant<geometry::aabb<2_u>, geometry::box<2_u>,
                                geometry::bounded_polygon<2_u, 5>>;
};

template <dim_t Nd> using geometries = typename supported_geometries<Nd>::type;

}  // namespace vtk
}  // namespace vis
}  // namespace hm3
