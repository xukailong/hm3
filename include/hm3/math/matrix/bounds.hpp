#pragma once
/// \file
///
/// Matrix bounds. The template parameters specify the number of rows and cols
/// respectively, and `dynamic` is used to specify that the bounds is not known
/// until run-time.
#include <hm3/math/matrix/traits.hpp>
#include <hm3/types.hpp>
#include <hm3/utility/assert.hpp>

namespace hm3::dense {

template <int_t NoRows, int_t NoCols>
struct bounds {
  static constexpr auto no_rows() noexcept {
    return static_cast<uint_t>(NoRows);
  }
  static constexpr auto no_cols() noexcept {
    return static_cast<uint_t>(NoCols);
  }
  static constexpr uint_t size() noexcept { return no_rows() * no_cols(); }

  constexpr bounds()              = default;
  constexpr bounds(bounds const&) = default;
  constexpr bounds(bounds&&)      = default;
  constexpr bounds& operator=(bounds const&) = default;
  constexpr bounds& operator=(bounds&&) = default;
  ~bounds()                             = default;

  constexpr bounds(int_t rs, int_t cs) {
    HM3_ASSERT(rs == no_rows(),
               "compile_time bound {} mismatch run-time bound {}", rs,
               no_rows());
    HM3_ASSERT(cs == no_cols(),
               "compile_time bound {} mismatch run-time bound {}", rs,
               no_rows());
  }
};

template <int_t NoCols>
struct bounds<dynamic, NoCols> {
  uint_t no_rows_ = 0;
  constexpr auto no_rows() const noexcept {
    return static_cast<uint_t>(no_rows_);
  }
  static constexpr auto no_cols() noexcept { return NoCols; }
  constexpr uint_t size() const noexcept { return no_rows() * no_cols(); }

  constexpr bounds()              = default;
  constexpr bounds(bounds const&) = default;
  constexpr bounds(bounds&&)      = default;
  constexpr bounds& operator=(bounds const&) = default;
  constexpr bounds& operator=(bounds&&) = default;
  ~bounds()                             = default;

  constexpr bounds(int_t nrows, int_t ncols) : no_rows_(nrows) {
    HM3_ASSERT(
     ncols == NoCols,
     "#of cols mismatch between constructor {} and template parameter {}",
     ncols, NoCols);
  }
};

template <int_t NoRows>
struct bounds<NoRows, dynamic> {
  uint_t no_cols_ = 0;
  static constexpr auto no_rows() noexcept {
    return static_cast<uint_t>(NoRows);
  }
  constexpr auto no_cols() const noexcept { return no_cols_; }
  constexpr uint_t size() const noexcept { return no_rows() * no_cols(); }

  constexpr bounds()              = default;
  constexpr bounds(bounds const&) = default;
  constexpr bounds(bounds&&)      = default;
  constexpr bounds& operator=(bounds const&) = default;
  constexpr bounds& operator=(bounds&&) = default;
  ~bounds()                             = default;

  constexpr bounds(int_t nrows, int_t ncols) : no_cols_(ncols) {
    HM3_ASSERT(
     nrows == NoRows,
     "#of rows mismatch between constructor {} and template parameter {}",
     nrows, NoRows);
  }
};

template <>
struct bounds<dynamic, dynamic> {
  uint_t no_rows_ = 0;
  uint_t no_cols_ = 0;
  constexpr auto no_rows() const noexcept { return no_rows_; }
  constexpr auto no_cols() const noexcept { return no_cols_; }
  constexpr uint_t size() const noexcept { return no_rows() * no_cols(); }

  constexpr bounds()              = default;
  constexpr bounds(bounds const&) = default;
  constexpr bounds(bounds&&)      = default;
  constexpr bounds& operator=(bounds const&) = default;
  constexpr bounds& operator=(bounds&&) = default;
  // TODO: ~bounds() = default;

  constexpr bounds(int_t nrows, int_t ncols)
   : no_rows_(nrows), no_cols_(ncols) {}
};

template <int_t MaxRows, int_t MaxCols>
struct max_bounds {
  static constexpr auto max_no_rows() noexcept {
    return static_cast<uint_t>(MaxRows);
  }
  static constexpr auto max_no_cols() noexcept {
    return static_cast<uint_t>(MaxCols);
  }
  static constexpr auto max_size() noexcept {
    return max_no_rows() * max_no_cols();
  }

  constexpr max_bounds()                  = default;
  constexpr max_bounds(max_bounds const&) = default;
  constexpr max_bounds(max_bounds&&)      = default;
  constexpr max_bounds& operator=(max_bounds const&) = default;
  constexpr max_bounds& operator=(max_bounds&&) = default;
  ~max_bounds()                                 = default;

  constexpr max_bounds(int_t, int_t) {}
};

template <int_t MaxCols>
struct max_bounds<dynamic, MaxCols> {
  uint_t max_no_rows_ = 0;
  constexpr auto max_no_rows() const noexcept {
    return static_cast<uint_t>(max_no_rows_);
  }
  static constexpr auto max_no_cols() noexcept { return MaxCols; }
  constexpr auto max_size() const noexcept {
    return max_no_rows() * max_no_cols();
  }

  constexpr max_bounds()                  = default;
  constexpr max_bounds(max_bounds const&) = default;
  constexpr max_bounds(max_bounds&&)      = default;
  constexpr max_bounds& operator=(max_bounds const&) = default;
  constexpr max_bounds& operator=(max_bounds&&) = default;
  ~max_bounds()                                 = default;

  constexpr max_bounds(int_t maxrows, int_t maxcols) : max_no_rows_(maxrows) {
    HM3_ASSERT(
     maxcols == MaxCols,
     "#of max cols mismatch between constructor {} and template parameter {}",
     maxcols, MaxCols);
  }
};

template <int_t MaxRows>
struct max_bounds<MaxRows, dynamic> {
  uint_t max_no_cols_ = 0;
  static constexpr auto max_no_rows() noexcept {
    return static_cast<uint_t>(MaxRows);
  }
  constexpr auto max_no_cols() const noexcept {
    return static_cast<uint_t>(max_no_cols_);
  }
  constexpr auto max_size() const noexcept {
    return max_no_rows() * max_no_cols();
  }

  constexpr max_bounds()                  = default;
  constexpr max_bounds(max_bounds const&) = default;
  constexpr max_bounds(max_bounds&&)      = default;
  constexpr max_bounds& operator=(max_bounds const&) = default;
  constexpr max_bounds& operator=(max_bounds&&) = default;
  ~max_bounds()                                 = default;

  constexpr max_bounds(int_t maxrows, int_t maxcols) : max_no_cols_(maxcols) {
    HM3_ASSERT(
     maxrows == MaxRows,
     "#of max rows mismatch between constructor {} and template parameter {}",
     maxrows, MaxRows);
  }
};

template <>
struct max_bounds<dynamic, dynamic> {
  uint_t max_no_rows_ = 0;
  uint_t max_no_cols_ = 0;
  constexpr auto max_no_rows() const noexcept { return max_no_rows_; }
  constexpr auto max_no_cols() const noexcept { return max_no_cols_; }
  constexpr auto max_size() const noexcept {
    return max_no_rows() * max_no_cols();
  }

  constexpr max_bounds()                  = default;
  constexpr max_bounds(max_bounds const&) = default;
  constexpr max_bounds(max_bounds&&)      = default;
  constexpr max_bounds& operator=(max_bounds const&) = default;
  constexpr max_bounds& operator=(max_bounds&&) = default;
  //  ~max_bounds()                                 = default;

  constexpr max_bounds(int_t maxrows, int_t maxcols)
   : max_no_rows_(maxrows), max_no_cols_(maxcols) {}
};

}  // namespace hm3::dense