#pragma once
#ifndef x69_EMULATOR_LIB_H
#define x69_EMULATOR_LIB_H

#ifdef x69_EMULATOR
#error "double definition"
#else 
#define x69_EMULATOR
#endif

namespace x69::emu
{

	struct Emulator;

	Emulator* new_emulator();
	void destroy_emulator(Emulator* _emulator);
	






};

#endif