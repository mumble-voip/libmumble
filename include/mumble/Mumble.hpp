// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_MUMBLE_HPP
#define MUMBLE_MUMBLE_HPP

#include "Macros.hpp"
#include "Types.hpp"

#include <cstdint>

namespace mumble {
class EXPORT Mumble {
public:
	enum class TypeUDP : uint8_t { VoiceCELTAlpha, Ping, VoiceSpeex, VoiceCELTBeta, VoiceOpus };

	struct Version {
		uint16_t major;
		uint8_t minor;
		uint8_t patch;

		uint32_t blob() const { return major << 16 | minor << 8 | patch; }

		Version(const uint32_t blob) : major(blob >> 16), minor(blob >> 8), patch(blob) {}
		Version(const uint16_t major, const uint16_t minor, const uint8_t patch)
			: major(major), minor(minor), patch(patch) {}
	};

	MUMBLE_PACK(struct PingUDP {
		uint32_t versionBlob;
		uint64_t timestamp;
		uint32_t sessions;
		uint32_t maxSessions;
		uint32_t maxBandwidth;
	});

	Mumble();
	virtual ~Mumble() = delete;

	static Version version();

	static Code init();
	static Code deinit();

	static TypeUDP packetType(const BufRefConst data) {
		const auto byte = (static_cast< uint8_t >(data[0]) >> 5) & 0x7;
		return static_cast< TypeUDP >(byte);
	}

private:
	Mumble(const Mumble &)  = delete;
	virtual Mumble &operator=(const Mumble &) = delete;
};
} // namespace mumble

#endif
