// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_TYPES_HPP
#define MUMBLE_TYPES_HPP

// For "include-what-you-use":
// https://github.com/include-what-you-use/include-what-you-use/issues/828
namespace mumble {};

#include "IP.hpp"

#include <algorithm>
#include <vector>

#include <boost/core/span.hpp>

namespace mumble {
enum class Code : int8_t {
	Ssl = -12,
	Refuse,
	Reach,
	Listen,
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

	virtual bool operator==(const Endpoint &endpoint) const { return endpoint.ip == ip && endpoint.port == port; }
};

static constexpr uint8_t infinite8   = UINT8_MAX;
static constexpr uint16_t infinite16 = UINT16_MAX;
static constexpr uint32_t infinite32 = UINT32_MAX;
static constexpr uint64_t infinite64 = UINT64_MAX;

using Buf         = std::vector< std::byte >;
using BufRef      = boost::span< std::byte >;
using BufRefConst = boost::span< const std::byte >;

template< size_t size > using FixedBuf = std::array< std::byte, size >;

static constexpr std::string_view text(const Code code) {
	switch (code) {
		case Code::Ssl:
			return "Ssl";
		case Code::Refuse:
			return "Refuse";
		case Code::Reach:
			return "Reach";
		case Code::Listen:
			return "Listen";
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

static constexpr std::byte toByte(const char byte) {
	return static_cast< std::byte >(byte);
}

static inline void toBuf(Buf &buf, const std::string_view str) {
	buf.resize(str.size());
	std::transform(str.cbegin(), str.cend(), buf.begin(), toByte);
}

} // namespace mumble

#endif
