#include "cards.hpp"
#include "player.hpp"
#include "network_handler.hpp"
#include "card_respository.hpp"

#include <random>

int main(void) {
	networked_UNO::network_handler* test = networked_UNO::network_handler::start_networking();
	if (test == nullptr)
		return -1;

	return 0;
}