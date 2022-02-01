// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_IP_HPP
#define MUMBLE_SRC_IP_HPP

#include "mumble/IP.hpp"

namespace mumble {
class IP::P {
	friend IP;

public:
	P()  = default;
	~P() = default;

private:
	std::array< uint8_t, v6Size > bytes;
};
} // namespace mumble

#endif
