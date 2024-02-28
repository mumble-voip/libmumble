// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Data.hpp"
#include "ThreadManager.hpp"

#include "mumble/Base64.hpp"
#include "mumble/Types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <utility>

#include <boost/thread/interruption.hpp>

static constexpr size_t iterations = 1000000;

using namespace mumble;

static std::string decode(const Data::Entry &entry) {
	std::string ret;
	ret.resize(base64::decodedSize(entry.second));

	const BufView out(reinterpret_cast< std::byte * >(ret.data()), ret.size());

	const auto written = base64::decode(out, entry.second);
	if (!written) {
		return {};
	}

	return ret;
}

static std::string encode(const Data::Entry &entry) {
	const BufViewConst in(reinterpret_cast< const std::byte * >(entry.first.data()), entry.first.size());

	std::string ret;
	ret.resize(base64::encodedSize(in));

	const auto written = base64::encode(ret, in);
	if (!written) {
		return {};
	}

	return ret;
}

static uint8_t thread() {
	std::random_device device;
	std::mt19937 algorithm(device());
	std::uniform_int_distribution< size_t > gen(0, std::tuple_size< Data::Table >() - 1);

	for (size_t i = 0; i < iterations; ++i) {
		if (boost::this_thread::interruption_requested()) {
			return 0;
		}

		auto entry = Data::ascii[gen(algorithm)];

		auto decoded = decode(entry);
		if (decoded != entry.first) {
			return 1;
		}

		auto encoded = encode(entry);
		if (encoded != entry.second) {
			return 2;
		}

		entry = Data::unicode[gen(algorithm)];

		decoded = decode(entry);
		if (decoded != entry.first) {
			return 3;
		}

		encoded = encode(entry);
		if (encoded != entry.second) {
			return 4;
		}
	}

	return 0;
}

int32_t main() {
	int32_t ret = 0;

	ThreadManager manager;

	for (uint32_t i = 0; i < manager.physicalNum(); ++i) {
		const ThreadManager::ThreadFunc func = [&ret, &manager]() {
			const auto threadRet = thread();
			if (threadRet != 0) {
				ret = threadRet;
				manager.requestStop();
			}
		};

		manager.add(func);
	}

	manager.wait();

	return ret;
}
