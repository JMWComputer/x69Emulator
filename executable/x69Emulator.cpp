#include <x69EmulatorLib.h>

#include <iostream>

namespace em = x69::emu;

int main(int argc, char* argv[], char* envp[])
{	
	auto _emu = em::new_emulator();
	em::destroy_emulator(_emu);
	return 0;
};
