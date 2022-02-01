// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_CONNECTION_HPP
#define MUMBLE_SRC_CONNECTION_HPP

#include "TLS.hpp"

#include "mumble/Mumble.hpp"

#include <functional>

namespace std {
class jthread;
class stop_token;
} // namespace std

namespace mumble {
class Pack;

class Connection : public SocketTLS {
public:
	struct Feedback {
		std::function< void() > opened;
		std::function< void() > closed;

		std::function< void(const mumble::Code) > failed;

		std::function< uint32_t() > timeout;
		std::function< uint32_t() > timeouts;

		std::function< void(const Pack &) > message;
	};

	Connection(SocketTLS &&socket);
	~Connection();

	void start(const Feedback &feedback);
	void stop();

	mumble::Code write(const std::stop_token &stopToken, BufRefConst buf);

private:
	Connection(const Connection &) = delete;
	Connection &operator=(const Connection &) = delete;

	mumble::Code toCode(const Code code);
	mumble::Code handleCode(const Code code);

	mumble::Code read(const std::stop_token &stopToken, BufRef buf);

	void thread(const std::stop_token stopToken);

	Feedback m_feedback;

	uint32_t m_timeouts;
	std::unique_ptr< std::jthread > m_thread;
};
} // namespace mumble

#endif
