// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CONNECTION_HPP
#define MUMBLE_CONNECTION_HPP

#include "Cert.hpp"
#include "Key.hpp"
#include "Macros.hpp"
#include "Mumble.hpp"

#include <functional>

namespace mumble {
namespace tcp {
	class Pack;
}

class MUMBLE_EXPORT Connection {
public:
	class P;
	using UniqueP = std::unique_ptr< P >;

	// CR krzmbrzl: "Feedback" seems like a bad name for this. How about "Callbacks" instead?
	struct Feedback {
		std::function< void() > opened;
		std::function< void() > closed;

		std::function< void(Code code) > failed;

		std::function< uint32_t() > timeout;
		std::function< uint32_t() > timeouts;

		std::function< void(tcp::Pack &pack) > pack;
	};

	Connection(Connection &&connection);
	Connection(const int32_t fd, const bool server);
	virtual ~Connection();

	virtual explicit operator bool() const;

	virtual Code operator()(
		const Feedback &feedback, const std::function< bool() > halt = []() { return false; });

	// CR krzmbrzl: What is this supposed to be used for? Returning a unique_ptr reference seems a bit silly. Either you want
	// ownership transfer, in which case you return by value, or you simply return the underlying plain pointer (which is more efficient anyway)
	virtual const UniqueP &p() const;
	// CR krzmbrzl: Unreadable function name: I'm guessing this returns a file-descriptor? In that case let's be more explicit and name
	// the function "fileDescriptor".
	virtual int32_t fd() const;

	virtual Endpoint endpoint() const;
	virtual Endpoint peerEndpoint() const;

	virtual const Cert::Chain &cert() const;
	// CR krzmbrzl: Why return by value?
	virtual Cert::Chain peerCert() const;

	// CR krzmbrzl: I assume the passed cert is being taken ownership of? In that case passing by value is probably more efficient
	// as this allows the type to also be move-constructed.
	virtual bool setCert(const Cert::Chain &cert, const Key &key);

	virtual Code process(
		const bool wait = true, const std::function< bool() > halt = []() { return false; });
	virtual Code write(
		const BufRefConst data, const bool wait = true, const std::function< bool() > halt = []() { return false; });

private:
	Connection(const Connection &)                    = delete;
	virtual Connection &operator=(const Connection &) = delete;

	UniqueP m_p;
};
} // namespace mumble

#endif
