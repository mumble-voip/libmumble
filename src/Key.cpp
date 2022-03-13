// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Key.hpp"

#include <cstring>
#include <memory>
#include <string>
#include <utility>

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#define CHECK      \
	if (!*this) {  \
		return {}; \
	}

using namespace mumble;

using P = Key::P;

EXPORT Key::Key() : m_p(new P(EVP_PKEY_new())) {
}

EXPORT Key::Key(const Key &key) : m_p(new P(key.pem(), key.isPrivate())) {
}

EXPORT Key::Key(Key &&key) : m_p(std::exchange(key.m_p, nullptr)) {
}

EXPORT Key::Key(void *handle) : m_p(new P(static_cast< EVP_PKEY * >(handle))) {
}

EXPORT Key::Key(const std::string_view pem, const bool isPrivate, std::string_view password)
	: m_p(new P(pem, isPrivate, password)) {
}

EXPORT Key::~Key() = default;

EXPORT Key::operator bool() const {
	return m_p && m_p->m_pkey;
}

EXPORT Key &Key::operator=(const Key &key) {
	// TODO: Switch to EVP_PKEY_dup() once available (OpenSSL 3.0.0).
	m_p = std::make_unique< P >(key.pem(), key.isPrivate());
	return *this;
}

EXPORT Key &Key::operator=(Key &&key) {
	m_p = std::exchange(key.m_p, nullptr);
	return *this;
}

EXPORT bool Key::operator==(const Key &key) const {
	if (!*this || !key) {
		return !*this && !key;
	}

	return EVP_PKEY_cmp(m_p->m_pkey, key.m_p->m_pkey) == 0;
}

EXPORT void *Key::handle() const {
	return m_p->m_pkey;
}

EXPORT bool Key::isPrivate() const {
	CHECK

	return i2d_PrivateKey(m_p->m_pkey, nullptr) > 0;
}

EXPORT std::string Key::pem() const {
	CHECK

	auto bio = BIO_new(BIO_s_secmem());
	if (!bio) {
		return {};
	}

	int ret;

	if (isPrivate()) {
		ret = PEM_write_bio_PrivateKey(bio, m_p->m_pkey, nullptr, nullptr, 0, nullptr, nullptr);
	} else {
		ret = PEM_write_bio_PUBKEY(bio, m_p->m_pkey);
	}

	std::string pem;

	if (ret > 0) {
		BUF_MEM *mem;
		if (BIO_get_mem_ptr(bio, &mem) > 0) {
			pem.append(mem->data, mem->length);
		}
	}

	BIO_free_all(bio);

	return pem;
}

P::P(EVP_PKEY *pkey) : m_pkey(pkey) {
}

P::P(const std::string_view pem, const bool isPrivate, std::string_view password) : m_pkey(nullptr) {
	auto bio = BIO_new_mem_buf(pem.data(), pem.size());
	if (!bio) {
		return;
	}

	if (isPrivate) {
		PEM_read_bio_PrivateKey(bio, &m_pkey, &P::passwordCallback, &password);
	} else {
		PEM_read_bio_PUBKEY(bio, &m_pkey, &P::passwordCallback, &password);
	}

	BIO_free_all(bio);
}

P::~P() {
	if (m_pkey) {
		EVP_PKEY_free(m_pkey);
	}
}

int P::passwordCallback(char *buf, const int32_t size, int, void *userdata) {
	auto password = static_cast< const std::string_view * >(userdata);

	auto length = password->size();
	if (length > static_cast< uint32_t >(size)) {
		length = size;
	}

	memcpy(buf, password->data(), length);

	return length;
}
