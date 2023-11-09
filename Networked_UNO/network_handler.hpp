#pragma once

#include <WinSock2.h>
#include <winsock.h>

#include <cstdint>

namespace networked_UNO {
	class network_handler {
		static const uint64_t WAIT_TIME = 100000;

		network_handler() = default;
		~network_handler() = default;

	public:
		static bool start_networking();

		static network_handler* get_network_handler();

		static bool terminate_networking();

		bool connect_to_server();
	};
}