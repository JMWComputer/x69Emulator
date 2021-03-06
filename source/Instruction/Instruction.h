#pragma once

#include "Spec/Spec.h"

#include <optional>





namespace x69::emu
{
	struct Instruction
	{
	public:
		using cpu_traits = CPUVersionTraits;
		using word_type = typename cpu_traits::word_type;

		word_type ibyte; // first instruction byte, I call it the instruction byte
		word_type dbyte; // second instruction byte, I call it the data byte
		std::optional<word_type> ebyte; // optional third instruction byte, I call it the extra byte

		constexpr Instruction() noexcept :
			ibyte{ 0x00 }, dbyte{ 0x00 }, ebyte{ std::nullopt }
		{};
		constexpr Instruction(word_type _ibyte, word_type _dbyte, word_type _ebyte) noexcept :
			ibyte{ _ibyte }, dbyte{ _dbyte }, ebyte{ _ebyte }
		{};
		constexpr Instruction(word_type _ibyte, word_type _dbyte) noexcept :
			ibyte{ _ibyte }, dbyte{ _dbyte }, ebyte{ std::nullopt }
		{};

	};

};
