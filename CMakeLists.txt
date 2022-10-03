# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

# XCR krmzbrzl: This is a very odd version requirement. I doubt this is the actually required minimal version
# Davide: Can probably be significantly lower, I just set the CMake version I had when creating the project.
cmake_minimum_required(VERSION 3.18.4)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

project(libmumble)

option(build-examples "Build example client and server" OFF)
option(build-tests "Build tests" ON)

if(build-tests)
	include(CTest)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# XCR krmzbrzl: I guess it would be good-practice to prefix these variables with "MUMBLE_"
# Davide: I thought about it, but then I realized it's unnecessary: these variables are only ever "seen" in this
# specific project, even when added through add_subdirectory().
set(3RDPARTY_DIR "${PROJECT_SOURCE_DIR}/3rdparty")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

if(WIN32)
	add_definitions(
		"-DOS_WINDOWS"
		"-DNOMINMAX"
		"-DWIN32_LEAN_AND_MEAN"
		"-D_CRT_SECURE_NO_WARNINGS"
	)
else()
	add_definitions(
		"-DOS_UNIX"
	)
endif()

if(MSVC)
	add_compile_options(
		# https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus
		"/Zc:__cplusplus"
	)
endif()

# XCR krmzbrzl: This should be a CACHE variable (otherwise cmake will issue a warning)
# Davide: I didn't spot any related warnings so far...
set(QUICKPOOL_TEST OFF)

add_subdirectory("${3RDPARTY_DIR}/quickpool")
add_subdirectory("${3RDPARTY_DIR}/toml11")

add_subdirectory(src)
