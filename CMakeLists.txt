# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

cmake_minimum_required(VERSION 3.18.4)

project(libmumble)

option(build-examples "Build example client and server" OFF)
option(build-tests "Build tests" ON)

if(build-tests)
	include(CTest)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

if(WIN32)
	add_definitions(
		"-DOS_WINDOWS"
		"-DWIN32_LEAN_AND_MEAN"
		"-D_CRT_SECURE_NO_WARNINGS"
		"-DEXPORT=__declspec(dllexport)"
	)
else()
	add_definitions(
		"-DOS_UNIX"
		"-DEXPORT=__attribute__ ((visibility (\"default\")))"
	)

	add_compile_options(
		"-fvisibility=hidden"
	)
endif()

if(MSVC)
	add_definitions(
		"-DCOMPILER_MSVC"
	)

	add_compile_options(
		# https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus
		"/Zc:__cplusplus"
	)
endif()

add_subdirectory("${3RDPARTY_DIR}/MPMCQueue")
add_subdirectory("${3RDPARTY_DIR}/toml11")

add_subdirectory(src)
