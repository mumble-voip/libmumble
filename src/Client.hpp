// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_CLIENT_HPP
#define MUMBLE_SRC_CLIENT_HPP

#include "mumble/Client.hpp"

#include <memory>

namespace boost {
class thread;
}

namespace mumble {
class SocketUDP;

class Client::P {
	friend Client;

public:
	P();
	~P() = default;

private:
	P(const P &) = delete;
	P &operator=(const P &) = delete;

	void udpThread();

	FeedbackUDP m_feedbackUDP;

	std::shared_ptr< SocketUDP > m_socketUDP;
	std::unique_ptr< boost::thread > m_threadUDP;
};
} // namespace mumble

#endif
