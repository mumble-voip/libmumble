// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble/Hash.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace mumble;

constexpr std::size_t nTestCases                     = 2;
const std::array< std::string, nTestCases > testData = { "",
														 "Lorem ipsum dolor sit amet, consectetur adipiscing elit." };

const std::map< std::string, std::array< std::string, nTestCases > > expectedHash = {
	{ "SHA512",
	  {
		  "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd4741"
		  "7a81a538327af927da3e",
		  "19d8350a48bb40d04b4045955a9d95599aa5bd5b8c74c36c098b58c3cd8af142b8d9cf0417ef6dc88c4ed91c69ea8e2adce7afec1afb"
		  "6a21d8cae681b0902997",
	  } },
	{ "SHA3-512",
	  {
		  "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6"
		  "d3e301758586281dcd26",
		  "efb580d4e7145bd4ddd10153624b747d21d09b79c0a2c708de09c19dcd6901a0c34d78ad0e7b7c64f46b7f3ab85aeca7d0f718e9cf38"
		  "089ad1c9cc05c45ae7ee",
	  } },
	{ "BLAKE2b512",
	  {
		  "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448"
		  "b755d56f701afe9be2ce",
		  "d6a1c4f1083b7535f7314305ac77fc6525b13333554e6a43bc75ef21f7fffc263b23fe1b713aac41151c50a4793c02cd7fcf66adbbac"
		  "ef4c547e8271c3d35111",
	  } },
};

static std::string toHex(const BufViewConst bytes) {
	std::ostringstream stream;

	stream << std::hex << std::setfill('0');

	for (const auto byte : bytes) {
		stream << std::setw(2) << static_cast< int >(byte);
	}

	return stream.str();
}

struct HashTest : ::testing::TestWithParam< std::tuple< std::string, std::size_t > > {};

TEST_P(HashTest, hash) {
	const std::string hashAlgorithm = std::get< 0 >(GetParam());
	const std::size_t testCaseIndex = std::get< 1 >(GetParam());

	ASSERT_TRUE(expectedHash.find(hashAlgorithm) != expectedHash.end())
		<< "Invalid hash algorithm '" << hashAlgorithm << "'";
	ASSERT_LE(testCaseIndex, expectedHash.at(hashAlgorithm).size()) << "No such test case index: " << testCaseIndex;

	Hash hash;
	hash.setType(hashAlgorithm);

	BufViewConst inputData{ reinterpret_cast< const std::byte * >(testData[testCaseIndex].data()),
							testData[testCaseIndex].size() };

	Buf digest(hash(BufView{}, inputData));

	std::size_t hashedSize = hash(digest, inputData);

	ASSERT_EQ(toHex(digest), expectedHash.at(hashAlgorithm)[testCaseIndex])
		<< "Hashed: '" << testData[testCaseIndex] << "'";
	ASSERT_EQ(hashedSize, digest.size());
}

INSTANTIATE_TEST_SUITE_P(Hash, HashTest,
						 ::testing::Combine(::testing::Values("SHA512", "SHA3-512", "BLAKE2b512"),
											::testing::Range(static_cast< std::size_t >(0), nTestCases)));
