#pragma comment(lib, "ws2_32.lib")

#include "network_handler.hpp"

#include <vector>
#include <utility>
#include <array>

namespace networked_UNO {
	static network_handler* s_network_handler = nullptr;

	bool network_handler::start_networking() {
		WSAData wsa_data;
		if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
			return false;
		
		return true;
	}

	bool network_handler::terminate_networking() {
		if (s_network_handler == nullptr)
			return false;
		if (WSACleanup() != 0)
			return false;

		delete s_network_handler;
		s_network_handler = nullptr;

		return true;
	}

	network_handler* network_handler::get_network_handler() {
		return s_network_handler;
	}
}