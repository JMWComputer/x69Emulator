#include <x69Emulator.h>

namespace emu = x69::emu;

#include <thread>
#include <chrono>
#include <iostream>

int main()
{
	auto _emu = emu::new_emulator();

	std::filesystem::path _fibbo{ SOURCE_ROOT "/test/fibbo.o" };
	
	
	auto _result = emu::load_program(_emu, _fibbo);
	if (!_result)
	{
		std::terminate();
	};

	std::string _foo{};

	while (true)
	{
		emu::step(_emu);
		auto _regs = emu::get_all_cpu_registers(_emu);
		for (auto& r : _regs)
		{
			std::cout << (int)r << '\n';
		};
		std::cout << "PC : " << emu::get_special_register(_emu, emu::rPC ) << '\n';
		//std::cin >> _foo;
		std::this_thread::sleep_for(std::chrono::milliseconds{ 250 });
		std::cout << "\x1B[2J\x1B[H";
		
	};

	emu::destroy_emulator(_emu);

	return 0;
};