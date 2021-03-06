#pragma once
/// \file
///
/// Serialization to HM3's File I/O
#include <hm3/io/session.hpp>
#include <hm3/io/client.hpp>
#include <hm3/io/file.hpp>
#include <hm3/tree/algorithm/dfs_sort.hpp>

namespace hm3 {
namespace grid {

/// Reads Grid from file \p file_name
template <typename Grid> Grid from_file(Grid const&, string const& file_name) {
  io::session s(io::restart, file_name, mpi::comm::world());
  io::client c(s, name(Grid{}) + "_" + file_name, type(Grid{}));
  auto f = c.get_file();
  auto t = from_file<Grid::dimension()>(Grid{}, f);
  if (!t.is_compact() or !tree::dfs_sort.is(t)) {
    HM3_FATAL_ERROR("fio error: cannot read non-compact or non-sorted tree");
  }
  return t;
}

/// Writes Grid \p g to file \p file_name
template <typename Grid> void to_file(Grid const& g, string const& file_name) {
  io::session s(io::create, file_name, mpi::comm::world());
  io::client c(s, name(g) + "_" + file_name, type(g));
  auto f = c.new_file();
  if (!g.is_compact() or !tree::dfs_sort.is(g)) {
    HM3_FATAL_ERROR(
     "fio error: cannot write non-compact or non-sorted tree/grid");
  }
  to_file_unwritten(f, g);
  c.write(f);
}

}  // namespace grid
}  // namespace hm3
