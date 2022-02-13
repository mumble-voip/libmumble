// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_SRC_PACK_HPP
#define MUMBLE_SRC_PACK_HPP

#include "mumble/Endian.hpp"
#include "mumble/Macros.hpp"
#include "mumble/Message.hpp"
#include "mumble/Types.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace google {
namespace protobuf {
	class Message;
}
} // namespace google

namespace mumble {

class Pack {
public:
	using Type = Message::Type;

	MUMBLE_PACK(struct NetHeader {
		uint16_t type = Endian::toNetwork(static_cast< uint16_t >(Type::Unknown));
		uint32_t size = 0;
	});

	Pack(NetHeader &header);
	Pack(const Message &message);

	Type type() const;

	BufRefConst buf() const;
	BufRefConst data() const;
	BufRef data();

	Message *process() const;

	static bool isPingUDP(const BufRefConst packet);

private:
	void setBuf(const Type type, const Buf &ref);
	bool setBuf(const Type type, const google::protobuf::Message &proto);

	static std::byte toByte(const char byte);
	static void toBuf(Buf &buf, const std::string_view str);

	Buf m_buf;
};
} // namespace mumble

#endif
