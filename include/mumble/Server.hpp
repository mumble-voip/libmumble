// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SERVER_HPP
#define MUMBLE_SERVER_HPP

#include "Session.hpp"

namespace mumble {
class EXPORT Server {
public:
	class P;

	struct FeedbackTCP {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(const mumble::Code) > failed;

		std::function< uint32_t() > timeout;

		std::function< bool(Endpoint &endpoint) > connection;
		std::function< bool(Session::P *p) > session;
	};

	struct FeedbackUDP {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(const mumble::Code) > failed;

		std::function< uint32_t() > timeout;

		std::function< void(Endpoint &endpoint, Mumble::PingUDP &ping) > ping;
		std::function< void(Endpoint &endpoint, BufRef buf) > encrypted;
	};

	Server();
	Server(Server &&server);
	virtual ~Server();

	virtual Server &operator=(Server &&server);

	virtual explicit operator bool() const;

	virtual Code startTCP(const FeedbackTCP &feedback);
	virtual Code stopTCP();

	virtual Code startUDP(const FeedbackUDP &feedback);
	virtual Code stopUDP();

	virtual Code bindTCP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindTCP();

	virtual Code bindUDP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindUDP();

	virtual Code sendUDP(const Endpoint &endpoint, const BufRefConst data);

private:
	Server(const Server &)  = delete;
	virtual Server &operator=(const Server &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
