#include "x69Emulator.h"

#include "Emulator.h"

namespace x69::emu
{
	struct Emulator
	{
	public:



		Emulator() : 
			mem_{}, cpu_{ &mem_ }
		{};

	private:
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


};

