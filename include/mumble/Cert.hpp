// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_CERT_HPP
#define MUMBLE_CERT_HPP

#include "Macros.hpp"
#include "Types.hpp"

#include "mumble/Key.hpp"

#include <map>
#include <memory>

namespace mumble {
	// CR krzmbrzl: Why are all the member functions virtual? The Cert class doesn't appear a likely candidate for inheriting from
class MUMBLE_EXPORT Cert {
public:
	class P;

	using Attributes  = std::map< std::string_view, std::string >;
	using Chain       = std::vector< Cert >;
	using Der         = std::vector< std::byte >;
	// CR krzmbrzl: This should probably not be called a "ref" but rather a "view"
	using DerRef      = boost::span< std::byte >;
	using DerRefConst = boost::span< const std::byte >;

	Cert();
	Cert(const Cert &cert);
	Cert(Cert &&cert);
	// CR krzmbrzl: Maybe consider making below single-arg ctors explicit to avoid weird implicit conversions?
	Cert(void *handle);
	Cert(const DerRefConst der);
	Cert(const std::string_view pem, std::string_view password = {});
	virtual ~Cert();

	virtual explicit operator bool() const;

	virtual Cert &operator=(const Cert &cert);
	virtual Cert &operator=(Cert &&cert);

	// CR krzmbrzl: We should also define operator!=
	virtual bool operator==(const Cert &cert) const;

	virtual void *handle() const;

	virtual Der der() const;
	// CR krzmbrzl: Probably returning a string_view is more performant
	virtual std::string pem() const;

	virtual Key publicKey() const;

	// CR krzmbrzl: We should prefer std::chrono over old C-style tm types
	virtual tm since() const;
	virtual tm until() const;

	virtual bool isAuthority() const;
	virtual bool isIssuer(const Cert &cert) const;
	virtual bool isSelfIssued() const;

	// CR krzmbrzl: These should return a const reference instead. Then the caller can copy, if that is really what they want
	virtual Attributes subjectAttributes() const;
	virtual Attributes issuerAttributes() const;

private:
	std::unique_ptr< P > m_p;
};
} // namespace mumble

#endif
