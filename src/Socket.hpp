// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_SOCKET_HPP
#define MUMBLE_SRC_SOCKET_HPP

#include "mumble/Macros.hpp"
#include "mumble/Types.hpp"

#include <array>
#include <cstdint>

#ifdef OS_WINDOWS
#	include <WinSock2.h>
#else
#	include <errno.h>
#endif

namespace mumble {
class Socket {
public:
	enum class Type : uint8_t { Unknown, TCP, UDP };

	enum State : uint8_t {
		Timeout      = 0b00000000,
		Triggered    = 0b00000001,
		InReady      = 0b00000010,
		OutReady     = 0b00000100,
		Disconnected = 0b00001000,
		Error        = 0b00010000
	};

	class Handle {
	public:
		static constexpr int8_t invalid = -1;

		using Pair = std::array< Handle, 2 >;

		Handle(Handle &&handle);
		Handle(const int32_t fd);
		Handle(const Type type = Type::Unknown);
		~Handle();

		Handle &operator=(const int32_t fd);

		explicit operator bool() const;

		int32_t fd() const;

		static Pair pair();

	private:
		Handle(const Handle &) = delete;
		Handle &operator=(const Handle &) = delete;

		int32_t m_fd;
	};

	Socket(Socket &&socket);
	Socket(const Type type);
	~Socket();

	explicit operator bool() const;

	int getEndpoint(Endpoint &endpoint);
	int setEndpoint(const Endpoint &endpoint, const bool ipv6Only = false);

	int setBlocking(const bool enable);

	bool trigger();
	State wait(const bool in, const bool out, const uint32_t timeout);

	static int osError();
	static constexpr Code osErrorToCode(const int error) {
		switch (error) {
			case 0:
				return Code::Success;
#ifdef OS_WINDOWS
			case WSAEACCES:
			case WSAECONNREFUSED:
				return Code::Refuse;
			case WSAEHOSTUNREACH:
			case WSAENETUNREACH:
				return Code::Reach;
			case WSAEADDRNOTAVAIL:
			case WSAEAFNOSUPPORT:
			case WSAEDESTADDRREQ:
			case WSAEFAULT:
			case WSAEINVAL:
			case WSAEISCONN:
			case WSAENETDOWN:
			case WSAENOTCONN:
			case WSAENOTSOCK:
			case WSAEOPNOTSUPP:
				return Code::Invalid;
			case WSAEMSGSIZE:
			case WSAENOBUFS:
				return Code::Memory;
			case WSANOTINITIALISED:
				return Code::Init;
			case WSAETIMEDOUT:
				return Code::Timeout;
			case WSAECONNABORTED:
				return Code::Cancel;
			case WSAEINTR:
			case WSAEWOULDBLOCK:
				return Code::Retry;
			case WSAEADDRINUSE:
			case WSAEALREADY:
			case WSAEINPROGRESS:
				return Code::Busy;
			case WSAECONNRESET:
			case WSAENETRESET:
			case WSAESHUTDOWN:
				return Code::Disconnect;
#else
			case EACCES:
			case ECONNREFUSED:
				return Code::Refuse;
			case ENETUNREACH:
				return Code::Reach;
			case EADDRNOTAVAIL:
			case EAFNOSUPPORT:
			case EBADF:
			case EDESTADDRREQ:
			case EFAULT:
			case EINVAL:
			case EISCONN:
			case ENOTCONN:
			case ENOTSOCK:
			case EOPNOTSUPP:
			case EPROTOTYPE:
				return Code::Invalid;
			case EMSGSIZE:
			case ENOBUFS:
			case ENOMEM:
				return Code::Memory;
			case ETIMEDOUT:
				return Code::Timeout;
#	if EAGAIN != EWOULDBLOCK
			case EAGAIN:
#	endif
			case ECONNABORTED:
				return Code::Cancel;
			case EWOULDBLOCK:
			case EINTR:
				return Code::Retry;
			case EADDRINUSE:
			case EALREADY:
			case EINPROGRESS:
				return Code::Busy;
			case ECONNRESET:
			case EPIPE:
				return Code::Disconnect;
#endif
		}

		return Code::Unknown;
	}

protected:
	Socket(const int fd);

	Handle m_handle;

private:
	Handle::Pair m_manualEvent;
};
} // namespace mumble

MUMBLE_ENUM_OPERATORS(mumble::Socket::State)

#endif
