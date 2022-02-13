// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Session.hpp"

#include "Connection.hpp"
#include "Pack.hpp"
#include "TCP.hpp"
#include "TLS.hpp"
#include "UDP.hpp"

#include "mumble/Cert.hpp"
#include "mumble/Key.hpp"
#include "mumble/Message.hpp"
#include "mumble/Types.hpp"

#include <atomic>
#include <cstdint>
#include <functional>
#include <utility>

using namespace mumble;

using P = Session::P;

EXPORT Session::Session(Session &&session) {
	m_p = std::exchange(session.m_p, nullptr);
}

EXPORT Session::Session(P *p) : m_p(p) {
}

EXPORT Session::~Session() = default;

EXPORT Endpoint Session::endpoint() const {
	Endpoint endpoint;
	m_p->m_connection->getEndpoint(endpoint);
	return endpoint;
}

EXPORT Endpoint Session::peerEndpoint() const {
	Endpoint endpoint;
	m_p->m_connection->getPeerEndpoint(endpoint);
	return endpoint;
}

EXPORT Cert::Chain Session::peerCert() const {
	return m_p->m_connection->peerCert();
}

EXPORT Code Session::start(const Feedback &feedback, const Cert::Chain &cert, const Key &key) {
	m_p->m_feedback = feedback;

	if (!cert.empty() && key) {
		if (!m_p->m_connection->setCert(cert, key)) {
			return Code::Ssl;
		}
	}

	Connection::Feedback connFeedback;

	connFeedback.opened   = feedback.opened ? feedback.opened : []() {};
	connFeedback.closed   = feedback.closed ? feedback.closed : []() {};
	connFeedback.failed   = feedback.failed ? feedback.failed : [](Code) {};
	connFeedback.timeout  = [this]() { return m_p->m_feedback.timeout ? m_p->m_feedback.timeout() : infinite32; };
	connFeedback.timeouts = [this]() { return m_p->m_feedback.timeouts ? m_p->m_feedback.timeouts() : infinite32; };
	connFeedback.message  = [this](const Pack &pack) {
        if (m_p->m_feedback.message) {
            const auto message = pack.process();
            if (message) {
                m_p->m_feedback.message(message);
            }
        }
	};

	m_p->m_connection->start(connFeedback);

	return Code::Success;
}

EXPORT Code Session::sendTCP(const Message &message, const std::atomic_bool &halt) {
	const Pack pack(message);
	return m_p->m_connection->write(pack.buf(), halt);
}

EXPORT Code Session::sendUDP(const Endpoint &endpoint, const BufRefConst data) {
	if (!m_p->m_socketUDP) {
		return Code::Init;
	}

	auto buf = data;

	return m_p->m_socketUDP->write(endpoint, buf);
}

P::P(SocketTCP &&socketTCP, const std::shared_ptr< SocketUDP > &socketUDP, const bool server)
	: m_socketUDP(socketUDP), m_connection(new Connection(SocketTLS(std::move(socketTCP), server))) {
}
