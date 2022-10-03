// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_IP_HPP
#define MUMBLE_IP_HPP

#include "Macros.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include <boost/core/span.hpp>

// XCR krzmbrzl: This naming is inconsistent with other class names (the use of underscore). Also the name could probably use some
// general improving.
//
// Davide: Unfortunately this doesn't depend on us, it's the "standard" structure name that is provided through the OS API.
struct sockaddr_in6;

namespace mumble {
class MUMBLE_EXPORT IP {
public:
	class P;

	static constexpr uint8_t v6Size = 16;
	static constexpr uint8_t v4Size = 4;

	using V6       = std::array< uint8_t, v6Size >;
	using V4       = std::array< uint8_t, v4Size >;
	using Ref      = boost::span< uint8_t >;
	using RefConst = boost::span< const uint8_t >;

	IP();
	IP(const IP &ip);
	IP(const RefConst ref);
	IP(const std::string_view string);
	IP(const sockaddr_in6 &sockaddr);
	virtual ~IP();

	virtual IP &operator=(const IP &ip);
	// XCR krzmbrzl: Should also implement operator!=
	// Davide: It's implicit. https://en.cppreference.com/w/cpp/language/operators
	virtual bool operator==(const IP &ip) const;

	virtual RefConst v6() const;
	virtual RefConst v4() const;

	virtual Ref v6();
	virtual Ref v4();

	virtual bool isV6() const;
	virtual bool isV4() const;

	virtual bool isWildcard() const;

	// XCR krzmbrzl: Why return by value?
	// Davide: We cannot return something we don't store ourselves as reference (the IP address is stored as an array of bytes).
	virtual std::string text() const;

	virtual void toSockAddr(sockaddr_in6 &sockaddr) const;

private:
	std::array< uint8_t, v6Size > m_bytes;
};
} // namespace mumble

#endif
