#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/solver/fv/models/euler/cv.hpp>
#include <hm3/solver/fv/models/euler/indices.hpp>
#include <hm3/solver/fv/models/euler/pv.hpp>
#include <hm3/solver/fv/models/euler/state.hpp>

namespace hm3::solver::fv::euler {

template <dim_t Ad>
struct physics : state, geometry::with_ambient_dimension<Ad>, indices<Ad> {
  physics(num_t gamma) : state{std::move(gamma)} {}

  using cv_t = cv<Ad>;
  using pv_t = pv<Ad>;

  cv_t cv() const noexcept { return cv_t{*this}; }
  pv_t pv() const noexcept { return pv_t{*this}; }

  template <typename S, typename V, typename CellData>
  static void load(V&& v, S const& s, CellData&& cell_data) noexcept {
    cell_data.load("density", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.rho(t);
    });
    cell_data.load("momentum_density",
                   [&](auto c, auto&& d) {
                     auto t = s.variables(c);
                     return v.rho_u(t, d);
                   },
                   v.dimension(),
                   [&](auto&& d) { return v.dimension_name(d); });
    cell_data.load("energy_density", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.rho_E(t);
    });
    cell_data.load("pressure", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.p(t);
    });
    cell_data.load("velocity",
                   [&](auto c, auto&& d) {
                     auto t = s.variables(c);
                     return v.u(t, d);
                   },
                   v.dimension(),
                   [&](auto&& d) { return v.dimension_name(d); });
    cell_data.load("temperature", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.temperature(t);
    });
    cell_data.load("M", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.mach_number(t);
    });
    cell_data.load("a", [&](auto c, auto&&) {
      auto t = s.variables(c);
      return v.speed_of_sound(t);
    });

    // TODO:
    // Gradient
    // Total energy
    // Specific internal energy
    // Enthalpy
  }
};

}  // namespace hm3::solver::fv::euler
