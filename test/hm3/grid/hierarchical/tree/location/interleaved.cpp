/// \file
///
/// Interleaved location tests
#include <hm3/grid/hierarchical/tree/location/interleaved.hpp>
#include "test.hpp"

using namespace hm3;
using namespace tree;

template struct hm3::tree::location::interleaved<1, uint32_t>;
template struct hm3::tree::location::interleaved<2, uint32_t>;
template struct hm3::tree::location::interleaved<3, uint32_t>;
template struct hm3::tree::location::interleaved<1, uint64_t>;
template struct hm3::tree::location::interleaved<2, uint64_t>;
template struct hm3::tree::location::interleaved<3, uint64_t>;

int main() {
  {  // 1D (32 bit)
    test_location<1, 31>(location::interleaved<1, uint32_t>{});
  }

  {  // 2D (32 bit)
    test_location<2, 15>(location::interleaved<2, uint32_t>{});
  }

  {  // 3D (32_bit)
    test_location<3, 9>(location::interleaved<3, uint32_t>{});
  }
  {  // 1D (64 bit)
    test_location<1, 63>(location::interleaved<1, uint64_t>{});
  }

  {  // 2D (64 bit)
    test_location<2, 31>(location::interleaved<2, uint64_t>{});
  }

  {  // 3D (64_bit)
    test_location<3, 20>(location::interleaved<3, uint64_t>{});
  }

  { test_location_2<location::interleaved>(); }

  return test::result();
}