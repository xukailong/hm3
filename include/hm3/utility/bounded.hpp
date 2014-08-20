#pragma once
/// \file
///
/// Bounded integer type
#include <hm3/utility/assert.hpp>
#include <hm3/utility/range.hpp>

namespace hm3 {

/// A tagged type with values in range [from, to)
template <typename T, T from, T to, typename Tag = void> struct bounded {
  using this_t         = bounded<T, from, to, Tag>;
  using value_type     = T;
  using tag            = Tag;
  using reference_type = value_type&;
  value_type value;

  constexpr bounded(T v = from) : value{v} {
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
  }

  constexpr value_type operator*() const noexcept {
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return value;
  }
  constexpr reference_type operator*() noexcept {
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return value;
  }
  constexpr explicit operator bool() const noexcept {
    return value >= from and value < to;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<T>{})
  static constexpr auto rng() noexcept {
    return view::iota(from, to)
           | view::transform([](T const& i) { return bounded{i}; });
  }

  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator==(this_t const& o) const noexcept {
    return *(*this) == *o;
  }
  CONCEPT_REQUIRES(EqualityComparable<value_type>())
  constexpr bool operator!=(this_t const& o) const noexcept {
    return *(*this) != *o;
  }

  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<(this_t const& o) const noexcept {
    return *(*this) < *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>(this_t const& o) const noexcept {
    return *(*this) > *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator<=(this_t const& o) const noexcept {
    return *(*this) <= *o;
  }
  CONCEPT_REQUIRES(WeaklyOrdered<value_type>())
  constexpr bool operator>=(this_t const& o) const noexcept {
    return *(*this) >= *o;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  this_t& operator++() noexcept {
    ++value;
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return (*this);
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  this_t operator++(int) noexcept {
    this_t tmp(*this);
    ++value;
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return tmp;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  friend this_t operator+(this_t const& a, this_t const& b) noexcept {
    this_t v{(*a) + (*b)};
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  this_t& operator+=(this_t const& other) noexcept {
    value += *other;
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return *this;
  }

  CONCEPT_REQUIRES(RandomAccessIncrementable<value_type>())
  this_t& operator-=(this_t const& other) noexcept {
    value -= *other;
    HM3_ASSERT(value >= from and value < to, "value is out-of-bounds [{}, {})",
               from, to);
    return *this;
  }

  template <typename OStream>
  friend constexpr OStream& operator<<(OStream& os, this_t const& a) {
    os << *a;
    return os;
  };
};

}  // namespace hm3
