# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

include(MumbleCompiler)

find_package(Boost REQUIRED
	COMPONENTS
		thread
)

if(NOT LIBMUMBLE_BUNDLED_GSL)
	find_package(Microsoft.GSL REQUIRED)
else()
	add_subdirectory("${3RDPARTY_DIR}/gsl" "${CMAKE_CURRENT_BINARY_DIR}/gsl" EXCLUDE_FROM_ALL)
endif()

find_package(OpenSSL REQUIRED)
find_package(Opus)

if(NOT Opus_FOUND)
	find_package(PkgConfig REQUIRED)
	if(PkgConfig_FOUND)
		pkg_check_modules(Opus REQUIRED opus)
	endif()
endif()

add_subdirectory(proto)

if(NOT LIBMUMBLE_STATIC)
	add_library(mumble SHARED)
else()
	add_library(mumble STATIC)
endif()

target_pedantic_warnings(mumble)
target_pimpl_warning_fix(mumble)

set_target_properties(mumble
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

if(WIN32)
	set(HAVE_EPOLL TRUE)
else()
	include(CheckSymbolExists)

	check_symbol_exists("epoll_create" "sys/epoll.h" HAVE_EPOLL)
endif()

target_compile_definitions(mumble
	PRIVATE
		"MUMBLE_SRC"

		$<$<BOOL:${HAVE_EPOLL}>:$<IF:$<PLATFORM_ID:Windows>,HAVE_WEPOLL,HAVE_EPOLL>>
)

target_include_directories(mumble
	PRIVATE
		${Opus_INCLUDE_DIRS}

		$<$<PLATFORM_ID:Windows>:${3RDPARTY_DIR}/wepoll>
	PUBLIC
		${INCLUDE_DIR}
)

target_sources(mumble
	PRIVATE
		"Base64.cpp"
		"Base64.hpp"
		"Cert.cpp"
		"Cert.hpp"
		"Connection.cpp"
		"Connection.hpp"
		"Crypt.cpp"
		"Crypt.hpp"
		"CryptOCB2.cpp"
		"CryptOCB2.hpp"
		"Hash.cpp"
		"Hash.hpp"
		"IP.cpp"
		"Key.cpp"
		"Key.hpp"
		"Lib.cpp"
		"Monitor.cpp"
		"Monitor.hpp"
		"Opus.cpp"
		"Opus.hpp"
		"Pack.cpp"
		"Peer.cpp"
		"Peer.hpp"
		"Socket.cpp"
		"Socket.hpp"
		"TCP.cpp"
		"TCP.hpp"
		"TLS.cpp"
		"TLS.hpp"
		"UDP.cpp"
		"UDP.hpp"

		$<$<PLATFORM_ID:Windows>:${3RDPARTY_DIR}/wepoll/wepoll.c>
		$<$<PLATFORM_ID:Windows>:${3RDPARTY_DIR}/wepoll/wepoll.h>
	PUBLIC
		"${INCLUDE_DIR}/mumble/Base64.hpp"
		"${INCLUDE_DIR}/mumble/Cert.hpp"
		"${INCLUDE_DIR}/mumble/Connection.hpp"
		"${INCLUDE_DIR}/mumble/Crypt.hpp"
		"${INCLUDE_DIR}/mumble/CryptOCB2.hpp"
		"${INCLUDE_DIR}/mumble/Endian.hpp"
		"${INCLUDE_DIR}/mumble/Hash.hpp"
		"${INCLUDE_DIR}/mumble/IP.hpp"
		"${INCLUDE_DIR}/mumble/Key.hpp"
		"${INCLUDE_DIR}/mumble/Lib.hpp"
		"${INCLUDE_DIR}/mumble/Macros.hpp"
		"${INCLUDE_DIR}/mumble/Message.hpp"
		"${INCLUDE_DIR}/mumble/NonCopyable.hpp"
		"${INCLUDE_DIR}/mumble/Opus.hpp"
		"${INCLUDE_DIR}/mumble/Pack.hpp"
		"${INCLUDE_DIR}/mumble/Peer.hpp"
		"${INCLUDE_DIR}/mumble/Types.hpp"
)

target_link_libraries(mumble
	PRIVATE
		Proto

		Boost::thread
		OpenSSL::Crypto
		OpenSSL::SSL
		quickpool

		${Opus_LINK_LIBRARIES}
		$<TARGET_NAME_IF_EXISTS:Opus::opus>

		$<$<PLATFORM_ID:Windows>:WS2_32>
	PUBLIC
		Microsoft.GSL::GSL
)

if(LIBMUMBLE_BUILD_EXAMPLES)
	add_subdirectory(example)
endif()

if(LIBMUMBLE_BUILD_TESTS)
	add_subdirectory(test)
endif()
