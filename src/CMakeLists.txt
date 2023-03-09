# This file is part of libmumble.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

include(MumbleCompiler)

find_package(Boost REQUIRED
	COMPONENTS
		thread
)

find_package(Microsoft.GSL REQUIRED)

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
	add_library(mumble_library SHARED)
else()
	add_library(mumble_library STATIC)
endif()

add_library(Mumble::libmumble ALIAS mumble_library)

target_pedantic_warnings(mumble_library)
target_pimpl_warning_fix(mumble_library)

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
	PRIVATE
		${Opus_INCLUDE_DIRS}
	PUBLIC
		"${PROJECT_SOURCE_DIR}/include"
)

target_sources(mumble_library
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
)

target_link_libraries(mumble_library
	PRIVATE
		Proto

		Boost::thread
		OpenSSL::Crypto
		OpenSSL::SSL
		quickpool

		${Opus_LINK_LIBRARIES}
		$<TARGET_NAME_IF_EXISTS:Opus::opus>

		$<$<PLATFORM_ID:Windows>:wepoll::wepoll>
	PUBLIC
		Microsoft.GSL::GSL
)

