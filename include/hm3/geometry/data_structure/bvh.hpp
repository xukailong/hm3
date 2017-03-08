#pragma once
/// \file
///
/// Bounding Volume Hierarchy
#include <hm3/geometry/data_structure/bvh/bvh.hpp>
#include <hm3/geometry/data_structure/bvh/node.hpp>
#include <hm3/geometry/data_structure/bvh/types.hpp>

namespace hm3::geometry {

template <dim_t Ad>
using bounding_volume_hierarchy = bvh::bvh<Ad>;

}  // namespace hm3::geometry
