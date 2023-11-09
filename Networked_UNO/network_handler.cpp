#include "network_handler.hpp"

#include <sockpp/udp_socket.h>

#include <vector>
#include <utility>
#include <array>

namespace networked_UNO {
	std::optional<network_handler> network_handler::get_network_handler() {
		static bool sockpp_init = false;
		if (!sockpp_init) {
			sockpp::initialize();
			sockpp_init = true;
		}

		return std::optional<network_handler>(network_handler());
	}
}