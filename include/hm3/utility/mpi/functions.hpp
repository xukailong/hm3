#pragma once
/// \file
///
// MPI Free functions
#include <hm3/utility/mpi/config.hpp>
#include <hm3/utility/mpi/call.hpp>

namespace hm3 {
namespace mpi {

/// Has the MPI Environment been initialized?
inline bool initialized() {
  int result;
  HM3_MPI_CALL(MPI_Initialized(&result));
  return result != 0;
};

/// Has the MPI Environment been finalized?
inline bool finalized() {
  int result;
  HM3_MPI_CALL(MPI_Finalized(&result));
  return result != 0;
};

}  // namespace mpi
}  // namespace hm3
