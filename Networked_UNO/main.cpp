#include "cards.hpp"
#include "player.hpp"
#include "network_handler.hpp"
#include "card_respository.hpp"

#include <random>

int main(void) {
	auto test = networked_UNO::network_handler::start_networking();

	return 0;
}