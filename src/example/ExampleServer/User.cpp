// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "User.hpp"

using namespace mumble;

User::User(P *p, const uint32_t id)
	: Session(p), m_id(id), m_cryptOK(false), m_good(0), m_late(0), m_lost(0), m_packets(32),
	  m_thread(&User::thread, this), m_decryptHistory({}) {
	const auto key = m_decrypt.genKey();
	if (!m_decrypt.setKey(key) || !m_encrypt.setKey(key)) {
		return;
	}

	m_decryptNonce = m_decrypt.genNonce();
	m_encryptNonce = m_encrypt.genNonce();
	if (!m_decrypt.setNonce(m_decryptNonce) || !m_encrypt.setNonce(m_encryptNonce)) {
		return;
	}

	m_cryptOK = true;
}

User::~User() {
	m_thread.interrupt();
}

uint32_t User::id() const {
	return m_id;
}

bool User::cryptOK() const {
	return m_cryptOK;
}

uint32_t User::good() const {
	return m_good;
}

uint32_t User::late() const {
	return m_late;
}

uint32_t User::lost() const {
	return m_lost;
}

BufRefConst User::key() const {
	return m_decrypt.key();
}

BufRefConst User::decryptNonce() const {
	return m_decrypt.nonce();
}

BufRefConst User::encryptNonce() const {
	return m_encrypt.nonce();
}

size_t User::decrypt(const BufRef out, const BufRefConst in) {
	// The checksum header occupies 4 bytes.
	if (in.size() < 4) {
		return {};
	}

	const auto nonceByte = static_cast< uint8_t >(in[0]);
	const auto tag       = in.subspan(1, 3);
	const auto encrypted = in.subspan(4);
	const auto prevNonce = m_decryptNonce;

	boost::span< uint8_t > nonce(reinterpret_cast< uint8_t * >(m_decryptNonce.data()), m_decryptNonce.size());

	bool restore = false;

	int32_t lost = 0;
	int32_t late = 0;

	if (((nonce[0] + 1) & 0xFF) == nonceByte) {
		// In order as expected.
		if (nonceByte > nonce[0]) {
			nonce[0] = nonceByte;
		} else if (nonceByte < nonce[0]) {
			nonce[0] = nonceByte;
			for (int i = 1; i < nonce.size(); i++)
				if (++nonce[i])
					break;
		} else {
			return {};
		}
	} else {
		// This is either out of order or a repeat.

		int diff = nonceByte - nonce[0];
		if (diff > 128)
			diff = diff - 256;
		else if (diff < -128)
			diff = diff + 256;

		if ((nonceByte < nonce[0]) && (diff > -30) && (diff < 0)) {
			// Late packet, but no wraparound.
			late     = 1;
			lost     = -1;
			nonce[0] = nonceByte;
			restore  = true;
		} else if ((nonceByte > nonce[0]) && (diff > -30) && (diff < 0)) {
			// Last was 0x02, here comes 0xff from last round
			late     = 1;
			lost     = -1;
			nonce[0] = nonceByte;
			for (int i = 1; i < nonce.size(); i++) {
				if (nonce[i]--)
					break;
			}

			restore = true;
		} else if ((nonceByte > nonce[0]) && (diff > 0)) {
			// Lost a few packets, but beyond that we're good.
			lost     = nonceByte - nonce[0] - 1;
			nonce[0] = nonceByte;
		} else if ((nonceByte < nonce[0]) && (diff > 0)) {
			// Lost a few packets, and wrapped around
			lost     = 256 - nonce[0] + nonceByte - 1;
			nonce[0] = nonceByte;
			for (int i = 1; i < nonce.size(); i++)
				if (++nonce[i])
					break;
		} else {
			return {};
		}

		if (m_decryptHistory[nonce[0]] == nonce[1]) {
			m_decryptNonce = prevNonce;
			return {};
		}
	}

	if (!m_decrypt.setNonce(m_decryptNonce)) {
		return {};
	}

	const auto written = m_decrypt.decrypt(out, encrypted, tag);
	if (!written) {
		m_decryptNonce = prevNonce;
		return {};
	}

	m_decryptHistory[nonce[0]] = nonce[1];

	if (restore) {
		m_decryptNonce = prevNonce;
	}

	++m_good;
	m_late += late;
	m_lost += lost;

	return written;
}

size_t User::encrypt(const BufRef out, const BufRefConst in) {
	// The maximum packet size allowed in the Mumble protocol is 1024 bytes.
	// 4 bytes are used for the checksum header, leaving 1020 bytes for the data.
	if (out.size() < 4 || in.size() > 1020) {
		return {};
	}

	for (auto &byte : m_encryptNonce) {
		if (++*reinterpret_cast< uint8_t * >(&byte)) {
			break;
		}
	}

	if (!m_encrypt.setNonce(m_encryptNonce)) {
		return {};
	}

	const auto encrypted = out.subspan(4);
	if (encrypted.empty()) {
		return {};
	}

	Buf tag(m_encrypt.blockSize());

	const auto written = m_encrypt.encrypt(encrypted, in, tag);
	if (!written) {
		return {};
	}

	out[0] = m_encryptNonce[0];
	std::copy_n(tag.cbegin(), 3, out.begin() + 1);

	return written + 4;
}

const Endpoints &User::endpoints() const {
	return m_endpoints;
}

void User::addEndpoint(const Endpoint &endpoint) {
	m_endpoints.emplace(endpoint);
}

void User::delEndpoint(const Endpoint &endpoint) {
	m_endpoints.extract(endpoint);
}

void User::send(const mumble::Message &message) {
	sendTCP(message);
}

void User::send(const Packet &packet) {
	m_packets.push(packet);
	m_cond.notify_all();
}

void User::thread() {
	boost::mutex mutex;

	while (!m_thread.interruption_requested()) {
		boost::unique_lock< boost::mutex > lock(mutex);

		m_cond.wait(lock);

		Packet packet;
		if (!m_packets.try_pop(packet)) {
			continue;
		}

		sendUDP(packet.endpoint, packet.buf);
	}
}
