#include "terminal_view.h"

namespace x69::emu::tv
{
	TerminalValues TERMINAL_VALUES{};

	TerminalPrintout::TerminalPrintout(TRect _bounds) :
		TView{ _bounds }
	{
		this->growMode = gfGrowHiX | gfGrowHiY;
		this->options = options | ofFramed;
	};

	void TerminalPrintout::draw()
	{
		auto _color = getColor(0x0301);
		TView::draw();
		TDrawBuffer _b;
		std::string _printLine{};
		int _y = 0;
		for (auto it = TERMINAL_VALUES.begin(); it < TERMINAL_VALUES.end(); it += TERMINAL_VALUES.width())
		{
			_printLine.assign(it, it + TERMINAL_VALUES.width());
			_b.moveStr(0, _printLine, _color);
			writeLine(0, _y++, _printLine.size(), 1, _b);
		};
	};


	TerminalWindow::TerminalWindow(const TRect& _r, const char* _title, short _num) :
		TWindow{ _r, _title, _num },
		TWindowInit{ &TerminalWindow::initFrame }
	{
		TRect _clipr = getClipRect();
		_clipr.grow(-1, -1);
		this->insert(new x69::emu::tv::TerminalPrintout{ _clipr });
	};

	std::pair<int, int> TerminalWindow::get_default_size()
	{
		return { 64, 48 };
	};


}
