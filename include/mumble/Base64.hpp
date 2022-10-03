// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_BASE64_HPP
#define MUMBLE_BASE64_HPP

// XCR krzmbrzl: I believe that using non-prefixed include paths will cause issues when this file is included
// by an external project that only has the include path set to the "mumble" parent directory.
//
// Davide: That should never happen, because the path is relative to "include/mumble" (all headers are there).
#include "Macros.hpp"
#include "Types.hpp"

#include <memory>

namespace mumble {
class MUMBLE_EXPORT Base64 {
public:
	class P;

	Base64();
	virtual ~Base64();

	virtual explicit operator bool();

	// XCR krzmbrzl: Adding const to value parameters in a function's declaration is useless
	// Davide: Right.
	virtual size_t decode(const BufRef out, const BufRefConst in);
	static size_t encode(const BufRef out, const BufRefConst in);

private:
	Base64(const Base64 &)                    = delete;
	virtual Base64 &operator=(const Base64 &) = delete;

	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
