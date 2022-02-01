// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_TEST_THREADMANAGER_HPP
#define MUMBLE_TEST_THREADMANAGER_HPP

#include <cstdint>
#include <functional>
#include <vector>

namespace std {
	class jthread;
	class stop_token;
};

class ThreadManager {
public:
	using ThreadFunc = std::function< void(const std::stop_token) >;

	ThreadManager();
	~ThreadManager();

	void add(const ThreadFunc &func);

	void requestStop();
	void wait();

	static uint32_t physicalNum();

private:
	std::vector< std::jthread> m_threads;
};

#endif
