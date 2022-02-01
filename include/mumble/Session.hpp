// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SESSION_HPP
#define MUMBLE_SESSION_HPP

#include "Cert.hpp"
#include "Key.hpp"
#include "Mumble.hpp"

#include <functional>
#include <stop_token>

namespace mumble {
class EXPORT Session {
public:
	class P;

	struct Feedback {
		std::function< void() > opened;
		std::function< void() > closed;

		std::function< void(Code) > failed;

		std::function< uint32_t() > timeout;
		std::function< uint32_t() > timeouts;

		std::function< void(Message *) > message;
	};

	Session(Session &&session);
	Session(P *p);
	virtual ~Session();

	virtual Endpoint endpoint() const;
	virtual Endpoint peerEndpoint() const;

	virtual Cert::Chain peerCert() const;

	virtual Code start(const Feedback &feedback, const Cert::Chain &cert = {}, const Key &key = {});

	virtual Code sendTCP(const Message &message, const std::stop_token &stopToken = {});
	virtual Code sendUDP(const Endpoint &endpoint, const BufRefConst data);

private:
	Session(const Session &) = delete;
	virtual Session &operator=(const Session &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
