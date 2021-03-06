#include "Emulator.h"

#include <fstream>

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

}
