// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Hash.hpp"

#include "mumble/Types.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include <openssl/evp.h>

#define CHECK      \
	if (!*this) {  \
		return {}; \
	}

#define CAST_BUF(var) (reinterpret_cast< unsigned char * >(var))

using namespace mumble;

using P = Hash::P;

EXPORT Hash::Hash(Hash &&crypt) : m_p(std::exchange(crypt.m_p, nullptr)) {
}

EXPORT Hash::Hash() : m_p(new P) {
}

EXPORT Hash::~Hash() = default;

EXPORT Hash::operator bool() const {
	return m_p && *m_p;
}

EXPORT Hash &Hash::operator=(Hash &&crypt) {
	m_p = std::exchange(crypt.m_p, nullptr);
	return *this;
}

EXPORT size_t Hash::operator()(const BufRef out, const BufRefConst in) {
	CHECK

	if (!out.size()) {
		return EVP_MD_CTX_size(m_p->m_ctx);
	}

	if (EVP_DigestInit_ex(m_p->m_ctx, nullptr, nullptr) <= 0) {
		return {};
	}

	if (EVP_DigestUpdate(m_p->m_ctx, in.data(), in.size()) <= 0) {
		return {};
	}

	uint32_t written;
	if (EVP_DigestFinal_ex(m_p->m_ctx, CAST_BUF(out.data()), &written) <= 0) {
		return {};
	}

	return written;
}

EXPORT void *Hash::handle() const {
	CHECK

	return m_p->m_ctx;
}

EXPORT std::string_view Hash::type() const {
	CHECK

	return m_p->type();
}

EXPORT bool Hash::setType(const std::string_view name) {
	CHECK

	return m_p->setType(name);
}

EXPORT uint32_t Hash::blockSize() const {
	CHECK

	return EVP_MD_CTX_block_size(m_p->m_ctx);
}

EXPORT bool Hash::reset() {
	CHECK

	return EVP_MD_CTX_reset(m_p->m_ctx) > 0;
}

P::P() : m_ctx(EVP_MD_CTX_new()) {
	if (m_ctx) {
		setType();
	}
}

P::~P() {
	if (m_ctx) {
		EVP_MD_CTX_free(m_ctx);
	}
}

P::operator bool() {
	return m_ctx && EVP_MD_CTX_md(m_ctx);
}

std::string_view P::type() {
	const auto type = EVP_MD_CTX_md(m_ctx);
	if (type == EVP_md_null()) {
		return {};
	}

	return EVP_MD_name(type);
}

bool P::setType(const std::string_view name) {
	const auto type = name.empty() ? EVP_md_null() : EVP_get_digestbyname(name.data());
	if (!type) {
		return false;
	}

	return EVP_DigestInit_ex(m_ctx, type, nullptr) > 0;
}
