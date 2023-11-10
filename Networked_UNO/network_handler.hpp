#pragma once

#include <WinSock2.h>
#include <winsock.h>
#include <string>
#include <atomic>

#include <cstdint>

namespace networked_UNO {
	class network_handler {
		static const uint64_t WAIT_TIME = 100000;

		struct connection_results {
			SOCKET socket;
			std::string error;
		};

		network_handler() = default;
		~network_handler() = default;

		static connection_results client_connection(SOCKET udp_socket, std::atomic_bool& running);
		static connection_results server_connection(SOCKET udp_socket, std::atomic_bool& running);
		

	public:
		static network_handler* start_networking();
		static bool terminate_networking(network_handler* network_handler);
	};
}