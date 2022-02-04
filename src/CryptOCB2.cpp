// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "CryptOCB2.hpp"

#include "mumble/Endian.hpp"

#include <algorithm>

#include <openssl/evp.h>
#include <openssl/rand.h>

#define CHECK      \
	if (!*this) {  \
		return {}; \
	}

#define CAST_BUF(var) (reinterpret_cast< unsigned char * >(var))
#define CAST_BUF_CONST(var) (reinterpret_cast< const unsigned char * >(var))

using namespace mumble;

using P = CryptOCB2::P;

using KeyBlock         = P::KeyBlock;
using KeyBlockRef      = P::KeyBlockRef;
using KeyBlockRefConst = P::KeyBlockRefConst;
using SubBlock         = P::SubBlock;

EXPORT CryptOCB2::CryptOCB2() : m_p(new P) {
}

EXPORT CryptOCB2::~CryptOCB2() = default;

EXPORT CryptOCB2::operator bool() const {
	return m_p && m_p->m_ok;
}

EXPORT uint32_t CryptOCB2::blockSize() const {
	CHECK

	return P::blockSize;
}

EXPORT uint32_t CryptOCB2::keySize() const {
	CHECK

	return P::keySize;
}

EXPORT uint32_t CryptOCB2::nonceSize() const {
	CHECK

	return P::nonceSize;
}

EXPORT BufRefConst CryptOCB2::key() const {
	CHECK

	return m_p->m_key;
}

EXPORT Buf CryptOCB2::genKey() const {
	CHECK

	Buf key(P::keySize);
	if (EVP_CIPHER_CTX_rand_key(m_p->m_ctx, CAST_BUF(key.data())) <= 0) {
		return {};
	}

	return key;
}

EXPORT bool CryptOCB2::setKey(const BufRefConst key) {
	CHECK

	if (key.size() != P::keySize) {
		return false;
	}

	std::copy(key.begin(), key.end(), m_p->m_key.begin());

	return EVP_CipherInit_ex(m_p->m_ctx, nullptr, nullptr, CAST_BUF_CONST(key.data()), nullptr, -1) > 0;
}

EXPORT BufRefConst CryptOCB2::nonce() const {
	CHECK

	return m_p->m_nonce;
}

EXPORT Buf CryptOCB2::genNonce() const {
	CHECK

	Buf nonce(P::nonceSize);
	if (RAND_priv_bytes(reinterpret_cast< unsigned char * >(nonce.data()), nonce.size()) <= 0) {
		return {};
	}

	return nonce;
}

EXPORT bool CryptOCB2::setNonce(const BufRefConst nonce) {
	CHECK

	if (nonce.size() != P::nonceSize) {
		return false;
	}

	std::copy(nonce.begin(), nonce.end(), m_p->m_nonce.begin());

	return true;
}

EXPORT size_t CryptOCB2::decrypt(BufRef out, BufRefConst in, const BufRefConst tag) {
	CHECK

	if (!out.size()) {
		return in.size();
	}

	KeyBlock delta;
	const auto deltaBytes = boost::as_writable_bytes(KeyBlockRef(delta));

	if (!m_p->process(true, deltaBytes, m_p->m_nonce)) {
		return {};
	}

	size_t written = 0;

	KeyBlock checksum{}, tmp;
	const auto tmpBytes = boost::as_writable_bytes(KeyBlockRef(tmp));

	while (in.size() > P::blockSize) {
		P::s2(delta);
		P::xorBlock(tmp, delta, P::toBlockRef(in));

		if (!m_p->process(false, tmpBytes, tmpBytes)) {
			return {};
		}

		P::xorBlock(P::toBlockRef(out), delta, tmp);
		written += P::blockSize;

		P::xorBlock(checksum, checksum, P::toBlockRef(out));

		in  = in.subspan(P::blockSize);
		out = out.subspan(P::blockSize);
	}

	P::s2(delta);

	tmp        = {};
	tmp.back() = Endian::swap(static_cast< SubBlock >(in.size() * 8));
	P::xorBlock(tmp, tmp, delta);

	KeyBlock pad;
	const auto padBytes = boost::as_writable_bytes(KeyBlockRef(pad));

	if (!m_p->process(true, padBytes, tmpBytes)) {
		return {};
	}

	tmp = {};
	std::copy(in.begin(), in.end(), tmpBytes.begin());
	P::xorBlock(tmp, tmp, pad);
	P::xorBlock(checksum, checksum, tmp);

	// Counter-cryptanalysis described in section 9 of https://eprint.iacr.org/2019/311
	// In an attack, the decrypted last block would need to equal `delta ^ len(128)`.
	// With a bit of luck (or many packets), smaller values than 128 (i.e. non-full blocks) are also
	// feasible, so we check `tmp` instead of `plain`.
	// Since our `len` only ever modifies the last byte, we simply check all remaining ones.
	if (std::equal(tmpBytes.begin(), tmpBytes.begin() + P::blockSize, deltaBytes.begin())) {
		return {};
	}

	std::copy_n(tmpBytes.begin(), in.size(), out.begin());
	written += in.size();

	if (tag.empty()) {
		return written;
	}

	P::s3(delta);
	P::xorBlock(tmp, delta, checksum);

	Buf retrievedTag(P::blockSize);
	if (!m_p->process(true, retrievedTag, tmpBytes)) {
		return {};
	}

	if (!std::equal(tag.begin(), tag.end(), retrievedTag.cbegin())) {
		return {};
	}

	return written;
}

EXPORT size_t CryptOCB2::encrypt(BufRef out, BufRefConst in, const BufRef tag) {
	CHECK

	if (!out.size()) {
		return in.size();
	}

	if (!tag.empty() && tag.size() != P::blockSize) {
		return {};
	}

	KeyBlock delta;
	if (!m_p->process(true, boost::as_writable_bytes(KeyBlockRef(delta)), m_p->m_nonce)) {
		return {};
	}

	size_t written = 0;

	KeyBlock checksum{}, tmp;
	const auto tmpBytes = boost::as_writable_bytes(KeyBlockRef(tmp));

	while (in.size() > P::blockSize) {
		// Counter-cryptanalysis described in section 9 of https://eprint.iacr.org/2019/311
		// For an attack, the second to last block (i.e. the last iteration of this loop)
		// must be all 0 except for the last byte (which may be 0 - 128).
		bool flipABit = false;

		if (in.size() - P::blockSize <= P::blockSize) {
			uint8_t sum = 0;

			for (uint8_t i = 0; i < P::blockSize - 1; ++i) {
				sum |= static_cast< uint8_t >(in[i]);
			}

			if (!sum) {
				// The assumption that critical packets do not turn up by pure chance turned out to be incorrect
				// since digital silence appears to produce them in mass.
				// So instead we now modify the packet in a way which should not affect the audio but will
				// prevent the attack.
				flipABit = true;
			}
		}

		P::s2(delta);
		P::xorBlock(tmp, delta, P::toBlockRef(in));

		if (flipABit) {
			*reinterpret_cast< uint8_t * >(tmp.data()) ^= 1;
		}

		if (!m_p->process(true, tmpBytes, tmpBytes)) {
			return {};
		}

		P::xorBlock(P::toBlockRef(out), delta, tmp);
		written += P::blockSize;

		P::xorBlock(checksum, checksum, P::toBlockRef(in));

		if (flipABit) {
			*reinterpret_cast< uint8_t * >(checksum.data()) ^= 1;
		}

		in  = in.subspan(P::blockSize);
		out = out.subspan(P::blockSize);
	}

	P::s2(delta);

	tmp        = {};
	tmp.back() = Endian::swap(static_cast< SubBlock >(in.size() * 8));
	P::xorBlock(tmp, tmp, delta);

	KeyBlock pad;
	const auto padBytes = boost::as_writable_bytes(KeyBlockRef(pad));

	if (!m_p->process(true, padBytes, tmpBytes)) {
		return {};
	}

	std::copy(in.begin(), in.end(), tmpBytes.begin());
	std::copy_n(padBytes.begin() + in.size(), P::blockSize - in.size(), tmpBytes.begin() + in.size());
	m_p->xorBlock(checksum, checksum, tmp);
	m_p->xorBlock(tmp, pad, tmp);

	std::copy_n(tmpBytes.begin(), in.size(), out.begin());
	written += in.size();

	if (tag.empty()) {
		return written;
	}

	P::s3(delta);
	P::xorBlock(tmp, delta, checksum);

	if (!m_p->process(true, tag, tmpBytes)) {
		return {};
	}

	return written;
}

P::P() : m_ok(false), m_key(), m_nonce(), m_ctx(EVP_CIPHER_CTX_new()) {
	if (!m_ctx) {
		return;
	}

	if (EVP_CipherInit_ex(m_ctx, EVP_aes_128_ecb(), nullptr, nullptr, nullptr, -1) <= 0) {
		return;
	}

	if (EVP_CIPHER_CTX_block_size(m_ctx) != blockSize || EVP_CIPHER_CTX_key_length(m_ctx) != keySize) {
		return;
	}

	m_ok = true;
}

P::~P() {
	if (m_ctx) {
		EVP_CIPHER_CTX_free(m_ctx);
	}
}

size_t P::process(const bool encrypt, const BufRef out, const BufRefConst in) {
	if (static_cast< bool >(EVP_CIPHER_CTX_encrypting(m_ctx)) == encrypt) {
		if (EVP_CipherInit_ex(m_ctx, nullptr, nullptr, nullptr, nullptr, encrypt) <= 0) {
			return {};
		}
	} else {
		// The AES-128-ECB implementation requires resetting the key when switching operation mode.
		// Not doing so doesn't cause any apparent failure, until you realize the output is messed up.
		if (EVP_CipherInit_ex(m_ctx, nullptr, nullptr, CAST_BUF_CONST(m_key.data()), nullptr, encrypt) <= 0) {
			return {};
		}
	}

	if (EVP_CIPHER_CTX_set_padding(m_ctx, 0) <= 0) {
		return {};
	}

	int written1;

	if (EVP_CipherUpdate(m_ctx, CAST_BUF(out.data()), &written1, CAST_BUF_CONST(in.data()), in.size()) <= 0) {
		return {};
	}

	int written2;

	if (EVP_CipherFinal_ex(m_ctx, CAST_BUF(out.data() + written1), &written2) <= 0) {
		return {};
	}

	return written1 + written2;
}

void P::xorBlock(const KeyBlockRef dst, const KeyBlockRefConst a, const KeyBlockRefConst b) {
	for (uint8_t i = 0; i < subBlocks; ++i) {
		dst[i] = a[i] ^ b[i];
	}
}

void P::s2(const KeyBlockRef block) {
	const SubBlock carry = Endian::swap(block[0]) >> shiftBits;

	for (uint8_t i = 0; i < subBlocks - 1; ++i) {
		block[i] = Endian::swap((Endian::swap(block[i]) << 1) | (Endian::swap(block[i + 1]) >> shiftBits));
	}

	block[subBlocks - 1] = Endian::swap((Endian::swap(block[subBlocks - 1]) << 1) ^ (carry * 0x87));
}

void P::s3(const KeyBlockRef block) {
	const SubBlock carry = Endian::swap(block[0]) >> shiftBits;

	for (uint8_t i = 0; i < subBlocks - 1; ++i) {
		block[i] ^= Endian::swap((Endian::swap(block[i]) << 1) | (Endian::swap(block[i + 1]) >> shiftBits));
	}

	block[subBlocks - 1] ^= Endian::swap((Endian::swap(block[subBlocks - 1]) << 1) ^ (carry * 0x87));
}

KeyBlockRef P::toBlockRef(const BufRef buf) {
	return KeyBlockRef(reinterpret_cast< SubBlock * >(buf.data()), subBlocks);
}

KeyBlockRefConst P::toBlockRef(const BufRefConst buf) {
	return KeyBlockRefConst(reinterpret_cast< const SubBlock * >(buf.data()), subBlocks);
}
