// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

// Inspired by https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp.

#ifndef MUMBLE_BASE64_HPP
#define MUMBLE_BASE64_HPP

#include "Types.hpp"

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

#include <gsl/span>

namespace mumble {
namespace base64 {
	using StrView      = gsl::span< char >;
	using StrViewConst = gsl::span< const char >;

	static constexpr char PAD_CHAR = '=';

	static constexpr std::byte operator+(const std::byte lhs, const std::byte rhs) {
		const auto ret = std::to_integer< uint8_t >(lhs) + std::to_integer< uint8_t >(rhs);
		return std::byte(ret);
	}

	static constexpr std::byte charToByte(const char ch) {
		if (ch >= 'A' && ch <= 'Z')
			return std::byte(ch - 'A');
		if (ch >= 'a' && ch <= 'z')
			return std::byte(ch - 'a' + ('Z' - 'A') + 1);
		if (ch >= '0' && ch <= '9')
			return std::byte(ch - '0' + ('Z' - 'A') + ('z' - 'a') + 2);
		if (ch == '+' || ch == '-')
			return std::byte(62);
		if (ch == '/' || ch == '_')
			return std::byte(63);

		throw std::range_error(std::string() + ch + " is not a valid character!");
	}

	static constexpr char byteToChar(const std::byte byte) {
		const auto num = std::to_integer< char >(byte);

		if (num <= 25)
			return num + 'A';
		if (num >= 26 && num <= 51)
			return num + 'a' - ('Z' - 'A') - 1;
		if (num >= 52 && num <= 61)
			return num + '0' - ('Z' - 'A') - ('z' - 'a') - 2;
		if (num == 62)
			return '+';
		if (num == 63)
			return '/';

		throw std::range_error(std::to_string(+num)
							   + " is not a valid byte that can be translated into Base64 alphabet!");
	}

	static inline bool isValid(const char ch) {
		try {
			charToByte(ch);
		} catch (const std::range_error &) {
			return false;
		}

		return true;
	}

	static inline bool isValid(const StrViewConst in) {
		for (const char ch : in) {
			if (!isValid(ch)) {
				return false;
			}
		}

		return true;
	}

	static constexpr size_t decodedSize(const StrViewConst in) {
		size_t size = (in.size() / 4) * 3;
		// Base64 can have up to 2 pad characters, in order to make the string a multiple of 4.
		// In order to calculate the exact output size we have to ignore padding.
		if (auto iter = in.rbegin(); *iter == PAD_CHAR) {
			size -= *++iter == PAD_CHAR ? 2U : 1U;
		}

		return size;
	}

	static constexpr size_t encodedSize(const BufViewConst in) {
		// ceil((float) a / 3) == (a + 2) / 3, assuming a is an integer.
		return ((in.size() + 2) / 3) * 4;
	}

	static constexpr size_t decode(BufView out, StrViewConst in) {
		if (in.empty()) {
			return 0;
		}

		if (in.size() % 4 != 0) {
			throw std::invalid_argument("Input is not a multiple of 4!");
		}

		const size_t size = decodedSize(in);
		if (out.size() < size) {
			throw std::invalid_argument("Insufficient output buffer size!");
		}

		for (; !in.empty(); out = out.subspan(3), in = in.subspan(4)) {
			out[0] = (charToByte(in[0]) << 2) + ((charToByte(in[1]) & std::byte(0x30)) >> 4);

			if (in[2] == '=') {
				break;
			}

			out[1] = ((charToByte(in[1]) & std::byte(0x0f)) << 4) + ((charToByte(in[2]) & std::byte(0x3c)) >> 2);

			if (in[3] == '=') {
				break;
			}

			out[2] = ((charToByte(in[2]) & std::byte(0x03)) << 6) + charToByte(in[3]);
		}

		return size;
	}

	static constexpr size_t encode(StrView out, BufViewConst in) {
		if (in.empty()) {
			return 0;
		}

		const size_t size = encodedSize(in);
		if (out.size() < size) {
			throw std::invalid_argument("Insufficient output buffer size!");
		}

		for (; !in.empty(); out = out.subspan(4)) {
			out[0] = byteToChar((in[0] & std::byte(0xfc)) >> 2);

			if (in.size() >= 2) {
				out[1] = byteToChar(((in[0] & std::byte(0x03)) << 4) + ((in[1] & std::byte(0xf0)) >> 4));

				if (in.size() >= 3) {
					out[2] = byteToChar(((in[1] & std::byte(0x0f)) << 2) + ((in[2] & std::byte(0xc0)) >> 6));
					out[3] = byteToChar(in[2] & std::byte(0x3f));
				} else {
					out[2] = byteToChar((in[1] & std::byte(0x0f)) << 2);
					out[3] = PAD_CHAR;
				}
			} else {
				out[1] = byteToChar((in[0] & std::byte(0x03)) << 4);
				out[2] = PAD_CHAR;
				out[3] = PAD_CHAR;
			}

			// All data blocks are guaranteed to be 3 bytes except the last one.
			in = in.subspan(std::min(size_t{ 3 }, in.size()));
		}

		return size;
	}
}; // namespace base64
} // namespace mumble

#endif
