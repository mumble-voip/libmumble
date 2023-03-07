# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

cmake_minimum_required(VERSION 3.18.4)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(libmumble
	VERSION "0.0.0.0"
	DESCRIPTION "The official Mumble library."
	HOMEPAGE_URL "https://www.mumble.info"
	LANGUAGES "C" "CXX"
)

option(LIBMUMBLE_BUILD_EXAMPLES "Build example client and server" OFF)
option(LIBMUMBLE_BUILD_TESTS "Build tests" ON)

option(LIBMUMBLE_BUNDLED_GSL "Use the bundled GSL version instead of looking for one on the system" ON)

option(LIBMUMBLE_STATIC "Build the library as static instead of shared" OFF)

include(CTest)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

set(QUICKPOOL_TEST OFF)

add_subdirectory("${3RDPARTY_DIR}/quickpool")
add_subdirectory("${3RDPARTY_DIR}/toml11")

add_subdirectory(src)
