// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_OPUS_HPP
#define MUMBLE_OPUS_HPP

#include "Macros.hpp"
#include "NonCopyable.hpp"
#include "Types.hpp"

#include <memory>

namespace mumble {
class Opus : NonCopyable {
public:
	using FloatView      = gsl::span< float >;
	using FloatViewConst = gsl::span< const float >;
};

class MUMBLE_EXPORT OpusDecoder : public Opus {
public:
	class P;

	OpusDecoder(OpusDecoder &&decoder);
	OpusDecoder(const uint8_t channels);
	virtual ~OpusDecoder();

	virtual explicit operator bool() const;

	virtual FloatView operator()(const FloatView out, const BufViewConst in, const bool decodeFEC = false);

	virtual Code init(const uint32_t sampleRate = 48000);
	virtual Code reset();

	virtual uint8_t channels() const;
	virtual uint32_t sampleRate() const;

	virtual bool inDTX() const;

	virtual bool usesPhaseInversion() const;
	virtual bool togglePhaseInversion(const bool enable);

	virtual uint32_t packetSamples(const BufViewConst packet);

private:
	std::unique_ptr< P > m_p;
};

class MUMBLE_EXPORT OpusEncoder : public Opus {
public:
	enum class Preset : uint8_t { Unknown, VoIP, Audio, LowDelay };

	class P;

	OpusEncoder(OpusEncoder &&encoder);
	OpusEncoder(const uint8_t channels);
	virtual ~OpusEncoder();

	virtual explicit operator bool() const;

	virtual BufView operator()(const BufView out, const FloatViewConst in);

	virtual Code init(const uint32_t sampleRate = 48000, const Preset preset = Preset::VoIP);
	virtual Code reset();

	virtual uint8_t channels() const;
	virtual uint32_t sampleRate() const;

	virtual Preset preset() const;
	virtual bool setPreset(const Preset preset);

	virtual uint32_t bitrate() const;
	virtual bool setBitrate(const uint32_t bitrate);

	virtual bool inDTX() const;

	virtual bool usesPhaseInversion() const;
	virtual bool togglePhaseInversion(const bool enable);

	virtual bool usesVBR() const;
	virtual bool toggleVBR(const bool enable);

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
