#include "player.hpp"

#include <cstdlib>

namespace networked_UNO {
	void player::add_cards_to_hand(const std::vector<const card_base*>& cards) {
		size_t size = hand.size();
		hand.resize(size + cards.size());
		memcpy(&hand[size], cards.data(), (cards.size() * sizeof(card_base*)));
	}
}