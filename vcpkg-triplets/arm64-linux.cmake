set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic) # Library linkage has to be dynamic by default to support assembly code in the x265 dependency, which doesn't play nice with PIC on ARM64

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# These flags need to be set until the following issue is resolved: https://github.com/microsoft/vcpkg/issues/44260
set(VCPKG_C_FLAGS "-DLIBYUV_DISABLE_NEON")
set(VCPKG_CXX_FLAGS "-DLIBYUV_DISABLE_NEON")
