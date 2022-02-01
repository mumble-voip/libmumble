// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "MumbleInit.hpp"

#include "mumble/Client.hpp"
#include "mumble/Message.hpp"

#include <cstdio>

#include <toml.hpp>

using namespace mumble;

static Session::Feedback sessionFeedback(Session &session) {
	Session::Feedback feedback;

	feedback.opened = [&session]() {
		printf("Session opened!\n");

		Message::Version ver;
		ver.version = Mumble::version().blob();
		ver.release = "Custom client";
		session.sendTCP(ver);
	};

	feedback.closed = []() { printf("Session closed!\n"); };

	feedback.failed = [](const Code code) { printf("Session failed with error \"%s\"!\n", toString(code).data()); };

	feedback.message = [](Message *message) {
		const auto ptr = std::unique_ptr< Message >(message);

		if (message->type() != Message::Type::UDPTunnel) {
			printf("%s received!\n", message->typeText().data());
		}
	};

	return feedback;
}

int32_t main(const int argc, const char **argv) {
	if (argc > 2) {
		printf("Usage: `example_client <config file>`\n");
		return 1;
	}

	std::string confPath = "config.toml";
	if (argc > 1) {
		confPath = argv[1];
	}

	const auto conf = toml::parse(confPath);

	MumbleInit mumbleInit;
	if (!mumbleInit) {
		return 2;
	}

	const auto local = toml::find(conf, "local");
	const auto peer  = toml::find(conf, "peer");

	const auto localTcpIP   = toml::find< std::string_view >(local, "tcpIP");
	const auto localTcpPort = toml::find< uint16_t >(local, "tcpPort");

	const auto peerTcpIP   = toml::find< std::string_view >(peer, "tcpIP");
	const auto peerTcpPort = toml::find< uint16_t >(peer, "tcpPort");

	Client client;

	const auto ret = client.connect({ peerTcpIP, peerTcpPort }, { localTcpIP, localTcpPort });
	if (ret.first != Code::Success) {
		printf("Client::connect() failed with error \"%s\"!\n", toString(ret.first).data());
		return 3;
	}

	Session session(ret.second);

	const auto code = session.start(sessionFeedback(session));
	if (code != Code::Success) {
		printf("Session::start() failed with error \"%s\"!\n", toString(code).data());
		return 4;
	}

	getchar();

	printf("Shutting down...\n");

	return 0;
}
