#include "cards.hpp"
#include "player.hpp"
#include "network_handler.hpp"
#include "card_respository.hpp"

#include <random>

int main(void) {
	networked_UNO::network_handler* network_handler = networked_UNO::network_handler::create_network_handler();
	if (network_handler == nullptr)
		return 0;

	network_handler->connect_to_server();

	delete network_handler;

	return 0;
}