// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_IP_HPP
#define MUMBLE_IP_HPP

#include <array>
#include <memory>
#include <span>
#include <string>

struct sockaddr_in6;

namespace mumble {
class EXPORT IP {
public:
	class P;

	static constexpr uint8_t v6Size = 16;
	static constexpr uint8_t v4Size = 4;

	using V6       = std::array< uint8_t, v6Size >;
	using V4       = std::array< uint8_t, v4Size >;
	using Ref      = std::span< uint8_t >;
	using RefConst = std::span< const uint8_t >;

	IP();
	IP(const IP &ip);
	IP(const RefConst ref);
	IP(const std::string_view string);
	IP(const sockaddr_in6 &sockaddr);
	virtual ~IP();

	virtual IP &operator=(const IP &ip);
	virtual bool operator==(const IP &ip) const;

	virtual RefConst v6() const;
	virtual RefConst v4() const;

	virtual Ref v6();
	virtual Ref v4();

	virtual bool isV6() const;
	virtual bool isV4() const;

	virtual bool isWildcard() const;

	virtual std::string text() const;

	virtual void toSockAddr(sockaddr_in6 &sockaddr) const;

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
