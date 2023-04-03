// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CRYPTOCB2_HPP
#define MUMBLE_CRYPTOCB2_HPP

#include "Crypt.hpp"
#include "Macros.hpp"
#include "Types.hpp"

#include <memory>

namespace mumble {
class MUMBLE_EXPORT CryptOCB2 : public Crypt {
public:
	class P;

	CryptOCB2();
	virtual ~CryptOCB2();

	explicit operator bool() const override;

	void *handle() const override { return nullptr; }

	std::string_view cipher() const override;
	bool setCipher(const std::string_view name) override;

	uint32_t blockSize() const override;
	uint32_t keySize() const override;
	uint32_t nonceSize() const override;

	BufViewConst key() const override;
	Buf genKey() const override;
	bool setKey(const BufViewConst key) override;

	BufViewConst nonce() const override;
	Buf genNonce() const override;
	bool setNonce(const BufViewConst nonce) override;

	size_t decrypt(BufView out, BufViewConst in, const BufViewConst tag = {}, BufViewConst = {}) override;
	size_t encrypt(BufView out, BufViewConst in, const BufView tag = {}, BufViewConst = {}) override;

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
