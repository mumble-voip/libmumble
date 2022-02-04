// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "ThreadManager.hpp"

#include "mumble/Opus.hpp"

#include <stop_token>

static constexpr size_t iterations = 1000;

static constexpr uint32_t sampleRate = 48000;

static constexpr std::array< uint16_t, 6 > bufferSamples = { 120, 240, 480, 960, 1920, 2880 };

using namespace mumble;

static uint8_t test(OpusDecoder &decoder, OpusEncoder &encoder, const BufRefConst in, const BufRef out) {
	if (!encoder(out, in)) {
		return 10;
	}

	if (!decoder(out, out)) {
		return 11;
	}

	return 0;
}

template< typename T > bool initOpus(T &opus) {
	const auto code = opus.init(sampleRate);
	if (code != Code::Success) {
		printf("Failed to init Opus with error \"%s\"!\n", text(code).data());
		return false;
	}

	return true;
}

static uint8_t thread(const std::stop_token &stopToken) {
	OpusDecoder monoDecoder(1);
	if (!initOpus(monoDecoder)) {
		return 1;
	}

	OpusEncoder monoEncoder(1);
	if (!initOpus(monoEncoder)) {
		return 2;
	}

	OpusDecoder stereoDecoder(2);
	if (!initOpus(stereoDecoder)) {
		return 3;
	}

	OpusEncoder stereoEncoder(2);
	if (!initOpus(stereoEncoder)) {
		return 4;
	}

	for (const auto samples : bufferSamples) {
		std::vector< float > data(samples * 2);
		std::vector< float > buf(data.size());

		BufRefConst in(reinterpret_cast< const std::byte * >(data.data()), data.size() * sizeof(float));
		BufRef out(reinterpret_cast< std::byte * >(buf.data()), buf.size() * sizeof(float));

		for (size_t i = 0; i < iterations; ++i) {
			if (stopToken.stop_requested()) {
				return 0;
			}

			test(monoDecoder, monoEncoder, in.first(in.size() / 2), out.first(out.size() / 2));
			test(stereoDecoder, stereoEncoder, in, out);
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
