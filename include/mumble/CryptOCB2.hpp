// Copyright 2022 The Mumble Developers. All rights reserved.
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

	virtual BufRefConst key() const;
	virtual Buf genKey() const;
	virtual bool setKey(const BufRefConst key);

	virtual BufRefConst nonce() const;
	virtual Buf genNonce() const;
	virtual bool setNonce(const BufRefConst nonce);

	virtual size_t decrypt(BufRef out, BufRefConst in, const BufRefConst tag = {});
	virtual size_t encrypt(BufRef out, BufRefConst in, const BufRef tag = {});

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
