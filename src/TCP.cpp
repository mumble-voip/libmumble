// This file is part of libmumble.
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

SocketTCP::SocketTCP(const int32_t handle) : Socket(handle) {
}

int SocketTCP::listen() {
	if (::listen(m_handle, SOMAXCONN) != 0) {
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
	const auto handle = static_cast< int32_t >(::accept(m_handle, reinterpret_cast< sockaddr * >(&addr), &size));
	if (handle == invalidHandle) {
		return { osError(), invalidHandle };
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	return { 0, handle };
}

int SocketTCP::connect(const Endpoint &endpoint) {
	sockaddr_in6 addr = {};
	endpoint.ip.toSockAddr(addr);
	addr.sin6_port = Endian::toNetwork(endpoint.port);

	if (::connect(m_handle, reinterpret_cast< sockaddr * >(&addr), sizeof(addr)) != 0) {
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
	if (getpeername(m_handle, reinterpret_cast< sockaddr * >(&addr), &size) != 0) {
		return osError();
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	return 0;
}
