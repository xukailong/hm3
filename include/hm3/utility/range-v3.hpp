#pragma once
/// \file
///
/// Includes the range-v3 library
#include <hm3/utility/config/compiler.hpp>

#define RANGES_CXX_LIB_IS_FINAL 0

#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include <range/v3/all.hpp>
#ifdef HM3_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
#include <hm3/utility/boost/vector.hpp>

/// Extensions:
namespace ranges {

template <typename T, class Alloc>
struct is_view<boost::container::vector<T, Alloc>> : std::false_type {};

}  // namespace ranges
