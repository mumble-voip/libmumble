# This file is part of libmumble.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

find_package(Protobuf REQUIRED)

protobuf_generate_cpp(TCP_CPP TCP_HPP "MumbleTCP.proto")
protobuf_generate_cpp(UDP_CPP UDP_HPP "MumbleUDP.proto")

add_library(Proto OBJECT)

target_disable_warnings(Proto)

target_include_directories(Proto
	INTERFACE
		${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(Proto PROPERTIES UNITY_BUILD OFF)

target_sources(Proto
	PRIVATE
		${TCP_CPP}
		${UDP_CPP}
		# Marking the headers as PUBLIC causes CMake's configure step to fail.
		# The files are generated during the build step.
		${TCP_HPP}
		${UDP_HPP}
)

target_link_libraries(Proto
	PRIVATE
		protobuf::libprotobuf
)
