// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_MUMBLE_HPP
#define MUMBLE_MUMBLE_HPP

#include "Macros.hpp"
#include "Types.hpp"

namespace mumble {
	// CR krzmbrzl: It seems this class is merely used as a namespace. How about turning this into an actual namespace?
class MUMBLE_EXPORT Mumble {
public:
	Mumble();
	virtual ~Mumble() = delete;

	static Version version();

	static Code init();
	static Code deinit();
	static size_t initCount();

private:
	Mumble(const Mumble &)                    = delete;
	virtual Mumble &operator=(const Mumble &) = delete;
};
} // namespace mumble

#endif
