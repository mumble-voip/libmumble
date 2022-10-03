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

	// XCR krzmbrzl: "Feedback" seems like a bad name for this. How about "Callbacks" instead?
	// Davide: I actually think the current name is great; it indicates that events are sent to the API user.
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

	// XCR krzmbrzl: What is this supposed to be used for? Returning a unique_ptr reference seems a bit silly. Either you want
	// ownership transfer, in which case you return by value, or you simply return the underlying plain pointer (which is more efficient anyway)
	//
	// Davide: This is actually used internally to access the stuff in P ("m_p" is private).
	// It's kind of a hack, that will be definitely removed while getting rid of PImpl (as discussed).
	virtual const UniqueP &p() const;
	virtual int32_t fd() const;

	virtual Endpoint endpoint() const;
	virtual Endpoint peerEndpoint() const;

	virtual const Cert::Chain &cert() const;
	// XCR krzmbrzl: Why return by value?
	// Davide: We don't store the certificate chain ourselves: it's always retrieved through the OpenSSL context.
	virtual Cert::Chain peerCert() const;

	// XCR krzmbrzl: I assume the passed cert is being taken ownership of? In that case passing by value is probably more efficient
	// as this allows the type to also be move-constructed.
	//
	// Davide: It's copied.
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
