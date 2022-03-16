// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble/Endian.hpp"

#ifdef COMPILER_MSVC
#	include <cstdlib>
#endif

using namespace mumble;

Endian::Endian() = default;

uint16_t Endian::swap(const uint16_t value) {
#ifdef COMPILER_MSVC
	return _byteswap_ushort(value);
#else
	return __builtin_bswap16(value);
#endif
}

uint32_t Endian::swap(const uint32_t value) {
#ifdef COMPILER_MSVC
	return _byteswap_ulong(value);
#else
	return __builtin_bswap32(value);
#endif
}

uint64_t Endian::swap(const uint64_t value) {
#ifdef COMPILER_MSVC
	return _byteswap_uint64(value);
#else
	return __builtin_bswap64(value);
#endif
}
