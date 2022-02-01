// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_SESSION_HPP
#define MUMBLE_SRC_SESSION_HPP

#include "Connection.hpp"

#include "mumble/Session.hpp"

namespace mumble {
class SocketTCP;
class SocketUDP;

class Session::P {
	friend Session;

public:
	P(SocketTCP &&socketTCP, const std::shared_ptr< SocketUDP > &socketUDP, const bool server);
	~P() = default;

private:
	Feedback m_feedback;

	std::shared_ptr< SocketUDP > m_socketUDP;
	std::unique_ptr< Connection > m_connection;
};
} // namespace mumble

#endif
