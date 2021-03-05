#include "x69Emulator.h"

#include "Emulator.h"

namespace x69::emu
{
	struct Emulator
	{
		Emulator() : 
			mem_{}, cpu_{ &mem_ }
		{};

		Memory mem_;
		CPU cpu_;
	};

	Emulator* new_emulator()
	{
		return new Emulator{};
	};
	void destroy_emulator(Emulator* _emulator)
	{
		delete _emulator;
	};

	Word get_cpu_register(Emulator* _emu, CPU_REGISTERS_E _reg)
	{
		return _emu->cpu_.registers().at((uint8_t)_reg).val_;
	};
	std::array<Word, 16> get_all_cpu_registers(Emulator* _emu)
	{
		std::array<Word, 16> _out{};
		auto& _regs = _emu->cpu_.registers();
		int _rc = 0;
		for (auto& r : _regs)
		{
			_out[_rc++] = r;
		};
		return _out;
	};

	Word get_special_register(Emulator* _emu, CPU_REGISTERS_E _reg)
	{
		return _emu->cpu_.special_regs().at((emu::SpecialRegisters::REGISTERS)_reg);
	};
	std::array<Word, 4> get_all_special_registers(Emulator* _emu)
	{
		std::array<Word, 4> _out{};
		auto& _regs = _emu->cpu_.special_regs();
		int _rc = 0;
		for (auto& r : _regs)
		{
			_out[_rc++] = r;
		};
		return _out;
	};

	bool load_program(Emulator* _emu, const uint8_t* _pBegin, const size_t _pSize)
	{
		auto& _mem = _emu->mem_;
		std::copy_n(_pBegin, _pSize, _mem.begin());
		return true;
	};
	bool load_program(Emulator* _emu, std::istream& _istr)
	{
		constexpr size_t READ_BUFFER_SIZE = 1024;

		std::vector<uint8_t> _bytes{};
		uint8_t _rbuff[READ_BUFFER_SIZE]{ 0 };
		std::fill_n(_rbuff, READ_BUFFER_SIZE, 0);

		while (!_istr.eof())
		{
			_istr.read((char*)_rbuff, READ_BUFFER_SIZE);
			_bytes.insert(_bytes.end(), _rbuff, _rbuff + _istr.gcount());
		};

		return load_program(_emu, _bytes.data(), _bytes.size());
	};
	bool load_program(Emulator* _emu, const std::filesystem::path& _path)
	{
		auto _mcode = emu::load_x69_machine_code(std::filesystem::absolute(_path));
		if (_mcode)
		{
			std::copy(_mcode->begin(), _mcode->end(), _emu->mem_.begin());
		};
		return _mcode.has_value();
	};

	void step(Emulator* _emu)
	{
		_emu->cpu_.step();
	};

	
};

