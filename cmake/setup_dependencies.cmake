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
	GIT_REPOSITORY https://github.com/tnagler/quickpool.git
	# The latest release currently still has issues that make it unusable for us
	GIT_TAG        0a6b2430511c32a6b9ed315799f6ebce8f3b981d
	GIT_SHALLOW    ON
)
FetchContent_Declare(
	wepoll
	GIT_REPOSITORY https://github.com/piscisaureus/wepoll.git
	GIT_TAG        v1.5.8
	GIT_SHALLOW    ON
	PATCH_COMMAND  "${CMAKE_COMMAND}" -E copy "${PROJECT_SOURCE_DIR}/cmake/wepoll_cmakelists.txt" "./CMakeLists.txt"
)

# Set some options for the dependencies
set(QUICKPOOL_TEST ${LIBMUMBLE_BUILD_TESTS} CACHE INTERNAL "")


message(STATUS ">>> Configuring dependencies (potentially includes downloading)")

FetchContent_MakeAvailable(quickpool)

if (LIBMUMBLE_BUNDLED_GSL)
	FetchContent_MakeAvailable(GSL)
endif()

if (WIN32)
	FetchContent_MakeAvailable(wepoll)
endif()

message(STATUS "<<< Dependency configuration finished")
