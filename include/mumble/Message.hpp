// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#ifndef MUMBLE_MESSAGE_HPP
#define MUMBLE_MESSAGE_HPP

#include "Cert.hpp"
#include "IP.hpp"
#include "Macros.hpp"
#include "Mumble.hpp"

#include <chrono>
#include <optional>

#define MUMBLE_MESSAGE_CDTOR(name)  \
	name() : Message(Type::name) {} \
	virtual ~name() = default;

namespace mumble {
class Version;

class EXPORT Message {
public:
	using Clock     = std::chrono::high_resolution_clock;
	using Timestamp = std::chrono::time_point< Clock >;

	struct Version;
	struct UDPTunnel;
	struct Authenticate;
	struct Ping;
	struct Reject;
	struct ServerSync;
	struct ChannelRemove;
	struct ChannelState;
	struct UserRemove;
	struct UserState;
	struct BanList;
	struct TextMessage;
	struct PermissionDenied;
	struct ACL;
	struct QueryUsers;
	struct CryptSetup;
	struct ContextActionModify;
	struct ContextAction;
	struct UserList;
	struct VoiceTarget;
	struct PermissionQuery;
	struct CodecVersion;
	struct UserStats;
	struct RequestBlob;
	struct ServerConfig;
	struct SuggestConfig;
	struct PluginDataTransmission;

	enum class Type : uint16_t {
		Version,
		UDPTunnel,
		Authenticate,
		Ping,
		Reject,
		ServerSync,
		ChannelRemove,
		ChannelState,
		UserRemove,
		UserState,
		BanList,
		TextMessage,
		PermissionDenied,
		ACL,
		QueryUsers,
		CryptSetup,
		ContextActionModify,
		ContextAction,
		UserList,
		VoiceTarget,
		PermissionQuery,
		CodecVersion,
		UserStats,
		RequestBlob,
		ServerConfig,
		SuggestConfig,
		PluginDataTransmission,
		Unknown = UINT16_MAX
	};

	enum class Perm : uint32_t {
		None            = 0b00000000'00000000'00000000'00000000,
		Write           = 0b00000000'00000000'00000000'00000001,
		Traverse        = 0b00000000'00000000'00000000'00000010,
		Enter           = 0b00000000'00000000'00000000'00000100,
		Speak           = 0b00000000'00000000'00000000'00001000,
		MuteDeafen      = 0b00000000'00000000'00000000'00010000,
		Move            = 0b00000000'00000000'00000000'00100000,
		MakeChannel     = 0b00000000'00000000'00000000'01000000,
		LinkChannel     = 0b00000000'00000000'00000000'10000000,
		Whisper         = 0b00000000'00000000'00000001'00000000,
		TextMessage     = 0b00000000'00000000'00000010'00000000,
		MakeTempChannel = 0b00000000'00000000'00000100'00000000,
		Listen          = 0b00000000'00000000'00001000'00000000,

		// Root channel only
		Kick             = 0b00000000'00000001'00000000'00000000,
		Ban              = 0b00000000'00000010'00000000'00000000,
		Register         = 0b00000000'00000100'00000000'00000000,
		SelfRegister     = 0b00000000'00001000'00000000'00000000,
		ResetUserContent = 0b00000000'00010000'00000000'00000000,

		All = Write + Traverse + Enter + Speak + MuteDeafen + Move + MakeChannel + LinkChannel + Whisper + TextMessage
			  + MakeTempChannel + Listen + Kick + Ban + Register + SelfRegister + ResetUserContent
	};

	Message(const Type type = Type::Unknown) : m_type(type), m_timestamp(Clock::now()) {}
	virtual ~Message() = default;

	virtual Type type() const { return m_type; }
	virtual Timestamp timestamp() const { return m_timestamp; }

	static constexpr std::string_view text(const Type type) {
		switch (type) {
			case Type::Version: {
				return "Version";
			}
			case Type::UDPTunnel: {
				return "UDPTunnel";
			}
			case Type::Authenticate: {
				return "Authenticate";
			}
			case Type::Ping: {
				return "Ping";
			}
			case Type::Reject: {
				return "Reject";
			}
			case Type::ServerSync: {
				return "ServerSync";
			}
			case Type::ChannelRemove: {
				return "ChannelRemove";
			}
			case Type::ChannelState: {
				return "ChannelState";
			}
			case Type::UserRemove: {
				return "UserRemove";
			}
			case Type::UserState: {
				return "UserState";
			}
			case Type::BanList: {
				return "BanList";
			}
			case Type::TextMessage: {
				return "TextMessage";
			}
			case Type::PermissionDenied: {
				return "PermissionDenied";
			}
			case Type::ACL: {
				return "ACL";
			}
			case Type::QueryUsers: {
				return "QueryUsers";
			}
			case Type::CryptSetup: {
				return "CryptSetup";
			}
			case Type::ContextActionModify: {
				return "ContextActionModify";
			}
			case Type::ContextAction: {
				return "ContextAction";
			}
			case Type::UserList: {
				return "UserList";
			}
			case Type::VoiceTarget: {
				return "VoiceTarget";
			}
			case Type::PermissionQuery: {
				return "PermissionQuery";
			}
			case Type::CodecVersion: {
				return "CodecVersion";
			}
			case Type::UserStats: {
				return "UserStats";
			}
			case Type::RequestBlob: {
				return "RequestBlob";
			}
			case Type::ServerConfig: {
				return "ServerConfig";
			}
			case Type::SuggestConfig: {
				return "SuggestConfig";
			}
			case Type::PluginDataTransmission: {
				return "PluginDataTransmission";
			}
			case Type::Unknown: {
				break;
			}
		}

		return "Unknown";
	}

private:
	Message(const Message &) = delete;
	virtual Message &operator=(const Message &) = delete;

	Type m_type;
	Timestamp m_timestamp;
};

struct Message::Version : public Message {
	uint32_t version      = 0;
	std::string release   = {};
	std::string os        = {};
	std::string osVersion = {};

	MUMBLE_MESSAGE_CDTOR(Version)
};

struct Message::UDPTunnel : public Message {
	std::vector< std::byte > packet = {};

	MUMBLE_MESSAGE_CDTOR(UDPTunnel)
};

struct Message::Authenticate : public Message {
	std::string username                = {};
	std::string password                = {};
	std::vector< std::string > tokens   = {};
	std::vector< int32_t > celtVersions = {};
	bool opus                           = false;

	MUMBLE_MESSAGE_CDTOR(Authenticate)
};

struct Message::Ping : public Message {
	uint64_t timestamp  = 0;
	uint32_t good       = 0;
	uint32_t late       = 0;
	uint32_t lost       = 0;
	uint32_t resync     = 0;
	uint32_t udpPackets = 0;
	uint32_t tcpPackets = 0;
	float udpPingAvg    = 0.f;
	float udpPingVar    = 0.f;
	float tcpPingAvg    = 0.f;
	float tcpPingVar    = 0.f;

	MUMBLE_MESSAGE_CDTOR(Ping)
};

struct Message::Reject : public Message {
	enum RejectType : uint8_t {
		None,
		WrongVersion,
		InvalidUsername,
		WrongUserPW,
		WrongServerPW,
		UsernameInUse,
		ServerFull,
		NoCertificate,
		AuthenticatorFail
	};

	RejectType type    = None;
	std::string reason = {};

	MUMBLE_MESSAGE_CDTOR(Reject)
};

struct Message::ServerSync : public Message {
	uint32_t session        = UINT32_MAX;
	uint32_t maxBandwidth   = 0;
	std::string welcomeText = {};
	uint64_t permissions    = 0;

	MUMBLE_MESSAGE_CDTOR(ServerSync)
};

struct Message::ChannelRemove : public Message {
	uint32_t channelID = UINT32_MAX;

	MUMBLE_MESSAGE_CDTOR(ChannelRemove)
};

struct Message::ChannelState : public Message {
	uint32_t channelID                       = UINT32_MAX;
	std::optional< uint32_t > parent         = {};
	std::string name                         = {};
	std::vector< uint32_t > links            = {};
	std::string description                  = {};
	std::vector< uint32_t > linksAdd         = {};
	std::vector< uint32_t > linksRemove      = {};
	bool temporary                           = false;
	int32_t position                         = 0;
	std::vector< std::byte > descriptionHash = {};
	uint32_t maxUsers                        = 0;
	bool isEnterRestricted                   = false;
	bool canEnter                            = true;

	MUMBLE_MESSAGE_CDTOR(ChannelState)
};

struct Message::UserRemove : public Message {
	uint32_t session   = UINT32_MAX;
	uint32_t actor     = UINT32_MAX;
	std::string reason = {};
	bool ban           = false;

	MUMBLE_MESSAGE_CDTOR(UserRemove)
};

struct Message::UserState : public Message {
	uint32_t session                                 = UINT32_MAX;
	uint32_t actor                                   = UINT32_MAX;
	std::string name                                 = {};
	std::optional< uint32_t > userID                 = {};
	uint32_t channelID                               = UINT32_MAX;
	bool mute                                        = false;
	bool deaf                                        = false;
	bool suppress                                    = false;
	bool selfMute                                    = false;
	bool selfDeaf                                    = false;
	std::vector< std::byte > texture                 = {};
	std::vector< std::byte > pluginContext           = {};
	std::string pluginIdentity                       = {};
	std::string comment                              = {};
	std::string hash                                 = {};
	std::vector< std::byte > commentHash             = {};
	std::vector< std::byte > textureHash             = {};
	bool prioritySpeaker                             = false;
	bool recording                                   = false;
	std::vector< std::string > temporaryAccessTokens = {};
	std::vector< uint32_t > listeningChannelAdd      = {};
	std::vector< uint32_t > listeningChannelRemove   = {};

	MUMBLE_MESSAGE_CDTOR(UserState)
};

struct Message::BanList : public Message {
	struct BanEntry {
		IP address         = {};
		uint32_t mask      = UINT32_MAX;
		std::string name   = {};
		std::string hash   = {};
		std::string reason = {};
		std::string start  = {};
		uint32_t duration  = 0;
	};

	std::vector< BanEntry > bans = {};
	bool query                   = false;

	MUMBLE_MESSAGE_CDTOR(BanList)
};

struct Message::TextMessage : public Message {
	uint32_t actor                    = UINT32_MAX;
	std::vector< uint32_t > session   = {};
	std::vector< uint32_t > channelID = {};
	std::vector< uint32_t > treeID    = {};
	std::string message               = {};

	MUMBLE_MESSAGE_CDTOR(TextMessage)
};

struct Message::PermissionDenied : public Message {
	enum DenyType : uint8_t {
		Text,
		Permission,
		SuperUser,
		ChannelName,
		TextTooLong,
		H9K,
		TemporaryChannel,
		MissingCertificate,
		UserName,
		ChannelFull,
		NestingLimit,
		ChannelCountLimit,
		ChannelListenerLimit,
		UserListenerLimit
	};

	std::optional< uint32_t > permission = {};
	std::optional< uint32_t > channelID  = {};
	uint32_t session                     = UINT32_MAX;
	std::string reason                   = {};
	DenyType type                        = H9K;
	std::optional< std::string > name    = {};

	MUMBLE_MESSAGE_CDTOR(PermissionDenied)
};

struct Message::ACL : public Message {
	struct ChanGroup {
		std::string name;
		bool inherited                           = true;
		bool inherit                             = true;
		bool inheritable                         = true;
		std::vector< uint32_t > add              = {};
		std::vector< uint32_t > remove           = {};
		std::vector< uint32_t > inheritedMembers = {};
	};

	struct ChanACL {
		bool applyHere                   = true;
		bool applySubs                   = true;
		bool inherited                   = true;
		std::optional< uint32_t > userID = {};
		std::string group                = {};
		uint32_t grant                   = 0;
		uint32_t deny                    = 0;
	};

	uint32_t channelID              = {};
	bool inheritACLs                = true;
	std::vector< ChanGroup > groups = {};
	std::vector< ChanACL > acls     = {};
	bool query                      = false;

	MUMBLE_MESSAGE_CDTOR(ACL)
};

struct Message::QueryUsers : public Message {
	std::vector< uint32_t > ids      = {};
	std::vector< std::string > names = {};

	MUMBLE_MESSAGE_CDTOR(QueryUsers)
};

struct Message::CryptSetup : public Message {
	std::vector< std::byte > key         = {};
	std::vector< std::byte > clientNonce = {};
	std::vector< std::byte > serverNonce = {};

	MUMBLE_MESSAGE_CDTOR(CryptSetup)
};

struct Message::ContextActionModify : public Message {
	enum Context : uint8_t {
		Server  = 0b00000001,
		Channel = 0b00000010,
		User    = 0b00000100,
	};

	enum Operation : uint8_t { Add, Remove };

	std::string action  = {};
	std::string text    = {};
	uint32_t context    = 0;
	Operation operation = Add;

	MUMBLE_MESSAGE_CDTOR(ContextActionModify)
};

struct Message::ContextAction : public Message {
	std::optional< uint32_t > session   = {};
	std::optional< uint32_t > channelID = {};
	std::string action                  = {};

	MUMBLE_MESSAGE_CDTOR(ContextAction)
};

struct Message::UserList : public Message {
	struct User {
		uint32_t userID                       = UINT32_MAX;
		std::string name                      = {};
		std::string lastSeen                  = {};
		std::optional< uint32_t > lastChannel = {};
	};

	std::vector< User > users = {};

	MUMBLE_MESSAGE_CDTOR(UserList)
};

struct Message::VoiceTarget : public Message {
	struct Target {
		std::vector< uint32_t > session     = {};
		std::optional< uint32_t > channelID = {};
		std::string group                   = {};
		bool links                          = false;
		bool children                       = false;
	};

	uint32_t id                   = UINT32_MAX;
	std::vector< Target > targets = {};

	MUMBLE_MESSAGE_CDTOR(VoiceTarget)
};

struct Message::PermissionQuery : public Message {
	uint32_t channelID   = UINT32_MAX;
	uint32_t permissions = 0;
	bool flush           = false;

	MUMBLE_MESSAGE_CDTOR(PermissionQuery)
};

struct Message::CodecVersion : public Message {
	int32_t alpha    = 0;
	int32_t beta     = 0;
	bool preferAlpha = true;
	bool opus        = false;

	MUMBLE_MESSAGE_CDTOR(CodecVersion)
};

struct Message::UserStats : public Message {
	struct Stats {
		uint32_t good   = 0;
		uint32_t late   = 0;
		uint32_t lost   = 0;
		uint32_t resync = 0;
	};

	uint32_t session         = UINT32_MAX;
	bool statsOnly           = false;
	Cert::Chain certificates = {};
	Stats fromClient         = {};
	Stats fromServer         = {};

	uint32_t udpPackets = 0;
	uint32_t tcpPackets = 0;
	uint32_t udpPingAvg = 0;
	uint32_t udpPingVar = 0;
	uint32_t tcpPingAvg = 0;
	uint32_t tcpPingVar = 0;

	Version version                     = {};
	std::vector< int32_t > celtVersions = {};
	IP address                          = {};
	uint32_t bandwidth                  = 0;
	uint32_t onlinesecs                 = 0;
	uint32_t idlesecs                   = 0;
	bool strongCertificate              = false;
	bool opus                           = false;

	MUMBLE_MESSAGE_CDTOR(UserStats)
};

struct Message::RequestBlob : public Message {
	std::vector< uint32_t > sessionTexture     = {};
	std::vector< uint32_t > sessionComment     = {};
	std::vector< uint32_t > channelDescription = {};

	MUMBLE_MESSAGE_CDTOR(RequestBlob)
};

struct Message::ServerConfig : public Message {
	uint32_t maxBandwidth       = 0;
	std::string welcomeText     = {};
	bool allowHTML              = false;
	uint32_t messageLength      = 0;
	uint32_t imageMessageLength = 0;
	uint32_t maxUsers           = 0;
	bool recordingAllowed       = false;

	MUMBLE_MESSAGE_CDTOR(ServerConfig)
};

struct Message::SuggestConfig : public Message {
	std::optional< uint32_t > version = {};
	std::optional< bool > positional  = {};
	std::optional< bool > pushToTalk  = {};

	MUMBLE_MESSAGE_CDTOR(SuggestConfig)
};

struct Message::PluginDataTransmission : public Message {
	uint32_t senderSession                   = UINT32_MAX;
	std::vector< uint32_t > receiverSessions = {};
	std::vector< std::byte > data            = {};
	std::string dataID                       = {};

	MUMBLE_MESSAGE_CDTOR(PluginDataTransmission)
};
} // namespace mumble

MUMBLE_ENUM_OPERATORS(mumble::Message::Perm)

#endif
