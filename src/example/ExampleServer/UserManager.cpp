// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "UserManager.hpp"

#include "Endpoints.hpp"
#include "User.hpp"

#include <mutex>
#include <unordered_set>
#include <utility>

#include <boost/thread/lock_types.hpp>
#include <boost/thread/thread_only.hpp>

#include <rigtorp/MPMCQueue.h>

using namespace mumble;

using UserPtr = UserManager::UserPtr;

UserManager::UserManager(const uint32_t max)
	: m_minID(1), m_maxID(max), m_usersToDel(32), m_thread(new boost::thread(&UserManager::thread, this)) {
}

UserManager::~UserManager() {
	m_thread->interrupt();
}

UserPtr UserManager::operator[](const uint32_t id) {
	boost::shared_lock lock(m_mutex);

	const auto iter = m_users.find(id);
	if (iter != m_users.cend()) {
		return iter->second;
	}

	return {};
}

UserPtr UserManager::operator[](const Endpoint &endpoint) {
	boost::shared_lock lock(m_mutex);

	const auto iter = m_endpoints.find(endpoint);
	if (iter != m_endpoints.cend()) {
		return iter->second;
	}

	return {};
}

bool UserManager::full() {
	return m_users.size() >= max();
}

uint32_t UserManager::max() {
	return m_maxID - m_minID + 1;
}

uint32_t UserManager::num() {
	boost::shared_lock lock(m_mutex);

	return m_users.size();
}

std::optional< uint32_t > UserManager::reserveID() {
	boost::unique_lock lock(m_mutex);

	if (m_users.size() >= max()) {
		return {};
	}

	for (uint32_t id = m_minID; id < m_maxID; ++id) {
		if (m_users.find(id) == m_users.cend()) {
			m_users.emplace(id, nullptr);

			return id;
		}
	}

	return {};
}

void UserManager::add(const UserPtr &user) {
	boost::unique_lock lock(m_mutex);

	m_users[user->id()] = std::shared_ptr< User >(user);
}

void UserManager::del(const uint32_t id) {
	m_usersToDel.push(id);
	m_cond.notify_all();
}

UserPtr UserManager::tryDecrypt(const BufRef out, const BufRefConst in, const Endpoint &endpoint) {
	UserPtr user;

	{
		boost::shared_lock lock(m_mutex);

		for (auto &iter : m_users) {
			if (iter.second->decrypt(out, in)) {
				user = iter.second;
			}
		}
	}

	if (!user) {
		return {};
	}

	{
		std::unique_lock lock(m_mutex);

		m_endpoints.insert_or_assign(endpoint, user);
	}

	return user;
}

void UserManager::thread() {
	while (!m_thread->interruption_requested()) {
		boost::unique_lock< boost::shared_mutex > lock(m_mutex);

		m_cond.wait(lock);

		uint32_t id;
		while (m_usersToDel.try_pop(id)) {
			const auto user = m_users.extract(id).mapped();
			for (const auto &endpoint : user->endpoints()) {
				m_endpoints.erase(endpoint);
			}
		}
	}
}
