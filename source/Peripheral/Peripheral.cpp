#include "Peripheral.h"

#include <fstream>
#include <sstream>

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

};