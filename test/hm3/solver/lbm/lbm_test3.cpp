#ifdef FIXED

#include <hm3/geometry/intersect.hpp>
#include <hm3/geometry/sd.hpp>
#include <hm3/grid/generation/uniform.hpp>
#include <hm3/io/session.hpp>
#include <hm3/solver/lbm/lattice/d2q9.hpp>
#include <hm3/solver/lbm/lbm.hpp>
#include <hm3/solver/lbm/ns.hpp>
#include <hm3/solver/lbm/state.hpp>
#include <hm3/solver/lbm/vtk.hpp>
#include <hm3/solver/utility.hpp>

using namespace hm3;
using namespace solver;

// struct BoundaryConditions {
//   using sph_t = geometry::sd::fixed_sphere<2>;
//   sph_t sph;

//   num_t density;
//   num_t accel;
//   num_t t_1;
//   num_t t_2;

//   BoundaryConditions(num_t rho, num_t a)
//    : sph(geometry::point<2>::constant(0.5), 0.05), density(rho), accel(a) {
//     t_1 = density * accel / 9.;
//     t_2 = density * accel / 36.;
//   }

//   template <typename Geom> bool is_cut(Geom&& g) const noexcept {
//     return geometry::is_intersected(g, (*this));
//   }

//   template <typename Geom> bool is_inside(Geom&& g) const noexcept {
//     return (*this)(centroid(g)) > 0. and !is_cut(g);
//   }

//   template <typename X> auto operator()(X&& x) const noexcept {
//     return sph(std::forward<X>(x));
//   }

//   // inflow
//   template <typename State> void redistribute(State&& s) {
//     for (auto&& b : s.blocks()) {
//       b.for_each_internal([&](auto c) {
//         if (c.x[0] == 2 and c.x[1] != 2 and c.x[1] != 101
//             and (b.nodes0(c, 3) - t_1 > 0. || b.nodes0(c, 6) - t_2 > 0.
//                  || b.nodes0(c, 7) - t_2 > 0.)) {
//           b.nodes0(c, 1) += t_1;
//           b.nodes0(c, 5) += t_2;
//           b.nodes0(c, 8) += t_2;

//           b.nodes0(c, 3) -= t_1;
//           b.nodes0(c, 6) -= t_2;
//           b.nodes0(c, 7) -= t_2;
//         }
//       });
//     }
//   }

//   template <typename State> void periodic(State&& s) {
//     using l = typename std::decay_t<State>::lattice_t;
//     for (auto&& b : s.blocks()) {
//       // periodic on inflow/outflow
//       auto offset = b.internal_cells_per_length();
//       b.for_each_internal([&](auto c) {
//         if (c.x[0] == 2) {
//           RANGES_FOR (auto&& d, l::node_ids()) {
//             b.nodes1(c, d) = b.nodes0(c.idx + offset - 1, d);
//           }
//         }
//         if (c.x[0] == 101) {
//           RANGES_FOR (auto&& d, l::node_ids()) {
//             b.nodes1(c, d) = b.nodes0(c.idx - (offset - 1), d);
//           }
//         }
//       });
//     }
//   }

//   template <typename State> void bounce_back(State&& s) {
//     using l = typename std::decay_t<State>::lattice_t;
//     for (auto&& b : s.blocks()) {
//       // bounce back on solid boundaries
//       b.for_each_internal([&](auto c) {
//         if (c.x[0] == 0 || c.x[0] == 1 || c.x[1] == 2 || c.x[1] == 101
//             || c.x[1] == 102 || c.x[1] == 103 || (*this)(b.center(c)) < 0.) {
//           RANGES_FOR (auto&& d, l::node_ids()) {
//             b.nodes0(c, d) = b.nodes1(c, l::opposite_dist(d));
//           }
//         }
//       });
//     }
//   }
// };

using namespace solver;

// struct obstacle {
//   using sph_t = geometry::sd::fixed_sphere<2>;
//   sph_t sph(geometry::point<2>::constant(0.5), 0.05);
//   template <typename X> bool operator()(X&& x) {
//     return x(0) < 0. || x(0) > 1. || x(1) < 0. || x(1) > 1. || sph(x) < 0.;
//   }
// };

struct poiseuille_profile {
  num_t radius              = 0.5;
  num_t dpdx                = -1.0;
  num_t kinematic_viscosity = 1.0;  // mu
  num_t y_center            = 0.5;
  num_t p_outflow           = 0.;
  num_t length              = 1.0;

  using vel_t = std::array<num_t, 2>;
  vel_t velocity(num_t y) const noexcept {
    vel_t us;
    num_t y_rel = y - y_center;
    us[0]       = -1. / (4. * kinematic_viscosity) * dpdx
            * (std::pow(radius, 2) - std::pow(y_rel, 2));
    us[1] = 0.;
    return us;
  }
  num_t p(num_t x) { return p_outflow + dpdx * (x - length); }
};

// struct inflow_bc {
//   template <typename X> bool contains(X&& x) const noexcept {
//     return x(0) < 0.;
//   }

//   template <typename State, typename Block, typename Idx>
//   auto value(State&& s, Block&& b, Idx idx) const noexcept {
//     num_t rho = 1.0;
//     auto x    = b.center(idx);
//     auto us = poiseuille_velocity_profile(x(1));
//     auto eq = s.physics.equilibrium_distribution(rho, us);
//     return eq;
//   }
// };

// struct outflow_bc {
//   template <typename X> bool contains(X&& x) const noexcept {
//     return x(0) > 1.;
//   }

//   template <typename State, typename Block, typename Idx>
//   auto value(State&& s, Block&& b, Idx&& idx) const noexcept {
//     num_t rho = 1.0;
//     auto x    = b.center(idx);
//     auto us = poiseuille_velocity_profile(x(1));
//     auto eq = s.physics.equilibrium_distribution(rho, us);
//     return eq;
//   }
// };

struct solid {
  template <typename State, typename Block, typename Idx>
  bool operator()(State&& s, Block&& b, Idx&& idx) const noexcept {
    // using sph_t = geometry::sd::fixed_sphere<2>;
    // sph_t sph(geometry::point<2>::constant(0.5), 0.05);
    auto x    = b.center(idx);
    num_t dx2 = 3. / 2. * 1.0 / 128.;
    return (x(0) < 0. + dx2 or x(0) > 1. - dx2 or x(1) < 0. + dx2);
  }
};

// struct halo_bc {
//   template <typename State, typename Block, typename Idx>
//   auto value(State&& s, Block&& b, Idx&& idx) const noexcept {
//     auto x       = b.center(idx);
//     auto profile = poiseuille_profile{};
//     auto us = profile.velocity(x(1));
//     num_t rho = s.physics.rho(profile.p(x(0)));
//     auto eq = s.physics.equilibrium_distribution(rho, us);
//     return eq;
//   }
// };

template <typename State, typename Obstacle>
void bounce_back(State&& s, Obstacle&& obstacle) {
  for (auto&& b : s.blocks()) {
    b.for_each_internal([&](auto cidx) {
      if (obstacle(s, b, cidx)) {  // bounce back
        RANGES_FOR (auto&& d, s.physics.all()) {
          b.nodes0(cidx, d) = b.nodes1(cidx, s.physics.opposite(d));
        }
      }
    });
  }
}

struct solid_boundary {
  template <typename A, typename B, typename C>
  bool operator()(A&& a, B&& b, C&& c) const noexcept {
    return solid{}(a, b, c);
  }
  template <typename S, typename B, typename C>
  void apply(S&& s, B&& b, C&& c) {
    RANGES_FOR (auto&& d, s.physics.all()) {
      b.nodes0(c, d) = b.nodes1(c, s.physics.opposite(d));
    }
  }
};

struct slip_boundary {
  template <typename S, typename B, typename C>
  bool operator()(S&& s, B&& b, C&& c) const noexcept {
    return b.center(c)(1) > 1. - 1. / 128. and !solid{}(s, b, c);
  }
  template <typename S, typename B, typename C>
  void apply(S&& s, B&& b, C&& c) {
    RANGES_FOR (auto&& d, s.physics.all()) {
      b.nodes0(c, d) = b.nodes1(c, s.physics.opposite(d));
    }
  }
};

template <typename State, typename Obstacle>
void exchange_periodic(State&& s, Obstacle&& obstacle) {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto cidx) {
      if (!obstacle(s, b, cidx)) {  // periodic
        if (cidx.x[0] == 1) {
          auto p_n = b.at(cidx, 0, 100);
          RANGES_FOR (auto&& d, s.physics.all()) {
            b.nodes0(cidx, d) = b.nodes0(p_n, d);
            // b.nodes1(cidx, d) = b.nodes1(101, d);
          }
        }
        if (cidx.x[0] == 102) {
          auto p_n = b.at(cidx, 0, -100);
          RANGES_FOR (auto&& d, s.physics.all()) {
            b.nodes0(cidx, d) = b.nodes0(p_n, d);
            // b.nodes1(cidx, d) = b.nodes1(2, d);
          }
        }
      }
    });
  }
}

template <typename State, typename Obstacle>
void redistribute(State&& s, Obstacle&& obstacle) {
  num_t density = 0.1;
  num_t accel   = 0.015;  // 0.015;

  const num_t t_1 = density * accel / 9.;
  const num_t t_2 = density * accel / 36.;
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto cidx) {
      if (!obstacle(s, b, cidx) and cidx.x[0] == 2
          and b.nodes0(cidx, s.physics.W) - t_1 > 0.
          and b.nodes0(cidx, s.physics.NW) - t_2 > 0.
          and b.nodes0(cidx, s.physics.SW) - t_2 > 0.) {
        b.nodes0(cidx, s.physics.E) += t_1;
        b.nodes0(cidx, s.physics.NE) += t_2;
        b.nodes0(cidx, s.physics.SE) += t_2;

        b.nodes0(cidx, s.physics.W) -= t_1;
        b.nodes0(cidx, s.physics.NW) -= t_2;
        b.nodes0(cidx, s.physics.SW) -= t_2;
      }
    });
  }
}

template <typename State, typename Obstacle>
void fd_slip(State&& s, Obstacle&& obstacle) {
  num_t u   = 1.0;  // * s.physics.mach * s.physics.cs();
  num_t v   = 0.0;
  num_t rho = 1.0;

  for (auto&& b : s.blocks()) {
    b.for_each([&](auto cidx) {
      if (obstacle(s, b, cidx) or b.center(cidx)(1) < 1.0 - 1.0 / 128.) {
        return;
      }
      auto f_eq = s.physics.equilibrium_distribution(rho, {{u, v}});
      RANGES_FOR (auto&& d, s.physics.all()) { b.nodes0(cidx, d) = f_eq[d]; }
    });
  }

  // for (auto&& b : s.blocks()) {
  //   b.for_each([&](auto cidx) {
  // if (!obstacle(s, b, cidx)) {
  // auto ic     = b.closest_internal_cell(cidx);
  // auto ic_n   = b.at(ic, 1, -1);
  // auto x_hc   = b.center(cidx);
  // auto x_ic   = b.center(ic);
  // auto x_ic_n = b.center(ic_n);

  // auto cell_length = b.cell_length();

  // num_t dy = cell_length / 2.;

  // auto line_ic_bndry = geometry::line<1>::through(
  //  geometry::point<1>{x_ic(1)}, geometry::point<1>(x_ic(1) + dy));

  // std::array<num_t, 2> values;
  // num_t u_ic = s.physics.u(&b.nodes0(ic, 0));
  // values[0] = u_ic;
  // values[1] = u;

  // num_t dy_gc = (x_hc(1) - x_ic(1)) / dy;

  // num_t u_gc = geometry::interpolate(line_ic_bndry, values, dy_gc);

  // auto line2 =
  // geometry::line<1>::through(geometry::point<1>{x_ic_n(1)},
  //                                         geometry::point<1>(x_ic(1)));

  // num_t dy_ic = cell_length;

  // dy = (x_hc(1) - x_ic_n(1)) / dy_ic;

  // // values[0] = s.physics.v(&b.nodes0(ic_n, 0));
  // // values[1] = s.physics.v(&b.nodes0(ic, 0));
  // num_t v_ic = s.physics.v(&b.nodes0(ic, 0));
  // num_t v_gc = 0;  // geometry::interpolate(line2, values, dy);

  // num_t rho_ic = s.physics.rho(&b.nodes0(ic, 0));
  // values[0] = s.physics.rho(&b.nodes0(ic_n, 0));
  // values[1]    = rho_ic;
  // num_t rho_gc = geometry::interpolate(line2, values, dy);

  // // std::cout << "cidx: " << cidx.idx << " ic.idx: " << ic.idx
  // //           << " ic_n.idx:" << ic_n.idx << " u_ic: " << u_ic
  // //           << " v_ic: " << v_ic << " rho_ic:" << rho_ic
  // //           << " u_gc: " << u_gc << " v_gc: " << v_gc
  // //           << " rho_gc:" << rho_gc << " u_t:" << u << std::endl;

  // auto f_eq = s.physics.equilibrium_distribution(rho_gc, {{u_gc,
  // v_gc}});
  // auto f_eq = s.physics.equilibrium_distribution(rho, {{u, v}});
  // RANGES_FOR (auto&& d, s.physics.all()) { b.nodes0(cidx, d) = f_eq[d]; }
  //}
  //   });
  // }
}

template <typename State, typename Obstacle>
void slip0(State&& s, Obstacle&& obstacle) {
  num_t u = 1.0 * s.physics.mach * s.physics.cs();
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto cidx) {
      if (
       //! obstacle(s, b, cidx) and
       cidx.x[1] == 101) {
        auto dists = &b.nodes0(cidx, 0);
        num_t rho  = s.physics.rho(dists);
        num_t v    = 0.0;  // s.physics.v(dists);
        auto f_eq = s.physics.equilibrium_distribution(rho, {{u, v}});
        RANGES_FOR (auto&& d, s.physics.all()) { b.nodes0(cidx, d) = f_eq[d]; }
      }
    });
  }
}

template <typename State, typename Obstacle>
void slip1(State&& s, Obstacle&& obstacle) {
  num_t u = 1.0 * s.physics.mach * s.physics.cs();
  for (auto&& b : s.blocks()) {
    b.for_each([&](auto cidx) {
      if (
       //! obstacle(s, b, cidx) and
       cidx.x[1] == 1) {
        auto dists = &b.nodes1(cidx, 0);
        num_t rho  = s.physics.rho(dists);
        num_t v    = 0.0;  // s.physics.v(dists);
        auto f_eq = s.physics.equilibrium_distribution(rho, {{u, v}});
        RANGES_FOR (auto&& d, s.physics.all()) { b.nodes1(cidx, d) = f_eq[d]; }
      }
    });
  }
}

template <typename State> void set_halos(State&& s, num_t v) {
  for (auto&& b : s.blocks()) {
    b.for_each_halo([&](auto cidx) {
      RANGES_FOR (auto&& d, s.physics.all()) {
        b.nodes0(cidx, d) = v;
        b.nodes1(cidx, d) = v;
      }
    });
  }
}

int main() {
  mpi::env env;
  auto comm = env.world();

  // Initialize I/O session
  io::session::remove("lbm_d2q9", comm);
  io::session s(io::create, "lbm_d2q9", comm);

  // Grid parameters
  constexpr uint_t nd       = 2;
  constexpr uint_t no_grids = 1;
  auto min_grid_level       = 0_u;
  auto max_grid_level       = min_grid_level + 1;
  auto node_capacity
   = tree::node_idx{tree::no_nodes_until_uniform_level(nd, max_grid_level)};
  auto bounding_box = geometry::square<nd>::unit();

  // Create the grid
  grid::mhc<nd> g(s, node_capacity, no_grids, bounding_box);

  // Refine the grid up to the minimum leaf node level
  grid::generation::uniform(g, min_grid_level);

  auto solver_block_capacity
   = grid::grid_node_idx{tree::no_nodes_at_uniform_level(nd, max_grid_level)};

  using lattice_t = solver::lbm::lattice::d2q9;
  using physics_t = solver::lbm::ns::physics<lattice_t>;
  using state_t   = solver::lbm::state<physics_t>;

  physics_t physics;
  state_t lbm_s{g, 0_g, solver_block_capacity, physics};

  // num_t density = 0.1;
  // num_t accel   = 0.015;
  // BoundaryConditions bcs(density, accel);

  /// Initial lbm solver grid:
  solver::initialize_grid(g, lbm_s, [&](auto&& n) { return g.is_leaf(n); });

  /// Initial condition  // TODO: this is constant for now
  auto ic = [&](auto) {
    // auto profile = poiseuille_profile{};
    // auto v = profile.velocity(x(1));
    // auto rho = physics.rho(profile.p(x(0)));
    // physics_t::variables var{rho, v};
    physics_t::variables var{1.0, {{0.0, 0.0}}};
    return var;
  };

  lbm::initialize_variables_to_equilibrium(lbm_s, ic);

  lbm::vtk::serialize(lbm_s, solid{}, "i_lbm2_res", 0, 0_gn);

  num_t min_cell_length = std::numeric_limits<num_t>::max();
  for (auto&& b : lbm_s.blocks()) {
    min_cell_length = std::min(min_cell_length, b.cell_length());
  }
  lbm_s.physics.min_cell_length = min_cell_length;

  lbm_s.physics.mach = 0.1;

  int c = 0;
  for (;;) {
    set_halos(lbm_s, 0.);
    // redistribute(lbm_s, solid{});
    fd_slip(lbm_s, solid{});
    lbm::propagate(lbm_s);
    lbm::propagate_slip(lbm_s, solid{});
    // exchange_periodic(lbm_s, solid{});
    //  lbm::propagate_periodic_x(lbm_s, solid{});
    // slip1(lbm_s, solid{});
    bounce_back(lbm_s, solid{});
    num_t omega = lbm_s.physics.omega(min_cell_length);
    lbm::collide(lbm_s, solid{}, 1.8);
    set_halos(lbm_s, 1.);
    ++c;
    if (c % 100 == 0) {
      lbm::vtk::serialize(lbm_s, solid{}, "lbm2_res", c, 0_gn);
    }
    std::cout << c << std::endl;
    if (c == 1000) { break; }
  }

  // lbm::advance(lbm_s, bcs, 100);
  return 0;
}
#else
int main() { return 0; }
#endif