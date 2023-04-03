// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble/PacketDataStream.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <type_traits>


using namespace mumble;


template< typename T > struct TestPacketDataStreamTypes : ::testing::Test {};

using DataTypes = ::testing::Types< std::int8_t, std::uint8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t,
									std::uint64_t, std::int64_t, std::size_t, float, double, bool, std::string >;
TYPED_TEST_SUITE(TestPacketDataStreamTypes, DataTypes);

TYPED_TEST(TestPacketDataStreamTypes, stream_operators) {
	using Type = TypeParam;

	Type originalValue;
	if constexpr (std::is_floating_point_v< Type >) {
		originalValue = static_cast< Type >(213.765643);
	} else if constexpr (std::is_arithmetic_v< Type >) {
		originalValue = static_cast< Type >(42);
	} else if constexpr (std::is_same_v< Type, std::string >) {
		originalValue = "Hello from the other side";
	} else {
		// This assertion always fails, when it ends up in the code. However, we have to
		// make the condition depend on the template parameter in order for the compiler to not
		// error without regarding the if constexpr first.
		static_assert(std::is_arithmetic_v< Type >, "Unexpected kind of type to test PacketDataStream with");
	}

	Buf underlyingBuffer(512);
	PacketDataStream stream(underlyingBuffer);

	ASSERT_TRUE(stream);

	stream << originalValue;

	ASSERT_TRUE(stream);
	ASSERT_GT(stream.seek().size(), 0);

	stream.rewind();

	Type recoveredValue;

	stream >> recoveredValue;

	ASSERT_TRUE(stream);

	ASSERT_EQ(recoveredValue, originalValue);


	if constexpr (std::is_signed_v< Type >) {
		originalValue  = static_cast< Type >(-125);
		recoveredValue = {};

		stream.rewind();

		stream << originalValue;

		stream.rewind();

		stream >> recoveredValue;

		ASSERT_EQ(recoveredValue, originalValue);
	}
}


struct TestIntegerValues : ::testing::TestWithParam< std::int64_t > {};

TEST_P(TestIntegerValues, consistency) {
	std::int64_t originalValue = GetParam();

	Buf buffer(512);
	PacketDataStream stream(buffer);

	stream << originalValue;

	ASSERT_TRUE(stream);
	ASSERT_EQ(stream.undersize(), 0);

	stream.rewind();

	std::int64_t recoveredValue;
	stream >> recoveredValue;

	ASSERT_EQ(recoveredValue, originalValue);
}

INSTANTIATE_TEST_SUITE_P(PacketDataStream, TestIntegerValues,
						 ::testing::Values(std::numeric_limits< std::int64_t >::min(), -5, -4, -3, -2, -1, 0, 1, 2, 3,
										   std::numeric_limits< std::int64_t >::max()));


TEST(PacketDataStream, space) {
	std::array< std::byte, 256 > buffer;
	buffer.fill(std::byte(0x55));

	PacketDataStream out({ buffer.data(), 1 });

	const int8_t valueOut = -2;

	out << valueOut;
	ASSERT_TRUE(out);
	ASSERT_EQ(out.data().size(), 1);
	ASSERT_TRUE(out.seek().empty());
	ASSERT_EQ(buffer[1], std::byte(0x55));

	out << valueOut;
	ASSERT_FALSE(out);
	ASSERT_EQ(out.data().size(), 1);
	ASSERT_TRUE(out.seek().empty());
	ASSERT_EQ(buffer[1], std::byte(0x55));

	PacketDataStream in({ buffer.data(), 1 });

	int8_t valueIn;

	in >> valueIn;
	ASSERT_EQ(valueIn, valueOut);
	ASSERT_TRUE(in);
	ASSERT_EQ(in.data().size(), 1);
	ASSERT_TRUE(in.seek().empty());

	in >> valueIn;
	ASSERT_FALSE(in);
	ASSERT_EQ(in.data().size(), 1);
	ASSERT_TRUE(in.seek().empty());
}

TEST(PacketDataStream, undersize) {
	std::array< std::byte, 256 > buffer;

	std::array< std::byte, 32 > data;
	data.fill(std::byte('Z'));

	for (std::size_t i = 0; i < data.size(); ++i) {
		PacketDataStream out({ buffer.data(), i });
		out << data;

		ASSERT_EQ(data.size() + 1 - i, out.undersize());
		ASSERT_FALSE(out);
		ASSERT_TRUE(out.seek().empty());
	}

	PacketDataStream out({ buffer.data(), data.size() + 1 });
	out << data;

	ASSERT_EQ(out.undersize(), 0);
	ASSERT_TRUE(out);
	ASSERT_TRUE(out.seek().empty());
}
