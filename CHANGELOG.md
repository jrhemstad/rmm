# RMM 0.5.0 (Date TBD)

## New Features

 - PR #2 Added CUDA Managed Memory allocation mode
 
## Improvements
 
 - PR #12 Enable building RMM as a submodule
 - PR #13 CMake: Added CXX11ABI option and removed Travis references
 - PR #16 CMake: Added PARALLEL_LEVEL environment variable handling for GTest build parallelism (matches cuDF)
 - PR #17 Update README with v0.5 changes including Managed Memory support

## Bug Fixes

 - PR #10 Change cnmem submodule URL to use https
 - PR #15 Temporarily disable hanging AllocateTB test for managed memory

# RMM 0.4.0 (20 Dec 2018)

## New Features

- PR #1 Spun off RMM from cuDF into its own repository.

## Improvements

- CUDF PR #472 RMM: Created centralized rmm::device_vector alias and rmm::exec_policy
- CUDF PR #465 Added templated C++ API for RMM to avoid explicit cast to `void**`
 
## Bug Fixes


RMM was initially implemented as part of cuDF, so we include the relevant changelog history below. 

# cuDF 0.3.0 (23 Nov 2018)

## New Features

 - PR #336 CSV Reader string support

## Improvements
 
 - CUDF PR #333 Add Rapids Memory Manager documentation
 - CUDF PR #321 Rapids Memory Manager adds file/line location logging and convenience macros

## Bug Fixes


# cuDF 0.2.0 and cuDF 0.1.0

These were initial releases of cuDF based on previously separate pyGDF and libGDF libraries. RMM was initially implemented as part of libGDF.

