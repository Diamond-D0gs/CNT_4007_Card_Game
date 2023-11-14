#pragma once

#include "network_handler.hpp"

namespace networked_UNO {
	class network_handler;

	class client_handler : public network_handler {
	private:
		client_handler(SOCKET tcp_socket)
			: network_handler(tcp_socket) {}

		~client_handler() = default;

	public:
		bool send_data(const std::vector<char>& data);
		std::optional<std::vector<char>> receive_data();

		friend network_handler;
	};
}