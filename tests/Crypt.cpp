// This file is part of libmumble.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "mumble/Crypt.hpp"
#include "mumble/CryptOCB2.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>


using namespace mumble;


struct TestCrypt : ::testing::TestWithParam< std::tuple< std::string, std::size_t > > {};

TEST_P(TestCrypt, encryption_roundtrip) {
	const std::string cipher     = std::get< 0 >(GetParam());
	const std::size_t bufferSize = std::get< 1 >(GetParam());

	std::unique_ptr< Crypt > crypt;
	if (cipher == "OCB2") {
		crypt = std::make_unique< CryptOCB2 >();
	} else {
		crypt = std::make_unique< Crypt >();
		ASSERT_TRUE(crypt->setCipher(cipher));
	}

	ASSERT_TRUE(crypt->setCipher(crypt->cipher()));
	ASSERT_TRUE(crypt->setKey(crypt->genKey()));
	ASSERT_TRUE(crypt->setNonce(crypt->genNonce()));


	std::random_device device;
	std::mt19937 algo(device());
	std::uniform_int_distribution< std::uint8_t > dataDist(0, std::numeric_limits< std::uint8_t >::max());

	std::vector< std::byte > data(bufferSize);
	std::generate(data.begin(), data.end(), [&]() { return static_cast< std::byte >(dataDist(algo)); });

	// Figure out how big the encrypted data will end up being
	Buf encrypted(crypt->encrypt({}, data));
	Buf decrypted(encrypted.size());

	Buf tag(crypt->blockSize());

	std::size_t encryptedSize = crypt->encrypt(encrypted, data, tag);

	ASSERT_EQ(encryptedSize, encrypted.size());

	std::size_t decryptedSize = crypt->decrypt(decrypted, encrypted, tag);

	ASSERT_EQ(decryptedSize, data.size());

	ASSERT_EQ(decrypted, data);
}

INSTANTIATE_TEST_SUITE_P(Crypt, TestCrypt,
						 ::testing::Combine(::testing::Values("ChaCha20-Poly1305", "AES-256-GCM", "OCB2"),
											::testing::Range(static_cast< std::size_t >(0),
															 static_cast< std::size_t >(1024),
															 static_cast< std::size_t >(128))));
