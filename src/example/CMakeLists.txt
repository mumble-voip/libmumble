# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

include(compiler)

list(APPEND EXAMPLES
	"ExampleClient"
	"ExampleServer"
)

add_library(Example STATIC)

target_include_directories(Example
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(Example
	PRIVATE
		"MumbleInit.cpp"
	PUBLIC
		"MumbleInit.hpp"
)

target_link_libraries(Example
	PRIVATE
		mumble
)

# All examples follow the convention of naming their subdirectory the same as their target.
# Therefore we can use the TARGET variable to reference the directory as well as the target.
foreach(TARGET IN LISTS EXAMPLES)
	add_subdirectory(${TARGET})

	target_pedantic_warnings(${TARGET})

	set_target_properties(${TARGET}
		PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/example"
	)

	target_link_libraries(${TARGET}
		PRIVATE
			mumble

			Example
	)
endforeach()
