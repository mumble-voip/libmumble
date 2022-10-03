// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_OPUS_HPP
#define MUMBLE_OPUS_HPP

#include "Macros.hpp"
#include "Types.hpp"

#include <memory>

// XCR krzmbrzl: Imo either the en- and decoding happens inside the library, in which case there is no need to expose
// encoder and decoder to the user, or its supposed to be handled by the user, in which case this should not be
// implemented in libmumble.
//
// Davide: As discussed, we're going to move these classes into an audio-specific library.
namespace mumble {
class MUMBLE_EXPORT OpusDecoder {
public:
	class P;

	OpusDecoder(OpusDecoder &&decoder);
	OpusDecoder(const uint8_t channels);
	virtual ~OpusDecoder();

	virtual explicit operator bool() const;

	virtual size_t operator()(const BufRef out, const BufRefConst in, const bool decodeFEC = false);

	virtual Code init(const uint32_t sampleRate = 48000);
	virtual Code reset();

	virtual uint8_t channels() const;
	virtual uint32_t sampleRate() const;

	virtual bool inDTX() const;

	virtual bool phaseInversion() const;
	virtual bool togglePhaseInversion(const bool enable);

	virtual uint32_t packetSamples(const BufRefConst packet);

private:
	std::unique_ptr< P > m_p;
};

// XCR krzmbrzl: This should be in its own header file
// Davide: Okay, let's do that when we move the classes.
class MUMBLE_EXPORT OpusEncoder {
public:
	enum class Preset : uint8_t { Unknown, VoIP, Audio, LowDelay };

	class P;

	OpusEncoder(OpusEncoder &&encoder);
	OpusEncoder(const uint8_t channels);
	virtual ~OpusEncoder();

	virtual explicit operator bool() const;

	virtual size_t operator()(const BufRef out, const BufRefConst in);

	virtual Code init(const uint32_t sampleRate = 48000, const Preset preset = Preset::VoIP);
	virtual Code reset();

	virtual uint8_t channels() const;
	virtual uint32_t sampleRate() const;

	virtual Preset preset() const;
	virtual bool setPreset(const Preset preset);

	virtual uint32_t bitrate() const;
	virtual bool setBitrate(const uint32_t bitrate);

	virtual bool inDTX() const;

	virtual bool phaseInversion() const;
	virtual bool togglePhaseInversion(const bool enable);

	virtual bool vbr() const;
	virtual bool toggleVBR(const bool enable);

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
