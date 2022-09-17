// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_PACK_HPP
#define MUMBLE_PACK_HPP

#include "Endian.hpp"
#include "Macros.hpp"
#include "Message.hpp"
#include "Types.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

namespace google {
namespace protobuf {
	class Message;
}
} // namespace google

namespace mumble {
template< typename NetHeader > class Pack {
public:
	Pack(const Pack &pack) = default;
	Pack(Pack &&pack)      = default;
	Pack(const uint32_t dataSize = 0) : m_buf(sizeof(NetHeader) + dataSize) {}
	virtual ~Pack() = default;

	virtual Pack &operator=(const Pack &pack) = default;
	virtual Pack &operator=(Pack &&pack)      = default;

	virtual bool operator==(const Pack &pack) const { return pack.m_buf == m_buf; }

	virtual BufRefConst buf() const { return m_buf; }

	virtual BufRef buf() { return m_buf; }

	virtual BufRefConst data() const { return { m_buf.data() + sizeof(NetHeader), m_buf.size() - sizeof(NetHeader) }; }

	virtual BufRef data() { return { m_buf.data() + sizeof(NetHeader), m_buf.size() - sizeof(NetHeader) }; }

	virtual const NetHeader &header() const { return *reinterpret_cast< const NetHeader * >(m_buf.data()); }

	virtual NetHeader &header() { return *reinterpret_cast< NetHeader * >(m_buf.data()); }

protected:
	Buf m_buf;
};

namespace tcp {
	MUMBLE_PACK(struct NetHeader {
		uint16_t type = Endian::toNetwork(static_cast< uint16_t >(Message::Type::Unknown));
		uint32_t size = 0;
	});

	class MUMBLE_EXPORT Pack : public mumble::Pack< NetHeader > {
	public:
		using Type = Message::Type;

		Pack(const Message &message);
		Pack(const NetHeader &header = {});
		Pack(const google::protobuf::Message &proto);
		virtual ~Pack();

		virtual bool operator()(Message &message, uint32_t dataSize = std::numeric_limits< uint32_t >::max()) const;

		virtual Type type() const { return static_cast< Type >(Endian::toHost(header().type)); }
	};
} // namespace tcp

namespace udp {
	MUMBLE_PACK(struct NetHeader { uint8_t type = static_cast< uint8_t >(Message::Type::Unknown); });

	class MUMBLE_EXPORT Pack : public mumble::Pack< NetHeader > {
	public:
		using Type = Message::Type;

		Pack(const Message &message);
		Pack(const uint32_t dataSize = 0, const NetHeader &header = {});
		Pack(const google::protobuf::Message &proto);
		virtual ~Pack();

		virtual bool operator()(Message &message, uint32_t dataSize = std::numeric_limits< uint32_t >::max()) const;

		virtual Type type() const { return static_cast< Type >(header().type); }
	};
} // namespace udp
} // namespace mumble

#endif
