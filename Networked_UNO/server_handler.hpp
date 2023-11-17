#pragma once

#include "network_handler.hpp"
#include "card_repository.hpp"

namespace networked_UNO {
	class server_handler : public network_handler {
		SOCKET client_socket = INVALID_SOCKET;

		server_handler(SOCKET client_socket, SOCKET tcp_socket)
			: client_socket(client_socket), network_handler(tcp_socket) {}
	
	public:
		bool send_data(const std::vector<char>& data);
		std::optional<std::vector<char>> receive_data();

		friend network_handler;
	};
}