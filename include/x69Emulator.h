#pragma once
#ifndef x69_EMULATOR_LIB_H
#define x69_EMULATOR_LIB_H

#ifdef x69_EMULATOR
#error "double definition"
#else 
#define x69_EMULATOR
#endif

#include <array>
#include <cstdint>
#include <istream>
#include <ostream>
#include <filesystem>

namespace x69::emu
{

	struct Emulator;

	Emulator* new_emulator();
	void destroy_emulator(Emulator* _emulator);
	
	enum CPU_REGISTERS_E
	{
		r0,
		r1,
		r2,
		r3,
		r4,
		r5,
		r6,
		r7,
		r8,
		r9,
		r10,
		r11,
		r12,
		r13,
		r14,
		r15
	};

	enum SPECIAL_REGISTERS_E
	{
		rPC = 0x0,
		rLR = 0x1,
		rSP = 0x2,
		rADDR = 0x3
	};

	using Word = uint8_t;

	Word get_cpu_register(Emulator* _emu, CPU_REGISTERS_E _reg);
	std::array<Word, 16> get_all_cpu_registers(Emulator* _emu);

	using DWord = uint16_t;

	DWord get_special_register(Emulator* _emu, SPECIAL_REGISTERS_E _reg);
	std::array<DWord, 4> get_all_special_registers(Emulator* _emu);

	bool load_program(Emulator* _emu, const uint8_t* _pBegin, const size_t _pSize);
	bool load_program(Emulator* _emu, std::istream& _istr);
	bool load_program(Emulator* _emu, const std::filesystem::path& _path);

	void step(Emulator* _emu);
	

};

#endif