#pragma once
/// \file
///
/// Workarounds std::math functions that are not constexpr
#include <hm3/math/integer.hpp>
#include <hm3/utility/assert.hpp>
#include <type_traits>

namespace hm3::math {

/// Is a floating point number NaN?
///
/// NaNs never compare equal to themselves
///
/// \note Workaround until std::isnan becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_nan(Float x) noexcept {
  return __builtin_isnan(x);  // NOLINT(cppcoreguidelines-pro-type-vararg)
}

/// Is a floating point number finite?
///
/// \note Workaround until std::isfinite becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_finite(Float x) noexcept {
  return __builtin_isfinite(x);  // NOLINT(cppcoreguidelines-pro-type-vararg)
}

/// Is a floating point number infinite?
///
/// \note Workaround until std::isinf becomes constexpr
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr bool is_infinite(Float x) noexcept {
  return __builtin_isinf(x);  // NOLINT(cppcoreguidelines-pro-type-vararg)
}

/// Integer floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t ifloor(Float x) noexcept {
  HM3_ASSERT(is_finite(x), "floor of not finite number {}", x);
  int64_t i = x;
  return i - (x < i);
}

/// Integer ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr int64_t iceil(Float x) noexcept {
  HM3_ASSERT(is_finite(x), "ceil of not finite number {}", x);
  int64_t i = x;
  return i + (x > i);
}

/// Constexpr version of cmath std::floor
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float floor(Float x) noexcept {
  return ifloor(x);
}

/// Constexpr version of cmath std::ceil
template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float ceil(Float x) noexcept {
  return iceil(x);
}

template <typename T, CONCEPT_REQUIRES_(std::is_lvalue_reference<T&&>{})>
constexpr T const& max(T&& a, T&& b) noexcept {
  return a > b ? a : b;
}

template <typename T, CONCEPT_REQUIRES_(std::is_rvalue_reference<T&&>{})>
constexpr T max(T&& a, T&& b) noexcept {
  return a > b ? std::move(a)   // NOLINT(misc-move-forwarding-reference)
               : std::move(b);  // NOLINT(misc-move-forwarding-reference)
}

template <typename T, typename U, typename UT = uncvref_t<T>,
          typename UU = uncvref_t<U>,
          CONCEPT_REQUIRES_(Same<UT, UU>{}
                            and (std::is_rvalue_reference<T&&>{}
                                 or std::is_rvalue_reference<U&&>{}))>
constexpr T max(T&& a, U&& b) noexcept {
  return a > b ? T(std::forward<T>(a)) : T(std::forward<U>(b));
}

template <typename Float, CONCEPT_REQUIRES_(std::is_floating_point<Float>{})>
constexpr Float abs(Float f) noexcept {
  return f < Float{0.} ? -f : f;
}

}  // namespace hm3::math
