#include "Emulator.h"

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
		return (this->allow_vmemop(_addr)) ? this->read(_addr + this->vmem_bottom_) : 0x00;
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

	};

}
