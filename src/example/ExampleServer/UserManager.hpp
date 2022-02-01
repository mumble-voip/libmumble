// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_EXAMPLESERVER_USERMANAGER_HPP
#define MUMBLE_EXAMPLESERVER_USERMANAGER_HPP

#include "Endpoints.hpp"

#include <condition_variable>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

#include <rigtorp/MPMCQueue.h>

namespace std {
class jthread;
}

class User;

class UserManager {
public:
	UserManager(const uint32_t max);
	~UserManager();

	using Buf         = mumble::Buf;
	using BufRef      = mumble::BufRef;
	using BufRefConst = mumble::BufRefConst;
	using UserPtr     = std::shared_ptr< User >;

	UserPtr operator[](const uint32_t id);
	UserPtr operator[](const Endpoint &endpoint);

	bool full();

	uint32_t max();
	uint32_t num();

	std::optional< uint32_t > reserveID();

	void add(const UserPtr &user);
	void del(const uint32_t id);

	UserPtr tryDecrypt(const BufRef out, const BufRefConst in, const Endpoint &endpoint);

private:
	void thread(const std::stop_token stopToken);

	uint32_t m_minID, m_maxID;
	std::unordered_map< uint32_t, UserPtr > m_users;
	std::unordered_map< Endpoint, UserPtr > m_endpoints;
	rigtorp::MPMCQueue< uint32_t > m_usersToDel;

	std::shared_mutex m_mutex;
	std::condition_variable_any m_cond;
	std::unique_ptr< std::jthread > m_thread;
};

#endif
