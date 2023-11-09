#pragma once

#include <cstdint>

namespace networked_UNO {
	enum CARD_COLOR {
		WILD = 0,
		RED = 1,
		GREEN = 2,
		BLUE = 3,
		YELLOW = 4
	};

	struct card_base {
		const uint8_t card_ID;
		const CARD_COLOR card_color;

		virtual ~card_base() = default;

	protected:
		card_base(const uint8_t card_ID, const CARD_COLOR card_color)
			: card_ID(card_ID), card_color(card_color) {}
	};

	struct numbered_card : public card_base {
		const uint8_t number;

		numbered_card(const uint8_t card_ID, const uint8_t number, const CARD_COLOR card_color)
			: number(number), card_base(card_ID, card_color) {}
	};

	struct draw_card : public card_base {
		const uint8_t num_cards_to_draw;

		draw_card(const uint8_t card_ID, const uint8_t num_cards_to_draw, const CARD_COLOR card_color)
			: num_cards_to_draw(num_cards_to_draw), card_base(card_ID, card_color) {}
	};

	struct skip_card : public card_base {
		skip_card(const uint8_t card_ID, const CARD_COLOR card_color)
			: card_base(card_ID, card_color) {}
	};

	struct change_card : public card_base {
		change_card(const uint8_t card_ID)
			: card_base(card_ID, CARD_COLOR::WILD) {}
	};
}