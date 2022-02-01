// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_EXAMPLESERVER_NODE_HPP
#define MUMBLE_EXAMPLESERVER_NODE_HPP

#include "mumble/Cert.hpp"
#include "mumble/Key.hpp"
#include "mumble/Server.hpp"

class UserManager;

class Node {
public:
	Node(const std::shared_ptr< UserManager > &userManager, const std::string_view tcpIP, const uint32_t tcpPort,
		 const std::string_view udpIP, const uint32_t udpPort, const uint32_t bandwidth);
	~Node();

	explicit operator bool() const;

	bool start();

	bool setCert(const std::string_view certPath, const std::string_view keyPath);

private:
	using SessionPtr = std::unique_ptr< mumble::Session >;

	bool m_ok;
	uint32_t m_bandwidth;

	std::shared_ptr< UserManager > m_userManager;

	std::vector< mumble::Cert > m_certChain;
	mumble::Key m_certKey;

	mumble::Server m_server;
};

#endif
