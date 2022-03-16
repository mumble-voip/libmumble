// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CRYPT_HPP
#define MUMBLE_CRYPT_HPP

#include "Macros.hpp"
#include "Types.hpp"

#include <memory>

namespace mumble {
class MUMBLE_EXPORT Crypt {
public:
	class P;

	Crypt(Crypt &&crypt);
	Crypt();
	virtual ~Crypt();

	virtual explicit operator bool() const;

	virtual Crypt &operator=(Crypt &&crypt);

	virtual void *handle() const;

	virtual std::string_view cipher() const;
	virtual bool setCipher(const std::string_view name);

	virtual uint32_t blockSize() const;
	virtual uint32_t keySize() const;
	virtual uint32_t nonceSize() const;

	virtual BufRefConst key() const;
	virtual Buf genKey() const;
	virtual bool setKey(const BufRefConst key);

	virtual BufRefConst nonce() const;
	virtual Buf genNonce() const;
	virtual bool setNonce(const BufRefConst nonce);

	virtual bool padding() const;
	virtual bool togglePadding(const bool enable);

	virtual bool reset();

	virtual size_t decrypt(const BufRef out, const BufRefConst in, const BufRefConst tag = {},
						   const BufRefConst aad = {});
	virtual size_t encrypt(const BufRef out, const BufRefConst in, const BufRef tag = {}, const BufRefConst aad = {});

private:
	Crypt(const Crypt &)   = delete;
	virtual Crypt &operator=(const Crypt &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
