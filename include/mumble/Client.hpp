// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CLIENT_HPP
#define MUMBLE_CLIENT_HPP

#include "Session.hpp"

namespace mumble {
class EXPORT Client {
public:
	class P;

	struct FeedbackUDP {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(const mumble::Code) > failed;

		std::function< uint32_t() > timeout;

		std::function< void(Endpoint &endpoint, Mumble::PingUDP &ping) > ping;
		std::function< void(Endpoint &endpoint, BufRef buf) > encrypted;
	};

	Client();
	Client(Client &&client);
	virtual ~Client();

	virtual Client &operator=(Client &&server);

	virtual explicit operator bool() const;

	virtual std::pair< Code, Session::P * > connect(const Endpoint &peerEndpoint, const Endpoint &endpoint = {});

	virtual Code startUDP(const FeedbackUDP &feedback);
	virtual Code stopUDP();

	virtual Code bindUDP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindUDP();

	virtual Code sendUDP(const Endpoint &endpoint, const BufRefConst data);

private:
	Client(const Client &)  = delete;
	virtual Client &operator=(const Client &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
