#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <winsock.h>

#include <string>
#include <vector>
#include <cstdint>
#include <optional>

namespace networked_UNO {
	const std::string IDENTITY_STRING = "Networked_UNO_CNT_4007_2023";
	const uint32_t CLIENT_RETRY_ATTEMPTS = 10;
	const uint32_t TCP_TIME_OUT_MS = 1000;
	const uint32_t UDP_TIME_OUT_MS = 100;
	const size_t DATA_RECV_SIZE = 32;
	const uint16_t PORT = 8757;

	class network_handler {
	protected:
		SOCKET tcp_socket = INVALID_SOCKET;

		network_handler(SOCKET tcp_socket)
			: tcp_socket(tcp_socket) {}

		virtual ~network_handler();

	public:
		static network_handler* start_networking();

		virtual bool send_data(const std::vector<char>& data) = 0;
		virtual std::optional<std::vector<char>> receive_data() = 0;
	};
}