// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include <mumble/Base64.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstring>
#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

using namespace mumble;


// We need to compare bytes and characters
namespace std {
bool operator==(byte byte, char c) {
	return static_cast< std::underlying_type_t< std::byte > >(byte)
		   == static_cast< std::underlying_type_t< std::byte > >(c);
}
} // namespace std


std::vector< std::byte > bytesFromStr(std::string_view str) {
	std::vector< std::byte > bytes;
	bytes.resize(str.size());

	std::memcpy(bytes.data(), reinterpret_cast< const std::byte * >(str.data()), str.size());

	return bytes;
}


struct Base64Test : ::testing::TestWithParam< std::tuple< std::vector< std::byte >, std::string > > {
	using ParamPack = std::tuple< std::vector< std::byte >, std::string >;
};


TEST_P(Base64Test, encode) {
	const std::vector< std::byte > rawData = std::get< 0 >(GetParam());
	const std::string expectedEncoding     = std::get< 1 >(GetParam());

	Base64 b64;

	std::vector< std::byte > encoded;
	// First call will only return the size
	std::size_t size = b64.encode(encoded, rawData);
	ASSERT_EQ(size, expectedEncoding.size());

	// Resize encoded to the proper size and the perform the actual encoding
	encoded.resize(size);
	size = b64.encode(encoded, rawData);

	ASSERT_THAT(encoded, ::testing::ElementsAreArray(expectedEncoding));
	ASSERT_EQ(size, expectedEncoding.size());
}

TEST_P(Base64Test, decode) {
	const std::vector< std::byte > expectedData = std::get< 0 >(GetParam());
	const std::string encodedData               = std::get< 1 >(GetParam());

	Base64 b64;

	std::vector< std::byte > decoded;
	// First call will only return the size
	std::size_t size =
		b64.decode(decoded, { reinterpret_cast< const std::byte * >(encodedData.data()), encodedData.size() });
	ASSERT_EQ(size, expectedData.size());

	// Resize encoded to the proper size and the perform the actual encoding
	decoded.resize(size);
	size = b64.decode(decoded, { reinterpret_cast< const std::byte * >(encodedData.data()), encodedData.size() });

	ASSERT_THAT(decoded, ::testing::ElementsAreArray(expectedData));
	ASSERT_EQ(size, expectedData.size());
}

// clang-format off
INSTANTIATE_TEST_SUITE_P(Base64, Base64Test, ::testing::Values(
	// ASCII
	Base64Test::ParamPack{ bytesFromStr("L"), "TA==" },
	Base64Test::ParamPack{ bytesFromStr("Lo"), "TG8=" },
	Base64Test::ParamPack{ bytesFromStr("Lor"), "TG9y" },
	Base64Test::ParamPack{ bytesFromStr("Lore"), "TG9yZQ==" },
	Base64Test::ParamPack{ bytesFromStr("Lorem"), "TG9yZW0=" },
	Base64Test::ParamPack{ bytesFromStr("\""), "Ig==" },
	// Non-ASCII symbols
	Base64Test::ParamPack{ bytesFromStr("⑩"), "4pGp" },
	Base64Test::ParamPack{ bytesFromStr("äöü"), "w6TDtsO8" },
	Base64Test::ParamPack{ bytesFromStr("❤"), "4p2k" }
));
// clang-format on



TEST(Base64, consistent_encoding) {
	std::vector< std::byte > encodeSource(32);
	std::vector< std::byte > encodeSink(32);
	std::vector< std::byte > decodeSink(32);

	Base64 b64;

	for (int i = 0; i <= std::numeric_limits< std::uint8_t >::max(); ++i) {
		// Single-byte encoding
		encodeSource[0]         = static_cast< std::byte >(i);
		std::size_t encodedSize = b64.encode(encodeSink, { encodeSource.data(), 1 });
		std::size_t decodedSize = b64.decode(decodeSink, { encodeSink.data(), encodedSize });
		ASSERT_EQ(decodedSize, 1) << "i = " << i;
		ASSERT_EQ(decodeSink[0], encodeSource[0]) << "i = " << i;

		for (int j = 0; i <= std::numeric_limits< std::uint8_t >::max(); ++i) {
			// Two-byte encoding
			encodeSource[1] = static_cast< std::byte >(j);
			encodedSize     = b64.encode(encodeSink, { encodeSource.data(), 2 });
			decodedSize     = b64.decode(decodeSink, { encodeSink.data(), encodedSize });
			ASSERT_EQ(decodedSize, 2) << "i,j = " << i << "," << j;
			ASSERT_EQ(decodeSink[0], encodeSource[0]) << "i,j = " << i << "," << j;
			ASSERT_EQ(decodeSink[1], encodeSource[1]) << "i,j = " << i << "," << j;

			for (int k = 0; i <= std::numeric_limits< std::uint8_t >::max(); ++i) {
				// Three-byte encoding
				encodeSource[2] = static_cast< std::byte >(k);
				encodedSize     = b64.encode(encodeSink, { encodeSource.data(), 3 });
				decodedSize     = b64.decode(decodeSink, { encodeSink.data(), encodedSize });
				ASSERT_EQ(decodedSize, 3) << "i,j,k = " << i << "," << j << "," << k;
				ASSERT_EQ(decodeSink[0], encodeSource[0]) << "i,j,k = " << i << "," << j << "," << k;
				ASSERT_EQ(decodeSink[1], encodeSource[1]) << "i,j,k = " << i << "," << j << "," << k;
				ASSERT_EQ(decodeSink[2], encodeSource[2]) << "i,j,k = " << i << "," << j << "," << k;
			}
		}
	}
}
