// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_PEER_HPP
#define MUMBLE_PEER_HPP

#include "Session.hpp"

namespace mumble {
class EXPORT Peer {
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

	Peer();
	Peer(Peer &&peer);
	virtual ~Peer();

	virtual Peer &operator=(Peer &&peer);

	virtual explicit operator bool() const;

	virtual std::pair< Code, Session::P * > connect(const Endpoint &peerEndpoint, const Endpoint &endpoint = {});

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
	Peer(const Peer &)    = delete;
	virtual Peer &operator=(const Peer &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
