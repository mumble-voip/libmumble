// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble/Opus.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <functional>
#include <numeric>


using namespace mumble;


struct TestOpus : ::testing::TestWithParam< std::tuple< int, int, int, Opus::Encoder::Preset, bool, bool > > {};

TEST_P(TestOpus, encode_decode_roundtrip) {
	const int frameSize                      = std::get< 0 >(GetParam());
	const int encoderChannelCount            = std::get< 1 >(GetParam());
	const int decoderChannelCount            = std::get< 2 >(GetParam());
	const Opus::Encoder::Preset encodePreset = std::get< 3 >(GetParam());
	const bool encoderPhaseInversion         = std::get< 4 >(GetParam());
	const bool decoderPhaseInversion         = std::get< 5 >(GetParam());

	constexpr std::size_t sampleRate = 48000;

	Opus::Encoder encoder(encoderChannelCount);
	Opus::Decoder decoder(decoderChannelCount);

	encoder.togglePhaseInversion(encoderPhaseInversion);
	decoder.togglePhaseInversion(decoderPhaseInversion);

	ASSERT_EQ(encoder.init(sampleRate, encodePreset), Code::Success);
	ASSERT_EQ(decoder.init(sampleRate), Code::Success);

	std::vector< float > inputData(frameSize * encoderChannelCount);
	for (std::size_t i = 0; i < inputData.size(); ++i) {
		// Generate "random" float values between zero and one
		inputData[i] = i / (0.34176f * i + i);
	}

	Buf encodedBuffer(sizeof(Opus::IntegerView::value_type) * frameSize * encoderChannelCount);
	std::vector< Opus::FloatView::value_type > floatDecodedBuffer(frameSize * decoderChannelCount);
	std::vector< Opus::IntegerView::value_type > intDecodedBuffer(frameSize * decoderChannelCount);

	BufView encodedData = encoder(encodedBuffer, inputData);
	ASSERT_GT(encodedData.size(), 0);

	Opus::FloatView decodedFloatData = decoder(floatDecodedBuffer, encodedData);
	ASSERT_EQ(decodedFloatData.size(), frameSize * decoderChannelCount);

	Opus::IntegerView decodedIntData = decoder(intDecodedBuffer, encodedData);
	ASSERT_EQ(decodedIntData.size(), frameSize * decoderChannelCount);

	// TODO: Do we also need to test encoding through an IntegerView?
}

INSTANTIATE_TEST_SUITE_P(Opus, TestOpus,
						 ::testing::Combine(::testing::Values(120, 240, 480, 960, 1920, 2880), ::testing::Values(1, 2),
											::testing::Values(1, 2),
											::testing::Values(Opus::Encoder::Preset::VoIP, Opus::Encoder::Preset::Audio,
															  Opus::Encoder::Preset::LowDelay),
											::testing::Bool(), ::testing::Bool()));
