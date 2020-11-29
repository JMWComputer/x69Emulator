#include "register_view.h"

#include <sstream>

namespace x69::emu::tv
{
	RegisterValues register_vals{};

	RegisterPrintout::RegisterPrintout(TRect _bounds) :
		TView{ _bounds }
	{
		this->growMode = gfGrowHiX | gfGrowHiY;
		this->options = options | ofFramed;

		active_ = this;

	};
	RegisterPrintout::~RegisterPrintout()
	{
		if (active_ == this)
		{
			active_ = nullptr;
		};
	};

	void RegisterPrintout::draw()
	{
		auto _color = getColor(0x0301);
		TView::draw();
		TDrawBuffer _b;
		std::string _printLine{};
		int _rcounter = 0;

		std::stringstream _sstr{};

		for (auto& _regVal : register_vals.regs)
		{
			_sstr.str("");
			_sstr << "r" << std::dec << _rcounter << " : " << std::hex << (int)_regVal;
			_printLine = _sstr.str();
			_b.moveStr(0, _printLine, _color);
			writeLine(2, _rcounter++, _printLine.size(), 1, _b);

		};

		_sstr.str("");
		_sstr << "pc : " << std::hex << register_vals.pc;
		_printLine = _sstr.str();
		_b.moveStr(0, _printLine, _color);
		writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

		_sstr.str("");
		_sstr << "lr : " << std::hex << register_vals.lr;
		_printLine = _sstr.str();
		_b.moveStr(0, _printLine, _color);
		writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

		_sstr.str("");
		_sstr << "sp : " << std::hex << register_vals.sp;
		_printLine = _sstr.str();
		_b.moveStr(0, _printLine, _color);
		writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

		_sstr.str("");
		_sstr << "adr : " << std::hex << register_vals.addr;
		_printLine = _sstr.str();
		_b.moveStr(0, _printLine, _color);
		writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

	};



	RegistersWindow::RegistersWindow(const TRect& _r, const char* _title, short _num) :
		TWindow{ _r, _title, _num },
		TWindowInit{ &RegistersWindow::initFrame }
	{
		TRect _clipr = getClipRect();
		_clipr.grow(-1, -1);
		this->insert(new RegisterPrintout{ _clipr });
	};

}