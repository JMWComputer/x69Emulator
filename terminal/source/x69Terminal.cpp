#include "x69Terminal.h"

#define Uses_TKeys
#define Uses_TApplication
#define Uses_TEvent
#define Uses_TRect
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TButton
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop

#include <tvision/tv.h>

#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

const int cmOpenRegisters = 100;
const int cmOpenTerminal = 101;

static inline short winNumber = 0;

struct RegisterValues
{
	uint8_t regs[16]{};
	uint16_t pc = 0;
	uint16_t lr = 0;
	uint16_t sp = 0;
	uint16_t addr = 0;
};
static inline RegisterValues register_vals{};

struct RegisterPrintout : public TView
{
private:
	static inline RegisterPrintout* active_ = nullptr;

public:
	static bool has_active() noexcept { return active_ != nullptr; };
	static auto get_active() noexcept { return active_; };

	RegisterPrintout(TRect _bounds) :
		TView{ _bounds }
	{
		this->growMode = gfGrowHiX | gfGrowHiY;
		this->options = options | ofFramed;

		active_ = this;

	};
	virtual void draw();

	~RegisterPrintout()
	{
		if (active_ == this)
		{
			active_ = nullptr;
		};
	};
};

void RegisterPrintout::draw()
{
	auto _color = getColor(0x0301);
	TView::draw();
	TDrawBuffer _b;
	std::string _printLine{};
	int _rcounter = 0;
	for (auto& _regVal : register_vals.regs)
	{
		_printLine = "r" + std::to_string(_rcounter++) + " : " + std::to_string(_regVal);
		_b.moveStr(0, _printLine, _color);
		writeLine(2, _rcounter, _printLine.size(), 1, _b);
	};

	_printLine = "pc : " + std::to_string(register_vals.pc);
	_b.moveStr(0, _printLine, _color);
	writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

	_printLine = "lr : " + std::to_string(register_vals.lr);
	_b.moveStr(0, _printLine, _color);
	writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

	_printLine = "sp : " + std::to_string(register_vals.sp);
	_b.moveStr(0, _printLine, _color);
	writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

	_printLine = "addr : " + std::to_string(register_vals.addr);
	_b.moveStr(0, _printLine, _color);
	writeLine(2, ++_rcounter, _printLine.size(), 1, _b);

};

struct RegistersWindow : public TWindow
{
	RegistersWindow(const TRect& _r, const char* _title, short _num) :
		TWindow{ _r, _title, _num },
		TWindowInit{ &RegistersWindow::initFrame }
	{
		TRect _clipr = getClipRect();
		_clipr.grow(-1, -1);
		this->insert(new RegisterPrintout{ _clipr }); 
	};
};



struct TerminalValues
{
	uint8_t width = 0;
	uint8_t height = 0;
	std::vector<char> data{};
};

static inline TerminalValues terminal_vals{};



struct TerminalPrintout : public TView
{
private:
	static inline TerminalPrintout* active_terminal_ = nullptr;

public:

	static bool has_active() noexcept { return active_terminal_ != nullptr; };
	static auto get_active() noexcept { return active_terminal_; };

	TerminalPrintout(TRect _bounds) :
		TView{ _bounds }
	{
		this->growMode = gfGrowHiX | gfGrowHiY;
		this->options = options | ofFramed;
		active_terminal_ = this;
	};

	virtual void draw()
	{
		auto _color = getColor(0x0301);
		TView::draw();
		TDrawBuffer _b;
		std::string _printLine{};
		int _y = 0;
		for (auto it = terminal_vals.data.begin(); it < terminal_vals.data.end(); it += terminal_vals.width)
		{
			_printLine.assign(it, it + terminal_vals.width);
			_b.moveStr(0, _printLine, _color);
			writeLine(0, _y++, _printLine.size(), 1, _b);
		};
	};

	~TerminalPrintout()
	{
		if (active_terminal_ == this)
			active_terminal_ = nullptr;
	};
};

struct TerminalWindow : public TWindow
{
	TerminalWindow(const TRect& _r, const char* _title, short _num) :
		TWindow{ _r, _title, _num },
		TWindowInit{ &RegistersWindow::initFrame }
	{
		TRect _clipr = getClipRect();
		_clipr.grow(-1, -1);
		this->insert(new TerminalPrintout{ _clipr });
	};
};

class THelloApp : public TApplication
{
public:

	THelloApp();

	virtual void handleEvent(TEvent& event);
	static TMenuBar* initMenuBar(TRect);
	static TStatusLine* initStatusLine(TRect);

	void idle() override;

private:

	void open_registers_window();
	void open_terminal_window();

};



void THelloApp::idle()
{
	if (TerminalPrintout::has_active())
	{
		TerminalPrintout::get_active()->draw();
	};
	if (RegisterPrintout::has_active())
	{
		RegisterPrintout::get_active()->draw();
	};
};

THelloApp::THelloApp() :
	TProgInit(&THelloApp::initStatusLine,
		&THelloApp::initMenuBar,
		&THelloApp::initDeskTop
	)
{
}

void THelloApp::open_registers_window()
{
	TRect _r{ 0, 0, 26, 7 };
	auto* _window = new RegistersWindow{ _r, "Registers", ++winNumber };
	this->deskTop->insert(_window);
};

void THelloApp::open_terminal_window()
{
	TRect _r{ 0, 0, 26, 7 };
	auto* _window = new TerminalWindow{ _r, "Terminal", ++winNumber };
	this->deskTop->insert(_window);
};



void THelloApp::handleEvent(TEvent& _event)
{
	TApplication::handleEvent(_event);
	if (_event.what == evCommand)
	{
		switch (_event.message.command)
		{
		case cmOpenRegisters:
			open_registers_window();
			break;
		case cmOpenTerminal:
			open_terminal_window();
			break;
		default:
			break;
		}
		clearEvent(_event);
	}
}

TMenuBar* THelloApp::initMenuBar(TRect r)
{

	r.b.y = r.a.y + 1;

	return new TMenuBar(r, 
		*new TSubMenu("View", kbAltH) +
		*new TMenuItem("Terminal", cmOpenTerminal, kbAltG) +
		*new TMenuItem("Registers", cmOpenRegisters, kbAltG) +
		newLine() +
		*new TMenuItem("Exit", cmQuit, cmQuit, hcNoContext, "Alt-X")
	);

}

TStatusLine* THelloApp::initStatusLine(TRect r)
{
	r.a.y = r.b.y - 1;
	return new TStatusLine(r,
		*new TStatusDef(0, 0xFFFF) +
		*new TStatusItem("~Alt-X~ Exit", kbAltX, cmQuit) +
		*new TStatusItem(0, kbF10, cmMenu)
	);
}

namespace x69::emu::peripheral::terminal
{

	namespace
	{
		void terminal_thread()
		{
			THelloApp helloWorld;
			helloWorld.run();
		};

		std::thread tthread_;
	};

	void open_terminal(uint8_t _width, uint8_t _height)
	{
		terminal_vals.width = _width;
		terminal_vals.height = _height;
		terminal_vals.data.resize((size_t)_width * (size_t)_height);
		tthread_ = std::thread{ &terminal_thread };
	};
	void close_terminal()
	{
		tthread_.join();
	};

	void set_character(char _c, uint8_t _x, uint8_t _y)
	{
		terminal_vals.data.at(((size_t)_x * (size_t)terminal_vals.width) + (size_t)_y) = _c;
	};
	char get_character(uint8_t _x, uint8_t _y)
	{
		return terminal_vals.data.at(((size_t)_x * (size_t)terminal_vals.width) + (size_t)_y);
	};






	void update_register_value(uint8_t _reg, uint8_t _val)
	{
		register_vals.regs[_reg] = _val;
	};
	void update_special_register(SPECIAL_REG _reg, uint16_t _val)
	{
		switch (_reg)
		{
		case SPECIAL_REG::PC:
			register_vals.pc = _val;
			break;
		case SPECIAL_REG::LR:
			register_vals.lr = _val;
			break;
		case SPECIAL_REG::SP:
			register_vals.sp = _val;
			break;
		case SPECIAL_REG::ADDR:
			register_vals.addr = _val;
			break;
		default:
			abort();
		};
	};



};
