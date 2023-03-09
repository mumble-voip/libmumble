// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_CONNECTION_HPP
#define MUMBLE_SRC_CONNECTION_HPP

#include "mumble/Connection.hpp"

#include "mumble/Cert.hpp"
#include "mumble/Types.hpp"

#include "Monitor.hpp"
#include "TLS.hpp"

#include <atomic>
#include <cstdint>
#include <functional>

namespace mumble {
class Connection::P : public SocketTLS {
	friend Connection;

public:
	using State = Monitor::Event::State;

	P(SocketTLS &&socket);
	~P();

	explicit operator bool() const;

	mumble::Code handleState(const State state);

private:
	mumble::Code read(BufView buf, const bool wait, const std::function< bool() > halt);
	mumble::Code write(BufViewConst buf, const bool wait, const std::function< bool() > halt);

	mumble::Code handleCode(const Code code, const bool wait);
	mumble::Code handleWait(Monitor &monitor);

	static constexpr mumble::Code interpretTLSCode(const Code code);

	Feedback m_feedback;

	Monitor m_monitorIn;
	Monitor m_monitorOut;

	Cert::Chain m_cert;
	uint32_t m_timeouts;
	std::atomic_flag m_closed;
};
} // namespace mumble

#endif
