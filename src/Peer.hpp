// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_PEER_HPP
#define MUMBLE_SRC_PEER_HPP

#include "mumble/Peer.hpp"

#include <memory>

namespace boost {
class thread;
}

namespace mumble {
class SocketTCP;
class SocketUDP;

class Peer::P {
	friend Peer;

public:
	P()  = default;
	~P() = default;

private:
	P(const P &) = delete;
	P &operator=(const P &) = delete;

	void tcpThread();
	void udpThread();

	FeedbackTCP m_feedbackTCP;
	FeedbackUDP m_feedbackUDP;

	std::unique_ptr< SocketTCP > m_socketTCP;
	std::shared_ptr< SocketUDP > m_socketUDP;
	std::unique_ptr< boost::thread > m_threadTCP;
	std::unique_ptr< boost::thread > m_threadUDP;
};
} // namespace mumble

#endif
