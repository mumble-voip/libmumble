# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

include(MumbleCompiler)

list(APPEND TESTS
	"TestBase64"
	"TestCrypt"
	"TestHash"
	"TestOpus"
)

add_library(Test STATIC)

target_include_directories(Test
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(Test
	PRIVATE
		"ThreadManager.cpp"
	PUBLIC
		"ThreadManager.hpp"
)

target_link_libraries(Test
	PUBLIC
		Boost::thread
)

# All tests follow the convention of naming their subdirectory the same as their target.
# Therefore we can use the TARGET variable to reference the directory as well as the target.
foreach(TARGET IN LISTS TESTS)
	add_subdirectory(${TARGET})
	add_test(
		NAME ${TARGET}
		COMMAND ${TARGET}
	)

	target_pedantic_warnings(${TARGET})

	set_target_properties(${TARGET}
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test"
	)

	target_link_libraries(${TARGET}
		PRIVATE
			mumble

			Test
	)
endforeach()
