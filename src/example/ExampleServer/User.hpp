// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_EXAMPLESERVER_USER_HPP
#define MUMBLE_EXAMPLESERVER_USER_HPP

#include "Endpoints.hpp"

#include "mumble/CryptOCB2.hpp"
#include "mumble/Session.hpp"

#include <boost/thread/thread.hpp>

#include <rigtorp/MPMCQueue.h>

class User : public mumble::Session {
public:
	using Buf         = mumble::Buf;
	using BufRef      = mumble::BufRef;
	using BufRefConst = mumble::BufRefConst;

	struct Packet {
		mumble::Endpoint endpoint;
		mumble::Buf buf;

		Packet() noexcept = default;

		Packet(const Packet &packet) noexcept : endpoint(packet.endpoint), buf(packet.buf) {}

		Packet(const mumble::Endpoint &endpoint, const BufRefConst buf) noexcept
			: endpoint(endpoint), buf(buf.begin(), buf.end()) {}

		Packet &operator=(const Packet &&packet) noexcept {
			endpoint = packet.endpoint;
			buf      = std::move(packet.buf);

			return *this;
		}
	};

	User(P *p, const uint32_t id);
	virtual ~User();

	uint32_t id() const;

	bool cryptOK() const;

	uint32_t good() const;
	uint32_t late() const;
	uint32_t lost() const;

	BufRefConst key() const;

	BufRefConst decryptNonce() const;
	BufRefConst encryptNonce() const;

	size_t decrypt(const BufRef out, const BufRefConst in);
	size_t encrypt(const BufRef out, const BufRefConst in);

	const Endpoints &endpoints() const;
	void addEndpoint(const Endpoint &endpoint);
	void delEndpoint(const Endpoint &endpoint);

	void send(const mumble::Message &message);
	void send(const Packet &packet);

private:
	void thread();

	uint32_t m_id;
	bool m_cryptOK;
	uint32_t m_good;
	uint32_t m_late;
	uint32_t m_lost;
	Endpoints m_endpoints;
	rigtorp::MPMCQueue< Packet > m_packets;
	boost::condition_variable m_cond;
	boost::thread m_thread;
	mumble::CryptOCB2 m_decrypt;
	mumble::CryptOCB2 m_encrypt;
	std::vector< std::byte > m_decryptNonce;
	std::vector< std::byte > m_encryptNonce;
	std::array< uint8_t, UINT8_MAX + 1 > m_decryptHistory;
};

#endif
