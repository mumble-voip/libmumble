// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Server.hpp"

#include "Pack.hpp"
#include "Session.hpp"
#include "TLS.hpp"
#include "UDP.hpp"

#include "mumble/IP.hpp"

#include <boost/thread/thread.hpp>

using namespace mumble;

using P = Server::P;

EXPORT Server::Server() : m_p(new P) {
}

EXPORT Server::Server(Server &&server) : m_p(std::exchange(server.m_p, nullptr)) {
}

EXPORT Server::~Server() {
	stopTCP();
	stopUDP();
}

EXPORT Server &Server::operator=(Server &&server) {
	m_p = std::exchange(server.m_p, nullptr);
	return *this;
}

EXPORT Server::operator bool() const {
	return static_cast< bool >(m_p);
}

EXPORT Code Server::startTCP(const FeedbackTCP &feedback) {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_socketTCP) {
		return Code::Init;
	}

	m_p->m_feedbackTCP = feedback;

	m_p->m_threadTCP = std::make_unique< boost::thread >(&P::tcpThread, m_p.get());

	return Code::Success;
}

EXPORT Code Server::stopTCP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_threadTCP) {
		return Code::Success;
	}

	m_p->m_threadTCP->interrupt();
	m_p->m_socketTCP->trigger();

	if (m_p->m_threadTCP->joinable()) {
		m_p->m_threadTCP->join();
	}

	m_p->m_threadTCP.reset();

	return Code::Success;
}

EXPORT Code Server::startUDP(const FeedbackUDP &feedback) {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_socketUDP) {
		return Code::Init;
	}

	m_p->m_feedbackUDP = feedback;

	m_p->m_threadUDP = std::make_unique< boost::thread >(&P::udpThread, m_p.get());

	return Code::Success;
}

EXPORT Code Server::stopUDP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (!m_p->m_threadUDP) {
		return Code::Success;
	}

	m_p->m_threadUDP->interrupt();
	m_p->m_socketUDP->trigger();

	if (m_p->m_threadUDP->joinable()) {
		m_p->m_threadUDP->join();
	}

	m_p->m_threadUDP.reset();

	return Code::Success;
}

EXPORT Code Server::bindTCP(Endpoint &endpoint, const bool ipv6Only) {
	if (!*this) {
		return Code::Invalid;
	}

	m_p->m_socketTCP = std::make_unique< SocketTCP >();
	if (!*m_p->m_socketTCP) {
		return Code::Open;
	}

	m_p->m_socketTCP->setBlocking(false);

	auto ret = m_p->m_socketTCP->setEndpoint(endpoint, ipv6Only);
	if (ret != 0) {
		return Socket::osErrorToCode(ret);
	}

	ret = m_p->m_socketTCP->listen();
	if (ret != 0) {
		return Socket::osErrorToCode(ret);
	}

	return Code::Success;
}

EXPORT Code Server::unbindTCP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (m_p->m_threadTCP) {
		return Code::Busy;
	}

	m_p->m_socketTCP.reset();

	return Code::Success;
}

EXPORT Code Server::bindUDP(Endpoint &endpoint, const bool ipv6Only) {
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

EXPORT Code Server::unbindUDP() {
	if (!*this) {
		return Code::Invalid;
	}

	if (m_p->m_threadUDP) {
		return Code::Busy;
	}

	m_p->m_socketUDP.reset();

	return Code::Success;
}

EXPORT Code Server::sendUDP(const Endpoint &endpoint, const BufRefConst data) {
	return m_p->m_socketUDP->write(endpoint, data);
}

void P::tcpThread() {
	if (m_feedbackTCP.started) {
		m_feedbackTCP.started();
	}

	auto state = Socket::InReady;

	while (!m_threadTCP->interruption_requested()) {
		while (state & Socket::InReady) {
			Endpoint endpoint;
			const auto ret = m_socketTCP->accept(endpoint);

			const auto code = Socket::osErrorToCode(ret.first);
			switch (code) {
				case Code::Success: {
					if (!m_feedbackTCP.connection) {
						continue;
					}

					if (!m_feedbackTCP.connection(endpoint) || !m_feedbackTCP.session) {
						continue;
					}

					const auto p = new Session::P(std::move(*ret.second), m_socketUDP, true);
					if (!m_feedbackTCP.session(p)) {
						delete p;
					}

					break;
				}
				case Code::Timeout:
				case Code::Cancel:
				case Code::Retry:
				case Code::Busy:
				case Code::Disconnect:
					state = Socket::Timeout;
					break;
				default:
					m_feedbackTCP.failed(code);
					return;
			}
		}

		state = m_socketTCP->wait(true, false, m_feedbackTCP.timeout ? m_feedbackTCP.timeout() : infinite32);
	}

	if (m_feedbackTCP.stopped) {
		m_feedbackTCP.stopped();
	}
}

void P::udpThread() {
	if (m_feedbackUDP.started) {
		m_feedbackUDP.started();
	}

	FixedBuf< 1024 > buf;

	auto state = Socket::InReady;

	while (!m_threadUDP->interruption_requested()) {
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
