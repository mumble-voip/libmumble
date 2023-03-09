// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_EXAMPLESERVER_ENDPOINTS_HPP
#define MUMBLE_EXAMPLESERVER_ENDPOINTS_HPP

#include "mumble/Types.hpp"

#include <unordered_set>

using Endpoint  = mumble::Endpoint;
using Endpoints = std::unordered_set< Endpoint >;

namespace std {
template<> struct hash< Endpoint > {
	size_t operator()(const Endpoint &endpoint) const {
		size_t hash = endpoint.port;

		for (const auto byte : endpoint.ip.v6()) {
			hash += byte;
		}

		return std::hash< size_t >()(hash);
	}
};
} // namespace std

#endif
