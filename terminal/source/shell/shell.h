#pragma once

#include "x69Terminal.h"

#define Uses_TTerminal
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TWindow
#define Uses_TInputLine
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TPalette

#include "Colors.h"

#include <tvision/tv.h>

namespace x69::emu::tv
{

	class ShellView : public TTerminal
	{
	public:

		ShellView(TRect _r);
	
	};

	class ShellInputLine : public TInputLine
	{
	public:
		ShellInputLine(TRect _r);
	};

	class ShellWindow : public TWindow
	{
	public:
		void handleEvent(TEvent& _event) override;
		void draw() override;

		ShellWindow(TRect _r, short _num, Terminal* _terminal);

	protected:
		std::string get_input_text();
		void set_input_text(const std::string& _str);
		void clear_input_text();

		void append_shell(const std::string& _str);
		
	private:
		void write_to_emulator(const std::string& _str);

	private:
		Terminal* terminal_ = nullptr;
		ShellView* sview_ = nullptr;
		ShellInputLine* input_ = nullptr;

	};

};
