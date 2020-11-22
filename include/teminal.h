#pragma once
#ifndef X69_IMPL_TERMINAL_H
#define X69_IMPL_TERMINAL_H

#include <stdint.h>

extern "C"
{
	typedef uint16_t address_type;
	typedef uint8_t word_type;

	void set_character(word_type _x, word_type _y, char _c);
	char get_character(word_type _x, word_type _y);

	word_type get_terminal_width();
	word_type get_terminal_height();

	void clear_terminal();

	bool open_terminal(word_type _x, word_type _y);
	void close_terminal();

};

#endif