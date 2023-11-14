#include "client_handler.hpp"

#include <iostream>

namespace networked_UNO {
	bool client_handler::send_data(const std::vector<char>& data) {
		if (send(tcp_socket, data.data(), static_cast<int>(data.size()), 0) != 0) {
			std::cout << "Error! Unable to send data to server. Terminating.\n";
			return false;
		}

		return true;
	}

	std::optional<std::vector<char>> client_handler::receive_data() {
		std::vector<char> data(1024);
		
		int data_received = recv(tcp_socket, data.data(), static_cast<int>(data.size()), 0);
		if (data_received <= 0) {
			std::cout << "Error! Unable to receive data from server. Terminating.\n";
			return std::optional<std::vector<char>>(std::nullopt);
		}

		data.resize(data_received);
		return std::optional<std::vector<char>>(data);
	}
}