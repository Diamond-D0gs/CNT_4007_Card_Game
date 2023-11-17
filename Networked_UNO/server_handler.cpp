#include "server_handler.hpp"

namespace networked_UNO {
	bool server_handler::send_data(const std::vector<char>& data) {
		return true;
	}

	std::optional<std::vector<char>> server_handler::receive_data() {
		return std::optional<std::vector<char>>(std::nullopt);
	}
}