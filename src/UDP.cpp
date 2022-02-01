// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "UDP.hpp"

#include "mumble/Endian.hpp"
#include "mumble/IP.hpp"

#ifdef OS_WINDOWS
#	include <WinSock2.h>
#else
#	include <netinet/in.h>
#endif

using namespace mumble;

SocketUDP::SocketUDP() : Socket(Type::UDP) {
}

Code SocketUDP::read(Endpoint &endpoint, BufRef &buf) {
	sockaddr_in6 addr;
#ifdef OS_WINDOWS
	int addrsize = sizeof(addr);
#else
	socklen_t addrsize = sizeof(addr);
#endif
	const auto ret =
		recvfrom(m_handle.fd(), buf.data(), buf.size(), 0, reinterpret_cast< sockaddr * >(&addr), &addrsize);
	if (ret <= 0) {
		return osErrorToCode(osError());
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	buf = buf.first(ret);

	return Code::Success;
}

Code SocketUDP::write(const Endpoint &endpoint, const BufRefConst buf) {
	sockaddr_in6 addr = {};
	endpoint.ip.toSockAddr(addr);
	addr.sin6_port = Endian::toNetwork(endpoint.port);

	const auto ret =
		sendto(m_handle.fd(), buf.data(), buf.size(), 0, reinterpret_cast< sockaddr * >(&addr), sizeof(addr));
	if (ret <= 0) {
		return osErrorToCode(osError());
	}

	return Code::Success;
}
