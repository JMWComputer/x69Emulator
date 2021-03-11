#include "CPU.h"

namespace x69::emu
{
	void CPU::step()
	{
		auto _ins = this->fetch_next_instruction();
		this->process_instruction(_ins);

		Memory::address_type _pcInc = 2;
		if (_ins.ebyte)
		{
			++_pcInc;
		};

		if (!this->pc_lock_)
		{
			this->special_regs()[SpecialRegisters::PC] += _pcInc;
		}
		else
		{
			this->pc_lock_ = false;
		};

	};

};
