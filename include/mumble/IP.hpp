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

#include <gsl/span>

struct sockaddr_in6;

namespace mumble {
class MUMBLE_EXPORT IP {
public:
	class P;

	static constexpr uint8_t v6Size = 16;
	static constexpr uint8_t v4Size = 4;

	using V6        = std::array< uint8_t, v6Size >;
	using V4        = std::array< uint8_t, v4Size >;
	using View      = gsl::span< uint8_t >;
	using ViewConst = gsl::span< const uint8_t >;

	IP();
	IP(const IP &ip);
	IP(const ViewConst view);
	IP(const std::string_view string);
	IP(const sockaddr_in6 &sockaddr);
	virtual ~IP();

	virtual IP &operator=(const IP &ip);
	virtual bool operator==(const IP &ip) const;

	virtual ViewConst v6() const;
	virtual ViewConst v4() const;

	virtual View v6();
	virtual View v4();

	virtual bool isV6() const;
	virtual bool isV4() const;

	virtual bool isWildcard() const;

	virtual std::string text() const;

	virtual void toSockAddr(sockaddr_in6 &sockaddr) const;

private:
	std::array< uint8_t, v6Size > m_bytes;
};
} // namespace mumble

#endif
