// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "TLS.hpp"

#include "mumble/Cert.hpp"
#include "mumble/Key.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>

using namespace mumble;

using Code = SocketTLS::Code;

SocketTLS::SocketTLS(SocketTLS &&socket)
	: SocketTCP(std::move(socket)), m_ssl(std::exchange(socket.m_ssl, nullptr)),
	  m_sslCtx(std::exchange(socket.m_sslCtx, nullptr)), m_closed(socket.m_closed.load()) {
}

SocketTLS::SocketTLS(const int32_t fd, const bool server)
	: SocketTCP(fd), m_ssl(nullptr), m_sslCtx(nullptr), m_closed(true) {
	if (!*static_cast< SocketTCP * >(this)) {
		return;
	}

	m_sslCtx = SSL_CTX_new(server ? TLS_server_method() : TLS_client_method());
	if (!m_sslCtx) {
		return;
	}

	m_ssl = SSL_new(m_sslCtx);
	if (!m_ssl) {
		return;
	}

	SSL_set_fd(m_ssl, m_fd);
	SSL_set_read_ahead(m_ssl, 1);
	SSL_set_options(m_ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
	SSL_set_verify(m_ssl, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE, verifyCallback);
}

SocketTLS::~SocketTLS() {
	if (m_ssl) {
		SSL_free(m_ssl);
	}

	if (m_sslCtx) {
		SSL_CTX_free(m_sslCtx);
	}
}

SocketTLS::operator bool() const {
	return m_ssl;
}

bool SocketTLS::isServer() const {
	return SSL_is_server(m_ssl);
}

bool SocketTLS::setCert(const Cert::Chain &cert, const Key &key) {
	if (!cert.size()) {
		return false;
	}

	if (SSL_use_certificate(m_ssl, static_cast< X509 * >(cert[0].handle())) <= 0) {
		return false;
	}

	if (cert.size() >= 2) {
		std::for_each(cert.cbegin() + 1, cert.cend(),
					  [this](const Cert &cert) { SSL_add1_chain_cert(m_ssl, cert.handle()); });
	}

	if (SSL_use_PrivateKey(m_ssl, static_cast< EVP_PKEY * >(key.handle())) <= 0) {
		return false;
	}

	if (SSL_check_private_key(m_ssl) <= 0) {
		return false;
	}

	return true;
}

Cert::Chain SocketTLS::peerCert() const {
	Cert::Chain cert;

	const auto stack = SSL_get0_verified_chain(m_ssl);
	for (int i = 0; i < sk_X509_num(stack); ++i) {
		const auto x509 = sk_X509_value(stack, i);
		if (X509_up_ref(x509)) {
			cert.push_back(x509);
		}
	}

	return cert;
}

uint32_t SocketTLS::pending() const {
	return SSL_pending(m_ssl);
}

::Code SocketTLS::accept() {
	ERR_clear_error();

	const auto code = interpretLibCode(SSL_accept(m_ssl));
	if (code == Code::Success) {
		m_closed = false;
	}

	return code;
}

::Code SocketTLS::connect() {
	ERR_clear_error();

	const auto code = interpretLibCode(SSL_connect(m_ssl));
	if (code == Code::Success) {
		m_closed = false;
	}

	return code;
}

::Code SocketTLS::disconnect() {
	if (m_closed) {
		return Code::Success;
	}

	m_closed = true;

	ERR_clear_error();

	return interpretLibCode(SSL_shutdown(m_ssl));
}

::Code SocketTLS::read(BufRef &buf) {
	ERR_clear_error();

	size_t read   = 0;
	const int ret = SSL_read_ex(m_ssl, buf.data(), buf.size(), &read);

	buf = buf.subspan(read);

	return interpretLibCode(ret, read, buf.size());
}

::Code SocketTLS::write(BufRefConst &buf) {
	ERR_clear_error();

	size_t written = 0;
	const int ret  = SSL_write_ex(m_ssl, buf.data(), buf.size(), &written);

	buf = buf.subspan(written);

	return interpretLibCode(ret, written, buf.size());
}

::Code SocketTLS::interpretLibCode(const int code, const bool processed, const bool remaining) {
	switch (SSL_get_error(m_ssl, code)) {
		case SSL_ERROR_NONE:
			if (processed) {
				return remaining ? Retry : Success;
			}

			return Shutdown;
		case SSL_ERROR_ZERO_RETURN:
			return Shutdown;
		case SSL_ERROR_WANT_READ:
			return WaitIn;
		case SSL_ERROR_WANT_WRITE:
			return WaitOut;
		case SSL_ERROR_SYSCALL:
			m_closed = true;

			if (!processed) {
				return Shutdown;
			}
		case SSL_ERROR_SSL:
			return Failure;
	}

	return Unknown;
};

int SocketTLS::verifyCallback(int, X509_STORE_CTX *) {
	return 1;
}
