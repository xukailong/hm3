#pragma once
/// \file
///
/// Compute node neighbor algorithm
#include <hm3/grid/hierarchical/tree/algorithm/node_at.hpp>
#include <hm3/grid/hierarchical/tree/algorithm/shift_location.hpp>
#include <hm3/grid/hierarchical/tree/concepts.hpp>
#include <hm3/grid/hierarchical/tree/location/default.hpp>
#include <hm3/grid/hierarchical/tree/relations/neighbor.hpp>
#include <hm3/grid/hierarchical/tree/types.hpp>

namespace hm3::tree {

/// Find neighbor \p n of node at location \p loc
///
/// Note: the manifold is associated to the neighbor index type
/// (todo: strongly type this)
///
struct node_neighbor_fn {
  template <typename Loc, typename NeighborIdx,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto location(Loc l, NeighborIdx p) const noexcept {
    static_assert(Loc::ambient_dimension() == Manifold::ambient_dimension(),
                  "");
    return shift_location(l, Manifold{}[p]);
  }

  template <typename Tree, typename NeighborIdx,
            typename Loc      = loc_t<Tree::ambient_dimension()>,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto location(Tree const& t, node_idx n, NeighborIdx p, Loc l = Loc{}) const
   noexcept {
    static_assert(Tree::ambient_dimension()
                   == ranges::uncvref_t<Loc>::ambient_dimension(),
                  "");
    static_assert(Tree::ambient_dimension() == Manifold::ambient_dimension(),
                  "");
    return location(node_location(t, n, l), p);
  }

  template <typename Tree, typename Loc, typename NeighborIdx,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, Loc&& loc, NeighborIdx p) const noexcept
   -> node_idx {
    static_assert(Tree::ambient_dimension()
                   == ranges::uncvref_t<Loc>::ambient_dimension(),
                  "");
    static_assert(Tree::ambient_dimension() == Manifold::ambient_dimension(),
                  "");
    return node_at(t, location(loc, p));
  }

  template <typename Tree, typename NeighborIdx,
            typename Loc      = loc_t<Tree::ambient_dimension()>,
            typename Manifold = get_tag_t<NeighborIdx>,
            CONCEPT_REQUIRES_(Location<Loc>{})>
  auto operator()(Tree const& t, node_idx n, NeighborIdx p, Loc l = Loc{}) const
   noexcept -> node_idx {
    static_assert(Tree::ambient_dimension()
                   == ranges::uncvref_t<Loc>::ambient_dimension(),
                  "");
    static_assert(Tree::ambient_dimension() == Manifold::ambient_dimension(),
                  "");
    return node_at(t, location(t, n, p, l));
  }
};

namespace {
constexpr auto const& node_neighbor = static_const<node_neighbor_fn>::value;
}  // namespace

}  // namespace hm3::tree
