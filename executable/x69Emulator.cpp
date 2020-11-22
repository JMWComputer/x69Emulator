#include <x69EmulatorLib.h>

#include <iostream>
#include <array>
#include <cstdint>
#include <optional>
#include <cassert>
#include <numeric>
#include <vector>
#include <fstream>
#include <filesystem>
#include <string>
#include <thread>
#include <chrono>

namespace em = x69::emu;

em::Memory direct_memory{};

void load_periphs(em::Memory* _mem)
{
	std::unique_ptr<em::TerminalPeriphal> _tmnl{ new em::TerminalPeriphal{} };
	_mem->register_peripheral(std::move(_tmnl));
};

int imain(int argc, char* argv[], char* envp[])
{
	std::cout << "x69 Emulator v0.0.1\n";

	std::vector<std::string> _args{};
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << '\n';
		_args.push_back(argv[i]);
	};

	std::filesystem::path _bootFile{ SOURCE_ROOT "executable/out.o" };

	std::cout << "arg count = " << _args.size() << '\n';

	for (auto it = _args.begin(); it != _args.end(); ++it)
	{
		const auto& arg = *it;
		if (arg == "-help")
		{
			std::cout << "Usage:\n [-boot <path to boot code>](default to boot.o)\n";
		}
		else if (arg.front() == '-')
		{
			const auto& _opt = arg;
			auto _param = it;

			if (it < _args.end() - 1)
			{
				_param = it + 1;
				if (_opt == "-boot")
				{
					_bootFile = std::filesystem::relative(*_param);
					std::cout << "Set build file to : \n";
				}
				else
				{
					std::cout << "Bad Arguement : " << _opt << '\n';
					abort();
				};
			};

		};
	};

	em::CPU _cpu{ &direct_memory };

	load_periphs(&direct_memory);



	auto _codeData = em::load_x69_machine_code(_bootFile);
	std::copy(_codeData.begin(), _codeData.end(), direct_memory.begin());

	while(true)
	{
		_cpu.step();
	};

	std::cout << _cpu.registers() << '\n';
	std::cout << _cpu.special_regs() << '\n';


	return 0;
}

int main(int argc, char* argv[], char* envp[])
{
	auto _res = imain(argc, argv, envp);
	return _res;
};
