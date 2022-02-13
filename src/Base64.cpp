// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Base64.hpp"

#include "mumble/Types.hpp"

#include <cmath>
#include <cstddef>
#include <memory>

#include <openssl/evp.h>

#define CHECK      \
	if (!*this) {  \
		return {}; \
	}

#define CAST_BUF(var) (reinterpret_cast< unsigned char * >(var))
#define CAST_BUF_CONST(var) (reinterpret_cast< const unsigned char * >(var))

using namespace mumble;

using P = Base64::P;

EXPORT Base64::Base64() : m_p(new P) {
}

EXPORT Base64::~Base64() = default;

EXPORT Base64::operator bool() {
	return m_p && m_p->m_ctx;
}

EXPORT size_t Base64::decode(const BufRef out, const BufRefConst in) {
	CHECK

	if (!out.size()) {
		// 4 input bytes = max. 3 output bytes.
		//
		// EVP_DecodeUpdate() ignores:
		// - Leading/trailing whitespace.
		// - Trailing newlines, carriage returns or EOF characters.
		//
		// EVP_DecodeFinal() fails if the input is not divisible by 4.
		return in.size() / 4 * 3;
	}

	// We don't use EVP_DecodeBlock() because it adds padding if the output is not divisible by 3.
	EVP_DecodeInit(m_p->m_ctx);

	int written_1;
	if (EVP_DecodeUpdate(m_p->m_ctx, CAST_BUF(out.data()), &written_1, CAST_BUF_CONST(in.data()), in.size()) < 0) {
		return {};
	}

	int written_2;
	if (EVP_DecodeFinal(m_p->m_ctx, CAST_BUF(out.data()), &written_2) <= 0) {
		return {};
	}

	return written_1 + written_2;
}

EXPORT size_t Base64::encode(const BufRef out, const BufRefConst in) {
	if (!out.size()) {
		// 3 input bytes = 4 output bytes.
		// +1 for the NUL terminator.
		//
		// EVP_EncodeBlock() adds padding when the input is not divisible by 3.
		return ceilf(in.size() / 3.f) * 4 + 1;
	}

	// EVP_EncodeBlock() returns the length of the string without the NUL terminator.
	const auto written = EVP_EncodeBlock(CAST_BUF(out.data()), CAST_BUF_CONST(in.data()), in.size());
	if (written < 0) {
		return {};
	}

	return written + 1;
}

P::P() : m_ctx(EVP_ENCODE_CTX_new()) {
}

P::~P() {
	if (m_ctx) {
		EVP_ENCODE_CTX_free(m_ctx);
	}
}
