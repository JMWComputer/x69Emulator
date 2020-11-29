#pragma once

#define Uses_TView
#define Uses_TRect
#define Uses_TWindow
#define Uses_TDrawBuffer

#include <tvision/tv.h>

namespace x69::emu::tv
{

	struct RegisterValues
	{
		uint8_t regs[16]{};
		uint16_t pc = 0;
		uint16_t lr = 0;
		uint16_t sp = 0;
		uint16_t addr = 0;
	};
	extern RegisterValues register_vals;

	struct RegisterPrintout : public TView
	{
	private:
		static inline RegisterPrintout* active_ = nullptr;

	public:
		static bool has_active() noexcept { return active_ != nullptr; };
		static auto get_active() noexcept { return active_; };

		RegisterPrintout(TRect _bounds);

		virtual void draw();

		~RegisterPrintout();
	};

	struct RegistersWindow : public TWindow
	{
		RegistersWindow(const TRect& _r, const char* _title, short _num);
	};



	

};
