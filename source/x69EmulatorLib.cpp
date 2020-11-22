#include "x69EmulatorLib.h"

#include <x69Terminal.h>

namespace x69::emu
{
	std::vector<uint8_t> load_x69_machine_code(const std::filesystem::path& _path)
	{
		std::cout << "Reading " << _path << '\n';
		assert(std::filesystem::exists(_path));
		if (_path.is_relative())
			std::filesystem::relative(_path);

		std::ifstream _ifstr{ _path, std::ifstream::binary };
		assert(_ifstr.is_open());

		std::vector<uint8_t> _bytes{};
		char _buff[256]{};
		while (!_ifstr.eof())
		{
			_ifstr.read(_buff, sizeof(_buff) / sizeof(char));
			_bytes.insert(_bytes.end(), _buff, _buff + _ifstr.gcount());
		};

		_ifstr.close();

		return _bytes;
	};

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

		uint8_t _rcounter = 0;
		for (auto& r : this->registers())
		{
			peripheral::terminal::update_register_value(_rcounter++, r);
		};

		namespace tm = peripheral::terminal;

		tm::update_special_register(tm::SPECIAL_REG::PC, this->special_regs()[SpecialRegisters::PC]);
		tm::update_special_register(tm::SPECIAL_REG::LR, this->special_regs()[SpecialRegisters::LR]);
		tm::update_special_register(tm::SPECIAL_REG::SP, this->special_regs()[SpecialRegisters::SP]);
		tm::update_special_register(tm::SPECIAL_REG::ADDR, this->special_regs()[SpecialRegisters::ADDR]);


	};

}

#pragma region TERMINAL_PERIPHERAL

namespace x69::emu
{
	void TerminalPeriphal::write_to_control(uint8_t _val)
	{
		if ((_val & (uint8_t)CONTROL_BITS::WRITE) != 0)
		{
			this->set_char(this->x_, this->y_, (char)this->c_);
		}
		else if ((_val & (uint8_t)CONTROL_BITS::READ) != 0)
		{
			this->c_ = this->get_char(this->x_, this->y_);
		}
		else if ((_val & (uint8_t)CONTROL_BITS::CLOSE) != 0)
		{
			this->close();
		}
		else if ((_val & (uint8_t)CONTROL_BITS::OPEN) != 0)
		{
			this->open(this->x_, this->y_);
		}
		else
		{
			// ignore
		};
	};

	void TerminalPeriphal::set_char(uint8_t _x, uint8_t _y, char _c)
	{
		peripheral::terminal::set_character(_c, _x, _y);
	};
	char TerminalPeriphal::get_char(uint8_t _x, uint8_t _y)
	{
		return peripheral::terminal::get_character(_x, _y);
	};

	void TerminalPeriphal::open(uint8_t _width, uint8_t _height)
	{
		if (!this->is_open_)
		{
			peripheral::terminal::open_terminal(_width, _height);
			is_open_ = true;
		};
	};
	void TerminalPeriphal::close()
	{
		if (this->is_open_)
		{
			peripheral::terminal::close_terminal();
			is_open_ = false;
		};
	};
};

#pragma endregion TERMINAL_PERIPHERAL

