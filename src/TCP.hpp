// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_TCP_HPP
#define MUMBLE_SRC_TCP_HPP

#include "Socket.hpp"

#include "mumble/Types.hpp"

#include <memory>
#include <utility>

namespace mumble {
class SocketTCP : public Socket {
public:
	using AcceptPtr = std::unique_ptr< SocketTCP >;

	SocketTCP();

	int listen();

	std::pair< int, AcceptPtr > accept(Endpoint &endpoint);
	int connect(const Endpoint &endpoint);

	int getPeerEndpoint(Endpoint &endpoint);

private:
	SocketTCP(const int fd);
};
} // namespace mumble

#endif
