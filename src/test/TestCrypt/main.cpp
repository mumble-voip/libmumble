// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "ThreadManager.hpp"

#include "mumble/Crypt.hpp"
#include "mumble/CryptOCB2.hpp"

#include <random>
#include <stop_token>

static constexpr size_t iterations = 100000;

using namespace mumble;

static uint16_t bufSize(std::mt19937 &algorithm) {
	std::uniform_int_distribution< size_t > gen(1, 1024);
	return gen(algorithm);
}

template< typename T > static uint8_t test(T &crypt, const BufRefConst in) {
	if (!crypt.setKey(crypt.genKey())) {
		return 10;
	}

	if (!crypt.setNonce(crypt.genNonce())) {
		return 11;
	}

	Buf out(crypt.encrypt({}, in));
	Buf tag(crypt.blockSize());

	auto written = crypt.encrypt(out, in, tag);
	if (!written) {
		return 12;
	}

	written = crypt.decrypt(out, out, tag);
	if (!written) {
		return 13;
	}

	if (!std::equal(out.cbegin(), out.cend(), in.begin())) {
		return 14;
	}

	return 0;
}

static uint8_t thread(const std::stop_token &stopToken) {
	Crypt cryptChaCha20;
	if (!cryptChaCha20.setCipher("ChaCha20-Poly1305")) {
		return 1;
	}

	Crypt cryptGCM;
	if (!cryptGCM.setCipher("AES-256-GCM")) {
		return 2;
	}

	CryptOCB2 cryptOCB2;

	std::random_device device;
	std::mt19937 algorithm(device());

	std::independent_bits_engine< std::default_random_engine, std::numeric_limits< uint8_t >::digits, uint8_t > engine;

	for (size_t i = 0; i < iterations; ++i) {
		if (stopToken.stop_requested()) {
			return 0;
		}

		std::vector< uint8_t > data(bufSize(algorithm));
		std::generate(begin(data), end(data), engine);

		BufRefConst in(reinterpret_cast< const std::byte * >(data.data()), data.size());

		auto ret = test(cryptChaCha20, in);
		if (ret != 0) {
			return ret;
		}

		ret = test(cryptGCM, in);
		if (ret != 0) {
			return ret;
		}

		ret = test(cryptOCB2, in);
		if (ret != 0) {
			return ret;
		}
	}

	return 0;
}

int32_t main() {
	int32_t ret = 0;

	ThreadManager manager;

	for (uint32_t i = 0; i < manager.physicalNum(); ++i) {
		const ThreadManager::ThreadFunc func = [&ret, &manager](const std::stop_token stopToken) {
			const auto threadRet = thread(stopToken);
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
