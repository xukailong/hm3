#pragma once
/// \file
///
/// Interprets cell variables as conservative variables
#include <hm3/geometry/algorithm/ambient_dimension.hpp>
#include <hm3/solver/fv/models/euler/equation_of_state.hpp>
#include <hm3/solver/fv/models/euler/indices.hpp>
#include <hm3/solver/fv/models/euler/state.hpp>
#include <hm3/utility/matrix.hpp>

namespace hm3::solver::fv::euler {

template <dim_t Ad>
struct cv_base : geometry::with_ambient_dimension<Ad>,
                 equation_of_state,
                 indices<Ad> {
  using equation_of_state::mach_number;
  using equation_of_state::pressure;
  using equation_of_state::speed_of_sound;
  using equation_of_state::temperature;

 private:
  using i = indices<Ad>;
  using d = geometry::with_ambient_dimension<Ad>;

 public:
  using vars = num_a<i::nvars()>;

  /// \name Accessors
  ///@{
  /// Density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho(V&& v) noexcept {
    return v(i::rho());
  }

  /// Momentum densities
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_u(V&& v) noexcept {
    return v.template head<d::ambient_dimension()>();
  }

  /// Momentum density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_u(V&& v, dim_t d) noexcept {
    return v(i::rho_u(d));
  }

  /// Energy density
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_E(V&& v) noexcept {
    return v(i::rho_E());
  }

  ///@}  Accessors

  /// Velocity
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u(V&& v, dim_t d) noexcept {
    return v(i::rho_u(d)) / v(i::rho());
  }

  /// Velocities
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u(V&& v) noexcept {
    return v.template head<d::ambient_dimension()>() / v(i::rho());
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u_mag2(V&& v) noexcept {
    return u(v).squaredNorm();
  }

  /// L2-Norm of the velocity squared ||u||_{L2}^{2}
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) rho_u_mag2(V&& v) noexcept {
    return rho_u(v).squaredNorm();
  }

  /// L2-Norm of the velocity ||u||_{L2}
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr decltype(auto) u_mag(V&& v) noexcept {
    return u(v).norm();
  }

  /// Pressure
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t p(V&& v, num_t gamma_m1) noexcept {
    return pressure(gamma_m1, rho_u_mag2(v), rho(v), rho_E(v));
  }

  /// Inplace conversion to primitive variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr void to_pv_ip(V&& v, num_t gamma_m1) noexcept {
    num_t rho_umag2 = rho_u_mag2(v);
    rho_u(v) /= rho(v);
    rho_E(v) = pressure(gamma_m1, std::move(rho_umag2), rho(v), rho_E(v));
  }

  /// Conversion to primitive variables
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr vars to_pv(V&& v, num_t gamma_m1) noexcept {
    vars tmp;
    rho(tmp)   = rho(v);
    rho_u(tmp) = rho_u(v) / rho(v);
    rho_E(tmp) = p(v, gamma_m1);
    return tmp;
  }

  /// Inplace flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, typename F,
            CONCEPT_REQUIRES_(!rvref<V&&> and !rvref<F&&>)>
  static constexpr void flux_ip(V&& v, F&& f, dim_t d,
                                num_t gamma_m1) noexcept {
    const num_t p_v = p(v, gamma_m1);
    const num_t u_d = u(v, d);
    rho(f)          = rho_u(v, d);
    rho_u(f)        = rho_u(v) * u_d;
    rho_u(f, d) += p_v;
    rho_E(f) = u_d * (rho_E(v) + p_v);
  }

  /// Flux across a surface whose normal points in the d-th spatial
  /// dimension
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr vars flux(V&& v, dim_t d, num_t gamma_m1) noexcept {
    vars f = vars::Zero();
    flux_ip(std::forward<V>(v), f, d, gamma_m1);
    return f;
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t temperature(V&& v, num_t gamma,
                                     num_t gamma_m1) noexcept {
    return temperature(gamma, rho(v), p(v, gamma_m1));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t speed_of_sound(V&& v, num_t gamma,
                                        num_t gamma_m1) noexcept {
    return speed_of_sound(gamma, rho(v), p(v, gamma_m1));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t mach_number(V&& v, dim_t d, num_t gamma,
                                     num_t gamma_m1) noexcept {
    return mach_number(std::abs(u(v)(d)), speed_of_sound(v, gamma, gamma_m1));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t mach_number(V&& v, num_t gamma,
                                     num_t gamma_m1) noexcept {
    return mach_number(u_mag(v), speed_of_sound(v, gamma, gamma_m1));
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  static constexpr num_t max_wave_speed(V&& v, dim_t d, num_t gamma,
                                        num_t gamma_m1) noexcept {
    return std::abs(u(v, d)) + speed_of_sound(v, gamma, gamma_m1);
  }
};

template <dim_t Ad>
struct cv : cv_base<Ad>, state {
  using b = cv_base<Ad>;
  using state::gamma;
  using state::gamma_m1;
  using cv_base<Ad>::mach_number;
  using typename cv_base<Ad>::vars;

  cv(state s) : state{std::move(s)} {}

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t p(V&& v) const noexcept {
    return b::p(std::forward<V>(v), gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr auto to_pv(V&& v) const noexcept {
    return b::to_pv(std::forward<V>(v), gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr void to_pv_ip(V&& v) const noexcept {
    b::to_pv_ip(v, gamma_m1);
  }

  template <typename V, typename F,
            CONCEPT_REQUIRES_(!rvref<V&&> and !rvref<F&&>)>
  constexpr void flux_ip(V&& v, F&& f, dim_t d) const noexcept {
    b::flux_ip(std::forward<V>(v), std::forward<F>(f), d, gamma_m1);
  }
  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr auto flux(V&& v, dim_t d) const noexcept {
    return b::flux(std::forward<V>(v), d, gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t temperature(V&& v) const noexcept {
    return b::temperature(std::forward<V>(v), gamma, gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t speed_of_sound(V&& v) const noexcept {
    return b::speed_of_sound(std::forward<V>(v), gamma, gamma_m1);
  }

  template <
   typename V,
   CONCEPT_REQUIRES_(!rvref<V&&> and !std::is_floating_point<uncvref_t<V>>{})>
  constexpr num_t mach_number(V&& v, dim_t d) const noexcept {
    return b::mach_number(std::forward<V>(v), d, gamma, gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t mach_number(V&& v) const noexcept {
    return b::mach_number(std::forward<V>(v), gamma, gamma_m1);
  }

  template <typename V, CONCEPT_REQUIRES_(!rvref<V&&>)>
  constexpr num_t max_wave_speed(V&& v, dim_t d) const noexcept {
    return b::max_wave_speed(std::forward<V>(v), d, gamma, gamma_m1);
  }
};

}  // namespace hm3::solver::fv::euler
