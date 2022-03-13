// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "TCP.hpp"

#include "mumble/Endian.hpp"
#include "mumble/IP.hpp"

#include <cstdint>

#ifdef OS_WINDOWS
#	include <WS2tcpip.h>
#else
#	include <netinet/in.h>
#	include <sys/socket.h>
#endif

using namespace mumble;

SocketTCP::SocketTCP() : Socket(Type::TCP) {
}

SocketTCP::SocketTCP(const int32_t fd) : Socket(fd) {
}

int SocketTCP::listen() {
	if (::listen(m_fd, SOMAXCONN) != 0) {
		return osError();
	}

	return 0;
}

std::pair< int, int32_t > SocketTCP::accept(Endpoint &endpoint) {
	sockaddr_in6 addr;
#ifdef OS_WINDOWS
	int size = sizeof(addr);
#else
	socklen_t size = sizeof(addr);
#endif
	// Explicit return data type because socket handles are unsigned on Windows.
	const int8_t fd = ::accept(m_fd, reinterpret_cast< sockaddr * >(&addr), &size);
	if (fd == invalidFD) {
		return { osError(), invalidFD };
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	return { 0, fd };
}

int SocketTCP::connect(const Endpoint &endpoint) {
	sockaddr_in6 addr = {};
	endpoint.ip.toSockAddr(addr);
	addr.sin6_port = Endian::toNetwork(endpoint.port);

	if (::connect(m_fd, reinterpret_cast< sockaddr * >(&addr), sizeof(addr)) != 0) {
		return osError();
	}

	return 0;
}

int SocketTCP::getPeerEndpoint(Endpoint &endpoint) const {
	sockaddr_in6 addr;
#ifdef OS_WINDOWS
	int size = sizeof(addr);
#else
	socklen_t size = sizeof(addr);
#endif
	if (getpeername(m_fd, reinterpret_cast< sockaddr * >(&addr), &size) != 0) {
		return osError();
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	return 0;
}
