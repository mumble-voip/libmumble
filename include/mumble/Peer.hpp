// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_PEER_HPP
#define MUMBLE_PEER_HPP

#include "Macros.hpp"
#include "Message.hpp"
#include "Types.hpp"

#include <functional>
#include <memory>

namespace mumble {
class Connection;

class MUMBLE_EXPORT Peer {
public:
	class P;

	using SharedConnection = std::shared_ptr< Connection >;

	// XCR krzmbrzl: Again, "Callbacks" is probably a better name
	// Davide: Replied to the other comment.
	struct Feedback {
		std::function< void() > started;
		std::function< void() > stopped;

		std::function< void(Code code) > failed;

		std::function< uint32_t() > timeout;
	};

	struct FeedbackTCP : Feedback {
		std::function< bool(Endpoint &endpoint, int32_t fd) > connection;
	};

	struct FeedbackUDP : Feedback {
		std::function< void(Endpoint &endpoint, BufRef buf) > encrypted;
		std::function< void(Endpoint &endpoint, udp::Message::Ping &ping) > ping;
		std::function< void(Endpoint &endpoint, legacy::udp::Ping &ping) > legacyPing;
	};

	Peer();
	Peer(Peer &&peer);
	virtual ~Peer();

	virtual Peer &operator=(Peer &&peer);

	virtual explicit operator bool() const;

	static std::pair< Code, int32_t > connect(const Endpoint &peerEndpoint, const Endpoint &endpoint = {});

	// XCR krzmbrzl: Presumably we are taking ownership of feedback, so passing by value would also allow move-construction
	// Davide: We're copying it.
	virtual Code startTCP(const FeedbackTCP &feedback, const uint32_t threads = 0);
	virtual Code stopTCP();

	virtual Code startUDP(const FeedbackUDP &feedback);
	virtual Code stopUDP();

	// XCR krzmbrzl: Why is endpoint passed as a non-const reference?
	// Davide: The syscall may return a different IP address and/or port compared to the requested ones.
	// This is guaranteed to happen when asking for a random port (0).
	virtual Code bindTCP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindTCP();

	virtual Code bindUDP(Endpoint &endpoint, const bool ipv6Only = false);
	virtual Code unbindUDP();

	virtual Code addTCP(const SharedConnection &connection);
	virtual Code delTCP(const SharedConnection &connection);

	virtual Code sendUDP(const Endpoint &endpoint, const BufRefConst data);

private:
	Peer(const Peer &)                    = delete;
	virtual Peer &operator=(const Peer &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
