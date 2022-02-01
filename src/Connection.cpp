// Copyright 2022 The Mumble Developers. All rights reserved.
// Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file at the root of the
// Mumble source tree or at <https://www.mumble.info/LICENSE>.

#include "Connection.hpp"

#include "Pack.hpp"

#include <thread>

using namespace mumble;

Connection::Connection(SocketTLS &&socket) : SocketTLS(std::move(socket)) {
	setBlocking(false);
}

Connection::~Connection() {
	stop();
}

void Connection::start(const Feedback &feedback) {
	m_feedback = feedback;

	m_timeouts = 0;

	m_thread = std::make_unique< std::jthread >(std::bind_front(&Connection::thread, this));
}

void Connection::stop() {
	m_thread->request_stop();

	trigger();

	if (m_thread->joinable()) {
		m_thread->join();
	}
}

mumble::Code Connection::toCode(const Code code) {
	using Code = mumble::Code;
	using TLS  = SocketTLS::Code;

	switch (code) {
		case TLS::Memory:
			return Code::Memory;
		case TLS::Failure:
			return Code::Failure;
		case TLS::Unknown:
			break;
		case TLS::Success:
			return Code::Success;
		case TLS::Retry:
			return Code::Retry;
		case TLS::Shutdown:
			return Code::Disconnect;
		case TLS::WaitIn:
		case TLS::WaitOut:
			return Code::Busy;
	}

	return Code::Unknown;
}

mumble::Code Connection::handleCode(const Code code) {
	using Code = mumble::Code;
	using TLS  = SocketTLS::Code;

	const auto mumbleCode = toCode(code);

	switch (code) {
		case TLS::Memory:
		case TLS::Failure:
		case TLS::Unknown:
			m_feedback.failed(mumbleCode);
			break;
		case TLS::Shutdown:
			m_feedback.closed();
		case TLS::Success:
			m_timeouts = 0;
		case TLS::Retry:
			break;
		case TLS::WaitIn:
		case TLS::WaitOut: {
			const auto state = wait(code == TLS::WaitIn, code == TLS::WaitOut, m_feedback.timeout());

			if (state & Socket::InReady || state & Socket::OutReady) {
				return Code::Retry;
			}

			if (state & Socket::Triggered) {
				return Code::Retry;
			}

			if (state & Socket::Timeout) {
				if (++m_timeouts < m_feedback.timeouts()) {
					return Code::Retry;
				}

				m_feedback.failed(Code::Timeout);
				return Code::Timeout;
			}

			if (state & Socket::Disconnected) {
				m_feedback.closed();
				return Code::Disconnect;
			}

			if (state & Socket::Error) {
				m_feedback.failed(Code::Failure);
				return Code::Failure;
			}
		}
	}

	return mumbleCode;
}

mumble::Code Connection::read(const std::stop_token &stopToken, BufRef buf) {
	using Code = mumble::Code;

	while (!stopToken.stop_requested()) {
		const auto code = handleCode(SocketTLS::read(buf));
		if (code == Code::Retry) {
			continue;
		} else {
			return code;
		}
	}

	return Code::Cancel;
}

mumble::Code Connection::write(const std::stop_token &stopToken, BufRefConst buf) {
	using Code = mumble::Code;

	while (!stopToken.stop_requested()) {
		const auto code = handleCode(SocketTLS::write(buf));
		if (code == Code::Retry) {
			continue;
		} else {
			return code;
		}
	}

	return Code::Cancel;
}

void Connection::thread(const std::stop_token stopToken) {
	using Code = mumble::Code;

	while (!stopToken.stop_requested()) {
		const auto code = handleCode(m_server ? accept() : connect());
		if (code == Code::Success) {
			break;
		}

		if (code == Code::Retry) {
			continue;
		}

		return;
	}

	if (stopToken.stop_requested()) {
		return;
	}

	m_feedback.opened();

	auto state = Socket::InReady;

	while (!stopToken.stop_requested()) {
		if (state & Socket::InReady) {
			do {
				Pack::NetHeader header;
				if (read(stopToken, { reinterpret_cast< std::byte * >(&header), sizeof(header) }) != Code::Success) {
					return;
				}

				if (header.size > UINT16_MAX) {
					m_feedback.failed(Code::Invalid);
					return;
				}

				Pack pack(header);

				if (read(stopToken, pack.data()) != Code::Success) {
					return;
				}

				m_feedback.message(pack);
			} while (pending() >= sizeof(Pack::NetHeader));
		}

		state = wait(true, false, m_feedback.timeout());
	}

	disconnect();

	m_feedback.closed();
}
