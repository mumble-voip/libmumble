// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_SERVER_HPP
#define MUMBLE_SRC_SERVER_HPP

#include "mumble/Server.hpp"

namespace std {
class jthread;
class stop_token;
} // namespace std

namespace mumble {
class SocketTCP;
class SocketUDP;

class Server::P {
	friend Server;

public:
	P()  = default;
	~P() = default;

private:
	P(const P &) = delete;
	P &operator=(const P &) = delete;

	void tcpThread(const std::stop_token stopToken);
	void udpThread(const std::stop_token stopToken);

	FeedbackTCP m_feedbackTCP;
	FeedbackUDP m_feedbackUDP;

	std::unique_ptr< SocketTCP > m_socketTCP;
	std::shared_ptr< SocketUDP > m_socketUDP;
	std::unique_ptr< std::jthread > m_threadTCP;
	std::unique_ptr< std::jthread > m_threadUDP;
};
} // namespace mumble

#endif
