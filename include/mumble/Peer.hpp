// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_PEER_HPP
#define MUMBLE_PEER_HPP

#include "Mumble.hpp"
#include "Types.hpp"

#include <functional>
#include <memory>

namespace mumble {
class Connection;

class EXPORT Peer {
public:
	class P;

	using SharedConnection = std::shared_ptr< Connection >;

	struct FeedbackTCP {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(Code code) > failed;

		std::function< uint32_t() > timeout;

		std::function< bool(Endpoint &endpoint, int32_t fd) > connection;
	};

	struct FeedbackUDP {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(Code code) > failed;

		std::function< uint32_t() > timeout;

		std::function< void(Endpoint &endpoint, Mumble::PingUDP &ping) > ping;
		std::function< void(Endpoint &endpoint, BufRef buf) > encrypted;
	};

	Peer();
	Peer(Peer &&peer);
	virtual ~Peer();

	virtual Peer &operator=(Peer &&peer);

	virtual explicit operator bool() const;

	static std::pair< Code, int32_t > connect(const Endpoint &peerEndpoint, const Endpoint &endpoint = {});

	virtual Code startTCP(const FeedbackTCP &feedback, const uint32_t threads = 0);
	virtual Code stopTCP();

	virtual Code startUDP(const FeedbackUDP &feedback);
	virtual Code stopUDP();

	virtual Code bindTCP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindTCP();

	virtual Code bindUDP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindUDP();

	virtual Code addTCP(const SharedConnection &connection);
	virtual Code delTCP(const SharedConnection &connection);

	virtual Code sendUDP(const Endpoint &endpoint, const BufRefConst data);

private:
	Peer(const Peer &)    = delete;
	virtual Peer &operator=(const Peer &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
