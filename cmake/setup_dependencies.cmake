# This file is part of libmumble.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

include(FetchContent)

set(LIBMUMBLE_DEPENDENCY_DIR "${PROJECT_SOURCE_DIR}/_dependencies" CACHE STRING "Directory into which dependencies shall be downloaded into")

set(FETCHCONTENT_BASE_DIR "${LIBMUMBLE_DEPENDENCY_DIR}")

FetchContent_Declare(
	GSL
	GIT_REPOSITORY https://github.com/microsoft/GSL
	GIT_TAG        v4.0.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	quickpool
	GIT_REPOSITORY https://github.com/tnagler/quickpool
	# The latest release currently still has issues that make it unusable for us
	GIT_TAG        ddc415bec1fc624e1c6b21c1b47063ca2eef84de
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	wepoll
	GIT_REPOSITORY https://github.com/piscisaureus/wepoll
	GIT_TAG        v1.5.8
	GIT_SHALLOW    ON
	PATCH_COMMAND  "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/cmake/wepoll_cmakelists.txt" "./CMakeLists.txt"
)
FetchContent_Declare(
	cmake_compiler_flags
	GIT_REPOSITORY https://github.com/Krzmbrzl/cmake-compiler-flags
	GIT_TAG        v2.0.0
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest
	GIT_TAG        v1.13.0
	GIT_SHALLOW    ON
)

# Set some options for the dependencies
set(QUICKPOOL_TEST ${LIBMUMBLE_BUILD_TESTS} CACHE INTERNAL "")


message(STATUS ">>> Configuring dependencies (potentially includes downloading)")

FetchContent_MakeAvailable(quickpool cmake_compiler_flags)

if (LIBMUMBLE_BUNDLED_GSL)
	FetchContent_MakeAvailable(GSL)
endif()

if (WIN32)
	FetchContent_MakeAvailable(wepoll)
endif()

# Append the compiler flags CMake module to the module path
FetchContent_GetProperties(cmake_compiler_flags SOURCE_DIR COMPILER_FLAGS_SRC_DIR)
list(APPEND CMAKE_MODULE_PATH "${COMPILER_FLAGS_SRC_DIR}")

message(STATUS "<<< Dependency configuration finished")
