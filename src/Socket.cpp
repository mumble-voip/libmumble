// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Socket.hpp"

#include "mumble/Endian.hpp"
#include "mumble/IP.hpp"

#include <utility>

#ifdef OS_WINDOWS
#	include <afunix.h>
#	include <WS2tcpip.h>
#else
#	include <errno.h>
#	include <fcntl.h>
#	include <unistd.h>

#	include <netinet/in.h>
#	include <sys/socket.h>
#endif

using namespace mumble;

using Pair = Socket::Pair;

Socket::Socket() : m_fd(invalidFD) {
}

Socket::Socket(Socket &&socket) : m_fd(socket.stealFD()) {
}

Socket::Socket(const int32_t fd) : m_fd(fd) {
}

Socket::Socket(const Type type) {
	switch (type) {
		case Type::Local:
#ifdef OS_WINDOWS
			// https://github.com/microsoft/WSL/issues/5272
			m_fd = socket(PF_UNIX, SOCK_STREAM, 0);
#else
			m_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);
#endif
			break;
		case Type::TCP:
			m_fd = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
			break;
		case Type::UDP:
			m_fd = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
			break;
		default:
			m_fd = invalidFD;
	}
}

Socket::~Socket() {
	close(m_fd);
}

Socket::operator bool() const {
	return m_fd != invalidFD;
}

const int32_t Socket::fd() const {
	return m_fd;
}

int32_t Socket::stealFD() {
	return std::exchange(m_fd, invalidFD);
}

int Socket::getEndpoint(Endpoint &endpoint) const {
	sockaddr_in6 addr;
#ifdef OS_WINDOWS
	int size = sizeof(addr);
#else
	socklen_t size = sizeof(addr);
#endif
	if (getsockname(m_fd, reinterpret_cast< sockaddr * >(&addr), &size) != 0) {
		return osError();
	}

	endpoint.ip   = IP(addr);
	endpoint.port = Endian::toHost(addr.sin6_port);

	return 0;
}

int Socket::setEndpoint(const Endpoint &endpoint, const bool ipv6Only) {
#ifdef OS_WINDOWS
	// IPV6_V6ONLY demands a 4-byte integer...
	DWORD value = ipv6Only;
#else
	int value = ipv6Only;
#endif
	if (setsockopt(m_fd, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast< char * >(&value), sizeof(value)) != 0) {
		return osError();
	}

	value = 1;
#ifdef SO_EXCLUSIVEADDRUSE
	if (setsockopt(m_fd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast< char * >(&value), sizeof(value)) != 0) {
		return osError();
	}
#endif
	sockaddr_in6 addr = {};
	endpoint.ip.toSockAddr(addr);
	addr.sin6_port = Endian::toNetwork(endpoint.port);

	if (bind(m_fd, reinterpret_cast< sockaddr * >(&addr), sizeof(addr)) != 0) {
		return osError();
	}

	return 0;
}

int Socket::setBlocking(const bool enable) {
#ifdef OS_WINDOWS
	u_long value = !enable;
	if (ioctlsocket(m_fd, FIONBIO, &value) != 0) {
		return osError();
	}
#else
	const int flags = fcntl(m_fd, F_GETFL, 0);
	if (flags == -1) {
		return osError();
	}

	const int new_flags = enable ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;
	if (flags == new_flags) {
		return 0;
	}

	if (fcntl(m_fd, F_SETFL, new_flags) != 0) {
		return osError();
	}
#endif
	return 0;
}

Pair Socket::localPair() {
#ifdef OS_WINDOWS
	// https://github.com/microsoft/WSL/issues/4240
	Socket listener(Type::Local);
	if (!listener) {
		return {};
	}

	sockaddr_un addr  = {};
	addr.sun_family   = AF_UNIX;
	const auto length = GetTempPath(sizeof(addr.sun_path), addr.sun_path);
	if (!length) {
		return {};
	}

	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);
	snprintf(addr.sun_path + length, sizeof(addr.sun_path) - length, "MumbleSocket%llu", ticks.QuadPart);

	if (bind(listener.fd(), reinterpret_cast< sockaddr * >(&addr), sizeof(addr)) != 0) {
		return {};
	}

	if (listen(listener.fd(), 1) != 0) {
		return {};
	}

	auto first = Socket(Type::Local);
	if (!first) {
		return {};
	}

	if (connect(first.fd(), reinterpret_cast< sockaddr * >(&addr), sizeof(addr)) != 0) {
		return {};
	}

	DeleteFile(addr.sun_path);

	auto second = accept(listener.fd(), nullptr, nullptr);
	if (!second) {
		return {};
	}

	return { first.stealFD(), second };
#else
	int fds[2];
	if (socketpair(PF_LOCAL, SOCK_DGRAM, 0, fds) != 0) {
		return {};
	}

	return { fds[0], fds[1] };
#endif
}

void Socket::close(const int32_t fd) {
	if (fd == invalidFD) {
		return;
	}
#ifdef OS_WINDOWS
	shutdown(fd, SD_BOTH);
	closesocket(fd);
#else
	shutdown(fd, SHUT_RDWR);
	::close(fd);
#endif
}

int Socket::osError() {
#ifdef OS_WINDOWS
	return WSAGetLastError();
#else
	return errno;
#endif
}
