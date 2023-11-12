#pragma once

#include <WinSock2.h>
#include <winsock.h>
#include <string>
#include <atomic>

#include <cstdint>

namespace networked_UNO {
	class network_handler {
		network_handler() = default;
		~network_handler() = default;

	public:
		static network_handler* start_networking();
		static bool terminate_networking(network_handler* network_handler);
	};
}