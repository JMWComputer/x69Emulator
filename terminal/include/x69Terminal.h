#pragma once

#include <cstdint>

namespace x69::emu::peripheral::terminal
{
	void open_terminal(uint8_t _width, uint8_t _height);
	void close_terminal();

	void set_character(char _c, uint8_t _x, uint8_t _y);
	char get_character(uint8_t _x, uint8_t _y);




	enum class SPECIAL_REG
	{
		PC,
		LR,
		SP,
		ADDR
	};

	void update_register_value(uint8_t _reg, uint8_t _val);
	void update_special_register(SPECIAL_REG _reg, uint16_t _val);

}