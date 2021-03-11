#pragma once

#include "Register/Register.h"

#include <numeric>
#include <cstdint>

namespace x69::emu
{
	template <uint8_t GeneralRegisters, typename WordType, typename DirectMemAddrType>
	struct CPUTraits
	{
		using word_type = WordType;
		using register_type = Register<typename word_type>;

		using direct_address_type = DirectMemAddrType;

		constexpr static inline uint8_t word_size() { return sizeof(WordType); };
		constexpr static inline uint8_t general_registers() { return GeneralRegisters; };
		constexpr static inline size_t  direct_memory_size() { return std::numeric_limits<DirectMemAddrType>::max(); };

	};

	using CPUVersionTraits = CPUTraits<16, uint8_t, uint16_t>;

};
