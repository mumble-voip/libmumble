// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_TYPES_HPP
#define MUMBLE_TYPES_HPP

#include "IP.hpp"

#include <vector>

namespace mumble {
enum class Code : int8_t {
	Ssl = -13,
	Refuse,
	Reach,
	Listen,
	Bind,
	Open,
	Unsupport,
	Invalid,
	Init,
	Timeout,
	Memory,
	Failure,
	Unknown,
	Success,
	Cancel,
	Retry,
	Busy,
	Disconnect
};

struct Endpoint {
	IP ip;
	uint16_t port;

	Endpoint() : port(0) {}
	Endpoint(const Endpoint &endpoint) = default;
	Endpoint(const IP &ip) : ip(ip), port(0) {}
	Endpoint(const uint16_t port) : port(port) {}
	Endpoint(const IP &ip, const uint16_t port) : ip(ip), port(port) {}
	virtual ~Endpoint() = default;

	virtual Endpoint &operator=(const Endpoint &endpoint) = default;
	virtual Endpoint &operator=(Endpoint &&endpoint) = default;

	virtual bool operator==(const Endpoint &endpoint) const = default;
};

class Base64;
class Cert;
class Client;
class Crypt;
class CryptOCB2;
class Endian;
class Key;
class Message;
class Mumble;
class Server;
class Session;

static constexpr uint8_t infinite8   = UINT8_MAX;
static constexpr uint16_t infinite16 = UINT16_MAX;
static constexpr uint32_t infinite32 = UINT32_MAX;
static constexpr uint64_t infinite64 = UINT64_MAX;

using Buf         = std::vector< std::byte >;
using BufRef      = std::span< std::byte >;
using BufRefConst = std::span< const std::byte >;

template< size_t size > using FixedBuf = std::array< std::byte, size >;

static constexpr std::string_view toString(const Code code) {
	switch (code) {
		case Code::Ssl:
			return "Ssl";
		case Code::Refuse:
			return "Refuse";
		case Code::Reach:
			return "Reach";
		case Code::Listen:
			return "Listen";
		case Code::Bind:
			return "Bind";
		case Code::Open:
			return "Open";
		case Code::Unsupport:
			return "Unsupport";
		case Code::Invalid:
			return "Invalid";
		case Code::Init:
			return "Init";
		case Code::Timeout:
			return "Timeout";
		case Code::Memory:
			return "Memory";
		case Code::Failure:
			return "Failure";
		case Code::Unknown:
			return "Unknown";
		case Code::Success:
			return "Success";
		case Code::Cancel:
			return "Cancel";
		case Code::Retry:
			return "Retry";
		case Code::Busy:
			return "Busy";
		case Code::Disconnect:
			return "Disconnect";
	}

	return {};
}
} // namespace mumble

#endif
