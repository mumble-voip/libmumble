// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Client.hpp"

#include "Pack.hpp"
#include "Session.hpp"
#include "TLS.hpp"
#include "UDP.hpp"

#include "mumble/IP.hpp"

#include <thread>

using namespace mumble;

using P = Client::P;

EXPORT Client::Client() : m_p(new P) {
}

EXPORT Client::Client(Client &&client) : m_p(std::exchange(client.m_p, nullptr)) {
}

EXPORT Client::~Client() {
	stopUDP();
}

EXPORT Client &Client::operator=(Client &&client) {
	m_p = std::exchange(client.m_p, nullptr);
	return *this;
}

EXPORT Client::operator bool() const {
	return static_cast< bool >(m_p);
}

EXPORT std::pair< Code, Session::P * > Client::connect(const Endpoint &peerEndpoint, const Endpoint &endpoint) {
	SocketTCP socket;

	if (endpoint.port || !endpoint.ip.isWildcard()) {
		const auto code = Socket::osErrorToCode(socket.setEndpoint(endpoint));
		if (code != Code::Success) {
			return { code, {} };
		}
	}

	const auto code = Socket::osErrorToCode(socket.connect(peerEndpoint));
	if (code != Code::Success) {
		return { code, {} };
	}

	return { Code::Success, new Session::P(std::move(socket), m_p->m_socketUDP, false) };
}

EXPORT Code Client::startUDP(const FeedbackUDP &feedback) {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_socketUDP) {
		return Code::Init;
	}

	m_p->m_feedbackUDP = feedback;

	m_p->m_threadUDP = std::make_unique< std::jthread >(std::bind_front(&P::udpThread, m_p.get()));

	return Code::Success;
}

EXPORT Code Client::stopUDP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_threadUDP) {
		return Code::Success;
	}

	m_p->m_threadUDP->request_stop();
	m_p->m_socketUDP->trigger();
	m_p->m_threadUDP.reset();

	return Code::Success;
}

EXPORT Code Client::bindUDP(Endpoint &endpoint, const bool ipv6Only) {
	if (!*this) {
		return Code::Invalid;
	}

	m_p->m_socketUDP = std::make_unique< SocketUDP >();
	if (!*m_p->m_socketUDP) {
		return Code::Open;
	}

	m_p->m_socketUDP->setBlocking(false);

	const auto ret = m_p->m_socketUDP->setEndpoint(endpoint, ipv6Only);
	if (ret != 0) {
		return Socket::osErrorToCode(ret);
	}

	return Code::Success;
}

EXPORT Code Client::unbindUDP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (m_p->m_threadUDP) {
		return Code::Busy;
	}

	m_p->m_socketUDP.reset();

	return Code::Success;
}

EXPORT Code Client::sendUDP(const Endpoint &endpoint, const BufRefConst data) {
	return m_p->m_socketUDP->write(endpoint, data);
}

void P::udpThread(const std::stop_token stopToken) {
	if (m_feedbackUDP.started) {
		m_feedbackUDP.started();
	}

	FixedBuf< 1024 > buf;

	auto state = Socket::InReady;

	while (!stopToken.stop_requested()) {
		while (state & Socket::InReady) {
			Endpoint endpoint;
			BufRef bufRef(buf);

			const auto code = m_socketUDP->read(endpoint, bufRef);
			switch (code) {
				case Code::Success:
					if (Pack::isPingUDP(bufRef)) {
						if (m_feedbackUDP.ping) {
							m_feedbackUDP.ping(endpoint, *reinterpret_cast< Mumble::PingUDP * >(bufRef.data()));
						}
					} else if (m_feedbackUDP.encrypted) {
						m_feedbackUDP.encrypted(endpoint, bufRef);
					}

					continue;
				case Code::Timeout:
				case Code::Retry:
				case Code::Busy:
					state = Socket::Timeout;
					continue;
				default:
					m_feedbackUDP.failed(code);
					return;
			}
		}

		state = m_socketUDP->wait(true, false, m_feedbackUDP.timeout ? m_feedbackUDP.timeout() : infinite32);
	}

	if (m_feedbackUDP.stopped) {
		m_feedbackUDP.stopped();
	}
}
