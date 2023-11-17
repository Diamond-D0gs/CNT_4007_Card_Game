#include "card_repository.hpp"

#include <algorithm>
#include <random>

namespace networked_UNO {
	void duplicate_card(std::vector<const card_base*>& cards, const uint32_t quantity, const card_base* card) {
		for (uint32_t i = 0; i < quantity; ++i)
			cards.push_back(card);
	}

	const std::vector<const card_base*> card_repository::create_cards() {
		std::vector<const card_base*> cards;

		// Insert the null card.
		cards.push_back(nullptr);

		// Create numbered cards.
		// Zero
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 0, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 0, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 0, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 0, CARD_COLOR::BLUE));
		// One
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 1, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 1, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 1, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 1, CARD_COLOR::BLUE));
		// Two
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::BLUE));
		// Three
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 3, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 3, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 3, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 3, CARD_COLOR::BLUE));
		// Four
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 4, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 4, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 4, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 4, CARD_COLOR::BLUE));
		// Five
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 5, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 5, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 5, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 5, CARD_COLOR::BLUE));
		// Six
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 6, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 6, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 6, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 6, CARD_COLOR::BLUE));
		// Seven
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 7, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 7, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 7, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 7, CARD_COLOR::BLUE));
		// Eight
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 8, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 8, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 8, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 8, CARD_COLOR::BLUE));
		// Nine
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 9, CARD_COLOR::RED));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 9, CARD_COLOR::YELLOW));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 9, CARD_COLOR::GREEN));
		cards.push_back(new numbered_card(static_cast<uint8_t>(cards.size()), 9, CARD_COLOR::BLUE));
		
		// Create skip cards.
		cards.push_back(new skip_card(static_cast<uint8_t>(cards.size()), CARD_COLOR::RED));
		cards.push_back(new skip_card(static_cast<uint8_t>(cards.size()), CARD_COLOR::YELLOW));
		cards.push_back(new skip_card(static_cast<uint8_t>(cards.size()), CARD_COLOR::GREEN));
		cards.push_back(new skip_card(static_cast<uint8_t>(cards.size()), CARD_COLOR::BLUE));

		// Create draw cards.
		cards.push_back(new draw_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::RED));
		cards.push_back(new draw_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::YELLOW));
		cards.push_back(new draw_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::GREEN));
		cards.push_back(new draw_card(static_cast<uint8_t>(cards.size()), 2, CARD_COLOR::BLUE));
		cards.push_back(new draw_card(static_cast<uint8_t>(cards.size()), 4, CARD_COLOR::WILD));

		// Create the change cards.
		cards.push_back(new change_card(static_cast<uint8_t>(cards.size())));

		return cards;
	}

	std::vector<const card_base*> card_repository::create_shuffled_deck(uint32_t seed) const {
		std::vector<const card_base*> deck;

		// Insert the numbered cards.
		// Zero (Only one of each zero).
		deck.push_back(cards[1]);
		deck.push_back(cards[2]);
		deck.push_back(cards[3]);
		deck.push_back(cards[4]);
		// One (Two of each other number).
		duplicate_card(deck, 2, cards[5]);
		duplicate_card(deck, 2, cards[6]);
		duplicate_card(deck, 2, cards[7]);
		duplicate_card(deck, 2, cards[8]);
		// Two
		duplicate_card(deck, 2, cards[9]);
		duplicate_card(deck, 2, cards[10]);
		duplicate_card(deck, 2, cards[11]);
		duplicate_card(deck, 2, cards[12]);
		// Three
		duplicate_card(deck, 2, cards[13]);
		duplicate_card(deck, 2, cards[14]);
		duplicate_card(deck, 2, cards[15]);
		duplicate_card(deck, 2, cards[16]);
		// Four
		duplicate_card(deck, 2, cards[17]);
		duplicate_card(deck, 2, cards[18]);
		duplicate_card(deck, 2, cards[19]);
		duplicate_card(deck, 2, cards[20]);
		// Five
		duplicate_card(deck, 2, cards[21]);
		duplicate_card(deck, 2, cards[22]);
		duplicate_card(deck, 2, cards[23]);
		duplicate_card(deck, 2, cards[24]);
		// Six
		duplicate_card(deck, 2, cards[25]);
		duplicate_card(deck, 2, cards[26]);
		duplicate_card(deck, 2, cards[27]);
		duplicate_card(deck, 2, cards[28]);
		// Seven
		duplicate_card(deck, 2, cards[29]);
		duplicate_card(deck, 2, cards[30]);
		duplicate_card(deck, 2, cards[31]);
		duplicate_card(deck, 2, cards[32]);
		// Eight
		duplicate_card(deck, 2, cards[33]);
		duplicate_card(deck, 2, cards[34]);
		duplicate_card(deck, 2, cards[35]);
		duplicate_card(deck, 2, cards[36]);
		// Nine
		duplicate_card(deck, 2, cards[37]);
		duplicate_card(deck, 2, cards[38]);
		duplicate_card(deck, 2, cards[39]);
		duplicate_card(deck, 2, cards[40]);

		// Insert skip cards (Four of each color).
		duplicate_card(deck, 4, cards[41]);
		duplicate_card(deck, 4, cards[42]);
		duplicate_card(deck, 4, cards[43]);
		duplicate_card(deck, 4, cards[44]);

		// Insert draw cards (Two of each color, four of WILD).
		duplicate_card(deck, 2, cards[45]);
		duplicate_card(deck, 2, cards[46]);
		duplicate_card(deck, 2, cards[47]);
		duplicate_card(deck, 2, cards[48]);
		duplicate_card(deck, 4, cards[49]);

		// Insert change cards (Four WILDs).
		duplicate_card(deck, 4, cards[50]);

		// Shuffle the deck.
		std::shuffle(deck.begin(), deck.end(), std::mt19937(seed));

		return deck;
	}

	const card_base* card_repository::decode_card_ID(const uint8_t card_ID) const {
		if (card_ID == 0 || card_ID >= cards.size())
			return nullptr;
		else
			return cards[card_ID];
	}
}