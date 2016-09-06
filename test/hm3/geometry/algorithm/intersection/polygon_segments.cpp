#include <hm3/geometry/algorithm/intersection/polygon_segments.hpp>
#include <hm3/geometry/io/vtk.hpp>
#include <hm3/utility/test.hpp>

using namespace hm3;

int main() {
  using sp_t = geometry::small_polygon<2, 10>;
  using p_t  = geometry::point<2>;
  using s_t  = geometry::segment<2>;

  auto p0 = p_t{0.0, 0.0};
  auto p1 = p_t{1.0, 0.0};
  auto p2 = p_t{1.0, 1.0};
  auto p3 = p_t{0.0, 1.0};

  sp_t p({p0, p1, p2, p3});

  // test 1:
  s_t s0(p_t{0.5, 1.5}, p_t{0.5, 0.5});
  s_t s1(p_t{0.5, 0.5}, p_t{1.5, 0.5});
  auto rng0 = {s0, s1};
  auto r0   = {s_t(p_t{0.5, 1.0}, p_t{0.5, 0.5}),  //
             s_t(p_t{0.5, 0.5}, p_t{1.0, 0.5})};

  // test 2:
  s_t s2(p_t{-0.25, 0.5}, p_t{0.25, 0.75});
  s_t s3(p_t{0.25, 0.75}, p_t{0.75, 0.25});
  s_t s4(p_t{0.75, 0.25}, p_t{1.25, 0.5});
  auto rng1 = {s2, s3, s4};
  auto r1   = {s_t(p_t{0.0, 0.625}, p_t{0.25, 0.75}), s3,  //
             s_t(p_t{0.75, 0.25}, p_t{1.0, 0.375})};

  using i_t = decltype(geometry::intersection(rng0, p));
  CHECK(geometry::intersection(rng0, p) == i_t{r0});
  CHECK(geometry::intersection(rng1, p) == i_t{r1});

#ifdef HM3_ENABLE_VTK
  geometry::io::vtk<2> o{};
  o.push_cell(p);
  o.push_range(rng0);
  o.push_range(rng1);
  o.write("polygon_segments_intersection");
#endif  // HM3_ENABLE_VTK

  return test::result();
}