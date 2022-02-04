// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Node.hpp"

#include "User.hpp"
#include "UserManager.hpp"

#include "mumble/Endian.hpp"
#include "mumble/IP.hpp"
#include "mumble/Message.hpp"

#include <fstream>
#include <sstream>

using namespace mumble;

Node::Node(const std::shared_ptr< UserManager > &userManager, const std::string_view tcpIP, const uint32_t tcpPort,
		   const std::string_view udpIP, const uint32_t udpPort, const uint32_t bandwidth)
	: m_ok(false), m_bandwidth(bandwidth), m_userManager(userManager) {
	Endpoint endpoint(tcpIP, tcpPort);
	auto code = m_server.bindTCP(endpoint);
	if (code != Code::Success) {
		printf("Node(): TCP bind failed with error \"%s\"!\n", text(code).data());
		return;
	}

	printf("TCP: [%s]:%hu\n", endpoint.ip.text().data(), endpoint.port);

	endpoint = Endpoint(udpIP, udpPort);
	code     = m_server.bindUDP(endpoint);
	if (code != Code::Success) {
		printf("Node(): UDP bind failed with error \"%s\"!\n", text(code).data());
		return;
	}

	printf("UDP: [%s]:%hu\n", endpoint.ip.text().data(), endpoint.port);

	m_ok = true;
}

Node::~Node() = default;

Node::operator bool() const {
	return m_ok;
}

bool Node::start() {
	Server::FeedbackUDP feedbackUDP;

	feedbackUDP.started = []() { printf("UDP started!\n"); };
	feedbackUDP.stopped = []() { printf("UDP stopped!\n"); };

	feedbackUDP.failed = [](const Code code) { printf("UDP failed with error \"%s\"!\n", text(code).data()); };

	feedbackUDP.timeout = []() { return 10000; };

	feedbackUDP.ping = [this](Endpoint &endpoint, Mumble::PingUDP &ping) {
		ping.versionBlob  = Endian::toNetwork(Mumble::version().blob());
		ping.sessions     = Endian::toNetwork(m_userManager->num());
		ping.maxSessions  = Endian::toNetwork(m_userManager->max());
		ping.maxBandwidth = Endian::toNetwork(m_bandwidth);

		m_server.sendUDP(endpoint, { reinterpret_cast< std::byte * >(&ping), sizeof(ping) });
	};

	feedbackUDP.encrypted = [this](Endpoint &endpoint, BufRef buf) {
		auto user = (*m_userManager)[endpoint];

		Buf decrypted(buf.size());

		if (!user) {
			user = m_userManager->tryDecrypt(decrypted, buf, endpoint);
			if (user) {
				printf("[%s]:%u <-> #%u association added to cache!\n", endpoint.ip.text().data(), endpoint.port,
					   user->id());
			} else {
				return;
			}
		}

		auto size = user->decrypt(decrypted, buf);
		if (!size) {
			return;
		}

		if (Mumble::packetType(decrypted) == Mumble::TypeUDP::Ping) {
			size = user->encrypt(buf, { decrypted.data(), size });
			if (!size) {
				return;
			}

			user->send({ endpoint, buf.first(size) });
		}
	};

	auto code = m_server.startUDP(feedbackUDP);
	if (code != Code::Success) {
		printf("Server::startUDP() failed with error \"%s\"!\n", text(code).data());
		return false;
	}

	Server::FeedbackTCP feedbackTCP;

	feedbackTCP.started = []() { printf("TCP started!\n"); };
	feedbackTCP.stopped = []() { printf("TCP stopped!\n"); };

	feedbackTCP.failed = [](const Code code) { printf("TCP failed with error \"%s\"!\n", text(code).data()); };

	feedbackTCP.timeout = []() { return 10000; };

	feedbackTCP.connection = [this](const Endpoint &endpoint) {
		printf("Incoming connection from [%s]:%u\n", endpoint.ip.text().data(), endpoint.port);
		return !m_userManager->full();
	};

	feedbackTCP.session = [this](Session::P *p) {
		const auto id = m_userManager->reserveID();
		if (!id) {
			return false;
		}

		auto user    = std::make_shared< User >(p, id.value());
		auto userPtr = std::weak_ptr< User >(user);

		Session::Feedback feedback;

		feedback.opened = [userPtr]() {
			if (const auto user = userPtr.lock()) {
				printf("[#%u] Created!\n", user->id());

				const auto certChain = user->peerCert();
				if (!certChain.size()) {
					printf("[#%u] Didn't provide a certificate!\n", user->id());
					return;
				}

				const auto attributes = certChain[0].subjectAttributes();
				for (const auto &attribute : attributes) {
					printf("[#%u] %s: %s\n", user->id(), attribute.first.data(), attribute.second.data());
				}
			}
		};

		feedback.closed = [this, userPtr]() {
			if (auto user = userPtr.lock()) {
				const auto id = user->id();
				user.reset();
				m_userManager->del(id);

				printf("[#%u] Closed!\n", id);
			}
		};

		feedback.failed = [this, userPtr](const Code code) {
			if (auto user = userPtr.lock()) {
				const auto id = user->id();
				user.reset();
				m_userManager->del(id);

				printf("[#%u] Failed with error \"%s\"!\n", id, text(code).data());
			}
		};

		feedback.message = [this, userPtr](Message *message) {
			const auto ptr = std::unique_ptr< Message >(message);

			const auto user = userPtr.lock();
			if (!user) {
				return;
			}

			using Type = Message::Type;

			if (message->type() != Type::UDPTunnel) {
				printf("[#%u] %s received!\n", user->id(), message->typeText().data());
			}

			using Perm = Message::Perm;

			switch (message->type()) {
				case Type::Version: {
					const auto ver = static_cast< Message::Version * >(message);
					ver->version   = Mumble::version().blob();
					ver->release   = "Custom server";
					ver->os.clear();
					ver->osVersion.clear();
					user->send(*ver);

					break;
				}
				case Type::UDPTunnel:
					break;
				case Type::Authenticate: {
					const auto auth = static_cast< const Message::Authenticate * >(message);
					printf("username: %s | password: %s\n", auth->username.c_str(), auth->password.c_str());

					Message::CryptSetup crypt;
					crypt.key.assign(user->key().begin(), user->key().end());
					crypt.clientNonce.assign(user->decryptNonce().begin(), user->decryptNonce().end());
					crypt.serverNonce.assign(user->encryptNonce().begin(), user->encryptNonce().end());
					user->send(crypt);

					Message::CodecVersion codec;
					codec.opus = true;
					user->send(codec);

					Message::ChannelState channel;
					channel.name              = "Root";
					channel.channelID         = 0;
					channel.position          = 0;
					channel.isEnterRestricted = false;
					channel.canEnter          = true;
					user->send(channel);

					Message::UserState state;
					state.session   = user->id();
					state.name      = auth->username;
					state.channelID = channel.channelID;
					user->send(state);

					Message::ServerSync sync;
					sync.session      = user->id();
					sync.maxBandwidth = m_bandwidth;
					sync.welcomeText  = "Welcome to the best server in the world!";
					sync.permissions  = static_cast< uint32_t >(Perm::Enter | Perm::Speak | Perm::TextMessage);
					user->send(sync);

					Message::ServerConfig config;
					config.maxBandwidth     = sync.maxBandwidth;
					config.welcomeText      = sync.welcomeText;
					config.allowHTML        = true;
					config.maxUsers         = m_userManager->max();
					config.recordingAllowed = true;
					user->send(config);

					break;
				}
				case Type::Ping: {
					const auto ping = static_cast< Message::Ping * >(message);
					user->send(*ping);
					break;
				}
				case Type::Reject:
					break;
				case Type::ServerSync:
					break;
				case Type::ChannelRemove:
					break;
				case Type::ChannelState:
					break;
				case Type::UserRemove:
					break;
				case Type::UserState:
					break;
				case Type::BanList:
					break;
				case Type::TextMessage:
					break;
				case Type::PermissionDenied:
					break;
				case Type::ACL:
					break;
				case Type::QueryUsers:
					break;
				case Type::CryptSetup:
					break;
				case Type::ContextActionModify:
					break;
				case Type::ContextAction:
					break;
				case Type::UserList:
					break;
				case Type::VoiceTarget:
					break;
				case Type::PermissionQuery: {
					const auto query = static_cast< Message::PermissionQuery * >(message);

					query->permissions = static_cast< uint32_t >(Perm::Enter | Perm::Speak | Perm::TextMessage);

					user->send(*query);
					break;
				}
				case Type::CodecVersion:
					break;
				case Type::UserStats: {
					const auto stats  = static_cast< Message::UserStats  *>(message);
					const auto target = (*m_userManager)[stats->session];
					if (!target) {
						break;
					}

					stats->fromClient.good = target->good();
					stats->fromClient.late = target->late();
					stats->fromClient.lost = target->lost();

					stats->address      = target->peerEndpoint().ip;
					stats->certificates = target->peerCert();
					stats->opus         = true;

					user->send(*stats);
					break;
				}
				case Type::RequestBlob:
					break;
				case Type::ServerConfig:
					break;
				case Type::SuggestConfig:
					break;
				case Type::PluginDataTransmission:
					break;
				case Type::Unknown:
					break;
			}
		};

		const auto code = user->start(feedback, m_certChain, m_certKey);
		if (code == Code::Success) {
			m_userManager->add(user);
		} else {
			printf("Session failed to start with error \"%s\"!\n", text(code).data());
		}

		return true;
	};

	code = m_server.startTCP(feedbackTCP);
	if (code != Code::Success) {
		printf("Server::startTCP() failed with error \"%s\"!\n", text(code).data());
		return false;
	}

	return true;
}

bool Node::setCert(const std::string_view certPath, const std::string_view keyPath) {
	std::ifstream certStream(certPath.data());
	std::stringstream buffer;
	buffer << certStream.rdbuf();

	Cert cert(buffer.str());
	if (!cert) {
		printf("Node::setCert(): Failed to load certificate!\n");
		return false;
	}

	std::ifstream keyStream(keyPath.data());
	buffer.str({});
	buffer.clear();
	buffer << keyStream.rdbuf();

	Key key(buffer.str(), true);
	if (!key) {
		printf("Node::setCert(): Failed to load key!\n");
		return false;
	}

	m_certChain = { cert };
	m_certKey   = key;

	return true;
}
