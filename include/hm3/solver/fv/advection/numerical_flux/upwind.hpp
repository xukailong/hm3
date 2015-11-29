#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/dimensions.hpp>
#include <hm3/solver/fv/euler/equation_of_state.hpp>
#include <hm3/solver/fv/euler/indices.hpp>
#include <hm3/solver/fv/euler/state.hpp>
#include <hm3/utility/matrix.hpp>
#include <hm3/utility/static_const.hpp>

namespace hm3 {
namespace solver {
namespace fv {
namespace advection {

namespace flux {

struct upwind_fn {
  /// Local Lax-Friedrichs Flux
  ///
  /// Computes the \p d-th component of the Local-Lax_Friedrichs flux at an
  /// interface with left \p v_l and right \p v_r states.
  template <typename V, typename VT, typename State,
            typename var_v = num_a<std::decay_t<VT>::nvars()>>
  constexpr auto operator()(VT&& vt, V&& v_l, V&& v_r, suint_t d,
                            State&& s) const noexcept {
    var_v f = vt.velocity(d) * v_l;
    return f;
  }
};

namespace {
constexpr auto&& upwind = static_const<upwind_fn>::value;
}  // namespace

}  // namespace flux
}  // namespace advection
}  // namespace fv
}  // namespace solver
}  // namespace hm3
