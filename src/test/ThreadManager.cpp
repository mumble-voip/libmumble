// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "ThreadManager.hpp"

#include <thread>

ThreadManager::ThreadManager() = default;

ThreadManager::~ThreadManager() {
	requestStop();
}

void ThreadManager::add(const ThreadFunc &func) {
	m_threads.emplace_back(func);
}

void ThreadManager::requestStop() {
	for (auto &thread : m_threads) {
		thread.request_stop();
	}
}

void ThreadManager::wait() {
	for (auto &thread : m_threads) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

uint32_t ThreadManager::physicalNum() {
	const auto num = std::jthread::hardware_concurrency();
	return num ? num : 4;
}
