#pragma once

#include "cards.hpp"

#include <vector>

namespace networked_UNO {
	class card_repository {
		const std::vector<const card_base*> cards;

		static const std::vector<const card_base*> create_cards();

	public:
		card_repository() : cards(create_cards()) {}

		~card_repository() = default;

		std::vector<const card_base*> create_shuffled_deck(const uint32_t seed) const;

		const card_base* decode_card_ID(const uint8_t card_ID) const;
	};
}