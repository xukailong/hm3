#pragma once
/// \file
///
/// Tree relations
///
/// TODO: move some of these to grid relations since they are also needed by
/// structured grids
#include <hm3/geometry/algorithm/bounding_length.hpp>
#include <hm3/geometry/algorithm/centroid.hpp>
#include <hm3/geometry/concepts.hpp>
#include <hm3/geometry/fwd.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>
#include <hm3/math/core.hpp>
#include <hm3/utility/array.hpp>
#include <hm3/utility/assert.hpp>
#include <hm3/utility/bounded.hpp>
#include <hm3/utility/fatal_error.hpp>
/// Use look-up table for the relative children position instead of arithmetic
/// operations
#define HM3_USE_CHILDREN_LOOKUP_TABLE

namespace hm3::tree {

//

/// \name Tree relations
///@{

/// Number of children of a node
///
/// \param ad [in] spatial dimension of the node
///
/// Formula: \f$ 2^{nd} \f$
///
static constexpr cpidx_t no_children(dim_t ad) noexcept {
  return math::ipow(cpidx_t{2}, cpidx_t(ad));
}

/// Number of siblings of a node
///
/// Siblings are nodes sharing the same parent.
///
/// \param ad [in] spatial dimension of the node
///
/// Formula: \f$ 2^{nd} \f$
///
static constexpr cpidx_t no_siblings(dim_t ad) noexcept {
  return no_children(ad);
}

/// Number of nodes sharing a single face within a node
///
/// \param ad [in] spatial dimension of the node
/// \param m [in] spatial dimension of a face
///
/// Formula: \f$ 2 ^ {m} \f$ for \f$ m >= m \f$ otherwise 0.
///
static constexpr npidx_t no_nodes_sharing_face(dim_t ad, dim_t m) noexcept {
  return (ad >= m) ? math::ipow(npidx_t{2}, npidx_t(m)) : npidx_t{0};
}

/// Number of nodes sharing a single face at a given level
///
/// \param ad [in] spatial dimension of the nodes
/// \param m [in] spatial dimension of the face
/// \param level [in] level starting from the root node (which is at level 0)
///
/// Formula: \f$ (2^{m})^{l} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_sharing_face_at_level(
 dim_t ad, dim_t m, level_idx level) noexcept {
  const auto nsf = no_nodes_sharing_face(ad, m);
  return nsf == npidx_t{0} and *level == lidx_t{0}
          ? nidx_t{0}
          : math::ipow(nidx_t{nsf}, static_cast<nidx_t>(*level));
}

/// Number of node faces
///
/// \param ad [in] spatial dimension of the node
/// \param m [in] spatial dimension of the face
///
/// Formula: \f$\ 2^{n_d - m} \begin{pmatrix} n_d // m \end{pmatrix} \f$
///
static constexpr npidx_t no_faces(dim_t ad, dim_t m) noexcept {
  return m <= ad ? math::ipow(npidx_t{2}, npidx_t(ad - m))
                    * math::binomial_coefficient(ad, m)
                 : npidx_t{0};
}

/// Number of nodes at an uniformly refined level \p level
///
/// \param ad [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ (2^{n_d})^{\mathrm{level}} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_at_uniform_level(dim_t ad, level_idx level) {
  return math::ipow(math::ipow(nidx_t{2}, nidx_t(ad)),
                    static_cast<nidx_t>(*level));
}

/// Number of nodes in a tree with a uniformly refined level \p level
///
/// \param ad [in] spatial dimension of the nodes
/// \param level [in] distance from nodes to the root node
///
/// Formula \f$\ \sum_{l = 0}^{level} (2^{n_d})^{\mathrm{l}} \f$
///
/// \note The result can be very big => nidx_t type.
///
static constexpr nidx_t no_nodes_until_uniform_level(dim_t ad,
                                                     level_idx level) {
  nidx_t no_nodes = 0;
  for (lidx_t l = 0, e = *level; l <= e; ++l) {
    no_nodes += no_nodes_at_uniform_level(ad, l);
  }
  return no_nodes;
}

/// Normalized length of a node at level \p l (for a root node of length = 1)
static constexpr num_t node_length_at_level(level_idx l) {
  return num_t{1.} / math::ipow(nidx_t{2}, static_cast<nidx_t>(*l));
}

template <dim_t Ad>
struct relative_child_positions_ {
  static constexpr inline array<array<rcpidx_t, 0>, 0> stencil{{}};
};

template <>
struct relative_child_positions_<1> {
  static constexpr inline array<array<rcpidx_t, 1>, 2> stencil{{
   {{-1}}, {{1}}
   //
  }};
};

template <>
struct relative_child_positions_<2> {
  static constexpr inline array<array<rcpidx_t, 2>, 4> stencil{{
   {{-1, -1}}, {{1, -1}}, {{-1, 1}}, {{1, 1}}
   //
  }};
};

template <>
struct relative_child_positions_<3> {
  static constexpr inline array<array<rcpidx_t, 3>, 8> stencil{{
   {{-1, -1, -1}},
   {{1, -1, -1}},
   {{-1, 1, -1}},
   {{1, 1, -1}},
   {{-1, -1, 1}},
   {{1, -1, 1}},
   {{-1, 1, 1}},
   {{1, 1, 1}}
   //
  }};
};

namespace {
template <dim_t Ad>
static constexpr auto relative_child_position_stencil
 = relative_child_positions_<Ad>::stencil;
}  // namespace

/// Relative position of the children w.r.t. their parent's center:
///
/// \tparam Ad number of spatial dimensions of the node
/// \param [in] p position of the children
///
/// \returns relative position (+1/-1, ...) of child \p w.r.t. his parent node
/// center (an array of size ad)
///
/// That is:
///              __________________________
///            /|   pos: 6   |   pos: 7  /|
///           / | (-1,+1,+1) | (+1,+1,+1) |
///          /  |____________|____________|
///         /   |   pos: 4   |   pos: 5   |
///        /    | (-1,-1,+1) | (+1,-1,+1) |
///       /     |____________|____________|
///      /     /                   /     /
///     /_____/___________________/     /
///    |   pos: 2   |   pos: 3   |     /    d (1) ^
///    | (-1,+1,-1) | (+1,+1,-1) |    /           |     ^ z (2)
///    |____________|____________|   /            |    /
///    |   pos: 0   |   pos: 1   |  /             |  /
///    | (-1,-1,-1) | (+1,-1,-1) | /              |/
///    |____________|____________|/               o-------> x (0)
///
///
///
///
///
template <dim_t Ad>
constexpr auto relative_child_position(const child_pos<Ad> p)
 -> const offset_t<Ad> {
#ifdef HM3_USE_CHILDREN_LOOKUP_TABLE
  return relative_child_position_stencil<Ad>[*p];
#else
  offset_t<Ad> r;
  for (dim_t d = 0; d < Ad; ++d) {
    r[d] = (*p / math::ipow(coidx_t{2}, coidx_t{d})) % 2 ? 1 : -1;
  }
  return r;
#endif
}

namespace child_centroid_detail {
struct child_centroid_fn {
  /// Centroid of child at position \p child_position for a parent with centroid
  /// \p parent_centroid and length \p parent_length
  template <typename Point, dim_t Ad>
  constexpr auto operator()(const child_pos<Ad> child_position,
                            Point parent_centroid, num_t parent_length) const
   noexcept -> Point {
    static_assert(geometry::Point<Point, Ad>{});
    const auto rcp = relative_child_position<Ad>(child_position);
    const num_t l4 = parent_length / 4.;
    for (dim_t d = 0; d < Ad; ++d) { parent_centroid(d) += l4 * rcp[d]; }
    return parent_centroid;
  }

  /// Centroid of child at position \p child_position for a \p parent geometry
  template <typename Box, dim_t Ad>
  constexpr auto operator()(const child_pos<Ad> child_position,
                            Box parent) const noexcept
   -> geometry::associated::point_t<Box> {
    static_asssert(geometry::trait::check<Box, geometry::trait::box<Ad>>);
    return child_centroid(child_position, geometry::centroid(parent),
                          geometry::bounding_length(parent));
  }
};
}  // namespace child_centroid_detail

namespace {
constexpr auto const& child_centroid
 = static_const<child_centroid_detail::child_centroid_fn>::value;
}

template <typename Box, dim_t Ad>
constexpr auto child_geometry(const child_pos<Ad> child_position, Box parent)
 -> Box {
  static_asssert(geometry::trait::check<Box, geometry::trait::box<Ad>>);
  return {child_centroid(child_position, parent),
          geometry::bounding_length(parent) / 2.};
}

///@} // Tree relations

}  // namespace hm3::tree
