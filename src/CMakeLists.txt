# Copyright 2022 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

find_package(Boost REQUIRED
	COMPONENTS
		thread
)
find_package(OpenSSL REQUIRED)
find_package(Opus)
find_package(Protobuf REQUIRED)

if(NOT Opus_FOUND)
	find_package(PkgConfig REQUIRED)
	if(PkgConfig_FOUND)
		pkg_check_modules(Opus REQUIRED opus)
	endif()
endif()

add_library(mumble SHARED)

protobuf_generate_cpp(PROTO_CPP PROTO_HPP "Mumble.proto")

set_target_properties(mumble
	PROPERTIES
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(mumble
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}
		${Opus_INCLUDE_DIRS}
	PUBLIC
		${INCLUDE_DIR}
)

target_sources(mumble
	PRIVATE
		"Base64.cpp"
		"Base64.hpp"
		"Cert.cpp"
		"Cert.hpp"
		"Client.cpp"
		"Client.hpp"
		"Connection.cpp"
		"Connection.hpp"
		"Crypt.cpp"
		"Crypt.hpp"
		"CryptOCB2.cpp"
		"CryptOCB2.hpp"
		"Endian.cpp"
		"Hash.cpp"
		"Hash.hpp"
		"IP.cpp"
		"IP.hpp"
		"Key.cpp"
		"Key.hpp"
		"Mumble.cpp"
		"Opus.cpp"
		"Opus.hpp"
		"Pack.cpp"
		"Pack.hpp"
		"Server.cpp"
		"Server.hpp"
		"Session.cpp"
		"Session.hpp"
		"Socket.cpp"
		"Socket.hpp"
		"TCP.cpp"
		"TCP.hpp"
		"TLS.cpp"
		"TLS.hpp"
		"UDP.cpp"
		"UDP.hpp"

		${PROTO_CPP}
		${PROTO_HPP}
	PUBLIC
		"${INCLUDE_DIR}/mumble/Base64.hpp"
		"${INCLUDE_DIR}/mumble/Cert.hpp"
		"${INCLUDE_DIR}/mumble/Client.hpp"
		"${INCLUDE_DIR}/mumble/Crypt.hpp"
		"${INCLUDE_DIR}/mumble/CryptOCB2.hpp"
		"${INCLUDE_DIR}/mumble/Endian.hpp"
		"${INCLUDE_DIR}/mumble/Hash.hpp"
		"${INCLUDE_DIR}/mumble/IP.hpp"
		"${INCLUDE_DIR}/mumble/Key.hpp"
		"${INCLUDE_DIR}/mumble/Macros.hpp"
		"${INCLUDE_DIR}/mumble/Message.hpp"
		"${INCLUDE_DIR}/mumble/Mumble.hpp"
		"${INCLUDE_DIR}/mumble/Opus.hpp"
		"${INCLUDE_DIR}/mumble/Server.hpp"
		"${INCLUDE_DIR}/mumble/Session.hpp"
		"${INCLUDE_DIR}/mumble/Types.hpp"
)

target_link_libraries(mumble
	PRIVATE
		Boost::thread
		MPMCQueue
		OpenSSL::Crypto
		OpenSSL::SSL
		protobuf::libprotobuf

		${Opus_LINK_LIBRARIES}
		$<TARGET_NAME_IF_EXISTS:Opus::opus>
)

if(build-examples)
	add_subdirectory(example)
endif()

if(build-tests)
	add_subdirectory(test)
endif()
