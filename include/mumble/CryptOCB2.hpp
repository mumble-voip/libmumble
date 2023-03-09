// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CRYPTOCB2_HPP
#define MUMBLE_CRYPTOCB2_HPP

#include "Macros.hpp"
#include "NonCopyable.hpp"
#include "Types.hpp"

#include <memory>

namespace mumble {
class MUMBLE_EXPORT CryptOCB2 : NonCopyable {
public:
	class P;

	CryptOCB2();
	virtual ~CryptOCB2();

	virtual explicit operator bool() const;

	virtual uint32_t blockSize() const;
	virtual uint32_t keySize() const;
	virtual uint32_t nonceSize() const;

	virtual BufViewConst key() const;
	virtual Buf genKey() const;
	virtual bool setKey(const BufViewConst key);

	virtual BufViewConst nonce() const;
	virtual Buf genNonce() const;
	virtual bool setNonce(const BufViewConst nonce);

	virtual size_t decrypt(BufView out, BufViewConst in, const BufViewConst tag = {});
	virtual size_t encrypt(BufView out, BufViewConst in, const BufView tag = {});

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
