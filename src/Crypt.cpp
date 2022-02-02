// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Crypt.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

#define CHECK      \
	if (!*this) {  \
		return {}; \
	}

#define CAST_BUF(var) (reinterpret_cast< unsigned char * >(var))
#define CAST_BUF_CONST(var) (reinterpret_cast< const unsigned char * >(var))

using namespace mumble;

using P = Crypt::P;

EXPORT Crypt::Crypt(Crypt &&crypt) : m_p(std::exchange(crypt.m_p, nullptr)) {
}

EXPORT Crypt::Crypt() : m_p(new P) {
}

EXPORT Crypt::~Crypt() = default;

EXPORT Crypt::operator bool() const {
	return m_p && *m_p;
}

EXPORT Crypt &Crypt::operator=(Crypt &&crypt) {
	m_p = std::exchange(crypt.m_p, nullptr);
	return *this;
}

EXPORT void *Crypt::handle() const {
	CHECK

	return m_p->m_ctx;
}

EXPORT std::string_view Crypt::cipher() const {
	CHECK

	return m_p->cipher();
}

EXPORT bool Crypt::setCipher(const std::string_view name) {
	CHECK

	return m_p->setCipher(name);
}

EXPORT uint32_t Crypt::blockSize() const {
	CHECK

	return m_p->blockSize();
}

EXPORT uint32_t Crypt::keySize() const {
	CHECK

	return m_p->m_key.size();
}

EXPORT uint32_t Crypt::nonceSize() const {
	CHECK

	return m_p->m_nonce.size();
}

EXPORT BufRefConst Crypt::key() const {
	CHECK

	return m_p->m_key;
}

EXPORT Buf Crypt::genKey() const {
	const auto size = keySize();
	if (!size) {
		return {};
	}

	Buf key(size);
	if (EVP_CIPHER_CTX_rand_key(m_p->m_ctx, CAST_BUF(key.data())) <= 0) {
		return {};
	}

	return key;
}

EXPORT bool Crypt::setKey(const BufRefConst key) {
	CHECK

	if (m_p->m_key.size() != key.size()) {
		if (EVP_CIPHER_CTX_set_key_length(m_p->m_ctx, key.size()) <= 0) {
			return false;
		}

		m_p->m_key.resize(key.size());
	}

	m_p->m_key.assign(key.begin(), key.end());

	return EVP_CipherInit_ex(m_p->m_ctx, nullptr, nullptr, CAST_BUF_CONST(key.data()), nullptr, -1) > 0;
}

EXPORT BufRefConst Crypt::nonce() const {
	CHECK

	return m_p->m_nonce;
}

EXPORT Buf Crypt::genNonce() const {
	const auto size = nonceSize();
	if (!size) {
		return {};
	}

	Buf nonce(size);
	if (RAND_priv_bytes(CAST_BUF(nonce.data()), nonce.size()) <= 0) {
		return {};
	}

	return nonce;
}

EXPORT bool Crypt::setNonce(const BufRefConst nonce) {
	CHECK

	if (m_p->m_nonce.size() != nonce.size()) {
		if (EVP_CIPHER_CTX_ctrl(m_p->m_ctx, EVP_CTRL_AEAD_SET_IVLEN, nonce.size(), nullptr) <= 0) {
			return false;
		}

		m_p->m_nonce.resize(nonce.size());
	}

	m_p->m_nonce.assign(nonce.begin(), nonce.end());

	return EVP_CipherInit_ex(m_p->m_ctx, nullptr, nullptr, nullptr, CAST_BUF_CONST(nonce.data()), -1) > 0;
}

EXPORT bool Crypt::padding() const {
	return m_p->m_padding;
}

EXPORT bool Crypt::togglePadding(const bool enable) {
	CHECK

	m_p->m_padding = enable;

	return true;
}

EXPORT bool Crypt::reset() {
	CHECK

	return EVP_CIPHER_CTX_reset(m_p->m_ctx) > 0;
}

EXPORT size_t Crypt::decrypt(const BufRef out, const BufRefConst in, const BufRefConst tag, const BufRefConst aad) {
	CHECK

	return m_p->process(false, out, in, { const_cast< std::byte * >(tag.data()), tag.size() }, aad);
}

EXPORT size_t Crypt::encrypt(const BufRef out, const BufRefConst in, const BufRef tag, const BufRefConst aad) {
	CHECK

	return m_p->process(true, out, in, tag, aad);
}

P::P() : m_padding(true), m_ctx(EVP_CIPHER_CTX_new()) {
	if (m_ctx) {
		setCipher();
	}
}

P::~P() {
	if (m_ctx) {
		EVP_CIPHER_CTX_free(m_ctx);
	}
}

P::operator bool() {
	return m_ctx && EVP_CIPHER_CTX_cipher(m_ctx);
}

uint32_t P::blockSize() const {
	const auto size = EVP_CIPHER_CTX_block_size(m_ctx);
	return size >= 0 ? size : 0;
}

std::string_view P::cipher() {
	const auto cipher = EVP_CIPHER_CTX_cipher(m_ctx);
	if (cipher == EVP_enc_null()) {
		return {};
	}

	return EVP_CIPHER_name(cipher);
}

bool P::setCipher(const std::string_view name) {
	const auto cipher = name.empty() ? EVP_enc_null() : EVP_get_cipherbyname(name.data());
	if (!cipher) {
		return false;
	}

	if (EVP_CipherInit_ex(m_ctx, cipher, nullptr, nullptr, nullptr, -1) <= 0) {
		return false;
	}

	m_key.resize(EVP_CIPHER_CTX_key_length(m_ctx));
	m_nonce.resize(EVP_CIPHER_CTX_iv_length(m_ctx));

	m_key   = {};
	m_nonce = {};

	return true;
}

size_t P::process(const bool encrypt, const BufRef out, const BufRefConst in, const BufRef tag, const BufRefConst aad) {
	if (!out.size()) {
		if (m_padding) {
			const auto size = blockSize();
			return size > 1 ? in.size() + size : in.size();
		} else {
			return in.size();
		}
	}

	if (static_cast< bool >(EVP_CIPHER_CTX_encrypting(m_ctx)) == encrypt) {
		if (EVP_CipherInit_ex(m_ctx, nullptr, nullptr, nullptr, nullptr, encrypt) <= 0) {
			return {};
		}
	} else {
		// Certain cipher implementations require resetting the key, nonce or both when switching operation mode.
		// Not doing so doesn't cause any apparent failure, until you realize the output is messed up.
		if (EVP_CipherInit_ex(m_ctx, nullptr, nullptr, CAST_BUF_CONST(m_key.data()), CAST_BUF_CONST(m_nonce.data()),
							  encrypt)
			<= 0) {
			return {};
		}
	}

	if (EVP_CIPHER_CTX_set_padding(m_ctx, m_padding) <= 0) {
		return {};
	}

	if (!encrypt && !tag.empty()) {
		if (EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_SET_TAG, tag.size(), tag.data()) <= 0) {
			return {};
		}
	}

	int written1;

	if (!aad.empty()) {
		if (EVP_CipherUpdate(m_ctx, nullptr, &written1, CAST_BUF_CONST(aad.data()), aad.size()) <= 0) {
			return {};
		}
	}

	if (EVP_CipherUpdate(m_ctx, CAST_BUF(out.data()), &written1, CAST_BUF_CONST(in.data()), in.size()) <= 0) {
		return {};
	}

	int written2;

	if (EVP_CipherFinal_ex(m_ctx, CAST_BUF(out.data() + written1), &written2) <= 0) {
		return {};
	}

	if (encrypt && !tag.empty()) {
		if (EVP_CIPHER_CTX_ctrl(m_ctx, EVP_CTRL_AEAD_GET_TAG, tag.size(), tag.data()) <= 0) {
			return {};
		}
	}

	return written1 + written2;
}
