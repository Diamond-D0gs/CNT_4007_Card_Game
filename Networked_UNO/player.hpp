#pragma once

#include "cards.hpp"

#include <vector>

namespace networked_UNO {
	class player {
		std::vector<const card_base*> hand;

	public:
		void add_cards_to_hand(const std::vector<const card_base*>& cards);
		
		const std::vector<const card_base*>& get_cards_in_hand() const {
			return hand;
		}
	};
}