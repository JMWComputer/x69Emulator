#include "x69EmulatorLib.h"

#include <x69Terminal.h>


namespace x69::emu
{

	std::optional<PeripheralLayout> parse_peripherals_file(const std::filesystem::path& _path)
	{
		namespace fs = std::filesystem;
		if (!fs::exists(_path))
		{
			return std::nullopt;
		};

		std::ifstream _ifstr{ _path };
		if (!_ifstr.is_open())
		{
			return std::nullopt;
		};

		PeripheralLayout _out{};

		std::string _libPath{};
		std::string _address{};

		while (!_ifstr.eof())
		{
			std::getline(_ifstr, _libPath, ':');

			if (_ifstr.eof())
			{
				break;
			};

			while (std::isspace(_ifstr.peek()))
			{
				_ifstr.ignore();
			};

			std::getline(_ifstr, _address, '\n');

			_out.peripherals.push_back(PeripheralLayout::periph{ _libPath, (uint16_t)std::stoi(_address, 0, 16) });

			_libPath.clear();
			_address.clear();

		};

		return _out;
	};

}



namespace x69::emu
{

	void Memory::set_vmem_range(std::pair<address_type, address_type> _range)
	{
		this->vmem_bottom_ = _range.first;
		this->vmem_top_ = _range.second;
	};

	bool Memory::allow_vmemop(address_type _addr) const
	{
		return (_addr + this->vmem_bottom_ >= this->vmem_bottom_ && _addr + this->vmem_bottom_ < this->vmem_top_);
	};

	Memory::word_type Memory::vread(address_type _addr)
	{
		return (this->allow_vmemop(_addr))? this->read(_addr + this->vmem_bottom_) : 0x00;
	};

	void Memory::vwrite(address_type _addr, word_type _val)
	{
		if (this->allow_vmemop(_addr))
		{
			this->write(_addr + this->vmem_bottom_, _val);
		};
	};
	
	std::pair<Memory::address_type, Memory::address_type> Memory::get_vmem_range() const noexcept
	{
		return { this->vmem_bottom_, this->vmem_top_ };
	};

}


namespace x69::emu
{
	std::optional<std::vector<uint8_t>> load_x69_machine_code(const std::filesystem::path& _path)
	{
		if (!std::filesystem::exists(_path))
		{
			return std::nullopt;
		};
		
		if (_path.is_relative())
		{
			std::filesystem::relative(_path);
		};

		std::ifstream _ifstr{ _path, std::ifstream::binary };
		if (!_ifstr.is_open())
		{
			return std::nullopt;
		};

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
		else if ((_val & (uint8_t)CONTROL_BITS::CLEAR) != 0)
		{
			peripheral::terminal::clear_terminal();
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
		peripheral::terminal::open_terminal(_width, _height);
	};
	void TerminalPeriphal::close()
	{
		peripheral::terminal::close_terminal();
	};

};

#pragma endregion TERMINAL_PERIPHERAL

