#pragma once

#include "CPU/CPU.h"

namespace x69::emu
{
	
	std::optional<std::vector<uint8_t>> load_x69_machine_code(const std::filesystem::path& _path);

}
