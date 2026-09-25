# This file is part of libmumble.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

find_package(Boost REQUIRED
	COMPONENTS
		thread
)

if (NOT TARGET Microsoft.GSL::GSL)
	find_package(Microsoft.GSL REQUIRED)
endif()

find_package(OpenSSL REQUIRED)
find_package(Opus REQUIRED)

add_subdirectory(proto)

if(NOT LIBMUMBLE_STATIC)
	add_library(mumble_library SHARED)
else()
	add_library(mumble_library STATIC)
endif()

add_library(Mumble::libmumble ALIAS mumble_library)

target_setup_default_flags(mumble_library)

set_target_properties(mumble_library
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
)

if(WIN32)
	# We use the bundled wepoll library
	set(HAVE_EPOLL TRUE)
else()
	include(CheckSymbolExists)

	check_symbol_exists("epoll_create" "sys/epoll.h" HAVE_EPOLL)
endif()

target_compile_definitions(mumble_library
	PRIVATE
		"MUMBLE_SRC"

		$<$<BOOL:${HAVE_EPOLL}>:$<IF:$<PLATFORM_ID:Windows>,HAVE_WEPOLL,HAVE_EPOLL>>
)

target_include_directories(mumble_library
	PUBLIC
		"${PROJECT_SOURCE_DIR}/include"
)

target_sources(mumble_library
	PRIVATE
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
)

target_link_libraries(mumble_library
	PRIVATE
		Proto

		Boost::thread
		OpenSSL::Crypto
		OpenSSL::SSL
		quickpool

		Opus::opus

		$<$<PLATFORM_ID:Windows>:wepoll::wepoll>
	PUBLIC
		Microsoft.GSL::GSL
)

