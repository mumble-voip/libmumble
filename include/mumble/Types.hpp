// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_TYPES_HPP
#define MUMBLE_TYPES_HPP

// For "include-what-you-use":
// https://github.com/include-what-you-use/include-what-you-use/issues/828
namespace mumble {}

#include "IP.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include <boost/core/span.hpp>

// CR krzmbrzl: Every type should be put into its own header file. If you really want to, you can then have this header
// file include all the sub-files, but having a file per type/class makes it so much easier to find out where the
// declaration/definition for a given type/class is located.

namespace mumble {
// CR krzmbrzl: "Code" is a pretty bad name for this. To me this makes it sound as if this was some kind of function that can be
// executed. Instead, this should be called e.g. "ErrorCode".
// CR krzmbrzl: It seems strange to have this enum be defined here without context. Clearly these codes have some meaning in a given
// context, but it doesn't appear to me that the same codes (or rather their names) can be reused in arbitrary contexts...
// Thus, I think this should be defined more in-context.
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
	virtual Endpoint &operator=(Endpoint &&endpoint)      = default;

	virtual bool operator==(const Endpoint &endpoint) const { return endpoint.ip == ip && endpoint.port == port; }
};

struct Version {
	uint16_t major;
	uint16_t minor;
	uint16_t patch;
	uint16_t extra;

	Version(const uint16_t major = 0, const uint16_t minor = 0, const uint16_t patch = 0, const uint16_t extra = 0)
		: major(major), minor(minor), patch(patch), extra(extra) {}

	// CR krzmbrzl: I don't think "blob" is a good name. I would propose "encoded" or something similar, instead.
	Version(const uint64_t blob)
		: Version(static_cast< uint16_t >((blob & maskMajor64) >> offsetMajor64),
				  static_cast< uint16_t >((blob & maskMinor64) >> offsetMinor64),
				  static_cast< uint16_t >((blob & maskPatch64) >> offsetPatch64),
				  static_cast< uint16_t >((blob & maskExtra64) >> offsetExtra64)) {}

	Version(const uint32_t blob)
		: Version(static_cast< uint16_t >((blob & maskMajor32) >> offsetMajor32),
				  static_cast< uint16_t >((blob & maskMinor32) >> offsetMinor32),
				  static_cast< uint16_t >((blob & maskPatch32) >> offsetPatch32)) {}

	// CR krzmbrzl: The encoded Version representation shouldn't be exposed to the client. It should only have to deal with
	// the Version struct and all en- and decoding should be an implementation detail of the library. That way, we don't
	// have to worry about breaking user-code when having to make changes to the version encoding.
	uint64_t blob64() const {
		return (static_cast< uint64_t >(major) << offsetMajor64) | (static_cast< uint64_t >(minor) << offsetMinor64)
			   | (static_cast< uint64_t >(patch) << offsetPatch64) | (static_cast< uint64_t >(extra) << offsetExtra64);
	}

	// CR krzmbrzl: This shouldn't even be exposed to the library user, as we consider this to be deprecated
	uint32_t blob32() const {
		return (std::min< uint32_t >(major, std::numeric_limits< uint16_t >::max()) << offsetMajor32)
			   | (std::min< uint32_t >(minor, std::numeric_limits< uint8_t >::max()) << offsetMinor32)
			   | (std::min< uint32_t >(patch, std::numeric_limits< uint8_t >::max()) << offsetPatch32);
	}

	// CR krzmbrzl: auto should not be used here
	static constexpr auto maskMajor64 = 0xFFFF000000000000;
	static constexpr auto maskMinor64 = 0xFFFF00000000;
	static constexpr auto maskPatch64 = 0xFFFF0000;
	static constexpr auto maskExtra64 = 0xFFFF;
	static constexpr auto maskMajor32 = 0xFFFF0000;
	static constexpr auto maskMinor32 = 0xFF00;
	static constexpr auto maskPatch32 = 0xFF;

	// CR krzmbrzl: auto should not be used here
	static constexpr auto offsetMajor64 = 48;
	static constexpr auto offsetMinor64 = 32;
	static constexpr auto offsetPatch64 = 16;
	static constexpr auto offsetExtra64 = 0;
	static constexpr auto offsetMajor32 = 16;
	static constexpr auto offsetMinor32 = 8;
	static constexpr auto offsetPatch32 = 0;
};

// CR krzmbrzl: "infinite" is completely wrong. If anything, these should be called "..._max". However, imo we shouldn't define
// these constants at all. If they are needed anywhere, users should either use the macros or even better: std::numeric_limits directly
static constexpr uint8_t infinite8   = UINT8_MAX;
static constexpr uint16_t infinite16 = UINT16_MAX;
static constexpr uint32_t infinite32 = UINT32_MAX;
static constexpr uint64_t infinite64 = UINT64_MAX;

// CR krzmbrzl: We should type out the full name for publicly visible type definitions -> "Buffer"
// But even then this is a bit unspecific. It's not just any buffer, it's something specific -> "ByteBuffer"?
using Buf         = std::vector< std::byte >;
// CR krzmbrzl: We should probably call these a "view" instead of a "ref"
using BufRef      = boost::span< std::byte >;
using BufRefConst = boost::span< const std::byte >;

// CR krzmbrzl: "FixedSize..." (otherwise it is not clear what exactly is fixed)
template< size_t size > using FixedBuf = std::array< std::byte, size >;

// CR krzmbrzl: The name of this function is too generic, imo. I would propose a more commonly used "toString", which still
// is very generic, but at least widely known.
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

// CR krzmbrzl: Do we really need below functions in the public API? I don't think so.

static constexpr std::byte toByte(const char byte) {
	return static_cast< std::byte >(byte);
}

// CR krzmbrzl: "toByteBuffer"
static inline void toBuf(Buf &buf, const std::string_view str) {
	buf.resize(str.size());
	std::transform(str.cbegin(), str.cend(), buf.begin(), toByte);
}

} // namespace mumble

#endif
