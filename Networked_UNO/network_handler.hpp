#pragma once

#include <cstdint>
#include <optional>

namespace networked_UNO {
	class network_handler {
		static const uint64_t WAIT_TIME = 100000;

		network_handler() {}

	public:
		network_handler(network_handler& network_handler) {}

		static std::optional<network_handler> get_network_handler() noexcept;

		bool connect_to_server();
	};
}