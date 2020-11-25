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
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TEditWindow
#define Uses_TScrollBar
#define Uses_TFileEditor
#define Uses_TIndicator
#define Uses_TWindow
#define Uses_TEvent
#define Uses_TRect
#define Uses_TPalette

#include <tvision/tv.h>

#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

#define EMULATOR_PALLETE_COLOR "\x9\xA\xB\xC\xD\xE"



const int cmOpenRegisters = 100;
const int cmOpenTerminal = 101;
const int cmOpenCommandLine = 102;

const int cmSaveAndRun = 103;

static inline short winNumber = 0;

using EMUTerminal = x69::emu::Terminal;




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


struct CommandLineWindow : TWindow
{
	struct CommandLineData
	{
		char inputLineData[128]{};
	};

	CommandLineWindow(const TRect& _r, const char* _title, short _num, EMUTerminal* _terminal) :
		TWindow{ _r, _title, _num },
		TWindowInit{ &CommandLineWindow::initFrame },
		terminal_{ _terminal }
	{
		
		TRect _thisRect = getClipRect();

		unsigned _inLineWidth = (unsigned)((_thisRect.b.x - 2) - (_thisRect.a.x + 2));
		TRect _inLineRect{ _thisRect.a.x + 2, _thisRect.b.y - 2, _thisRect.b.x - 2, _thisRect.b.y - 1 };

		TRect _inLineLabelRect = _inLineRect;
		_inLineLabelRect.b.x = _inLineLabelRect.a.x + 3;
		_inLineRect.a.x += 3;

		this->input_line_ = new TInputLine{ _inLineRect, _inLineWidth };
		this->insert(this->input_line_);

		this->insert(new TLabel(_inLineLabelRect, "$: ", this->input_line_));

		this->input_line_->setState(sfSelected, false);

	};

	void handleEvent(TEvent& _event) override;

	void disable_input_line();
	void enable_input_line();

	void clear_user_input();

private:
	void got_user_input();

	EMUTerminal* terminal_ = nullptr;
	TInputLine* input_line_ = nullptr;

	bool allow_input_ = false;

};

void CommandLineWindow::disable_input_line()
{
	this->allow_input_ = false;
	this->clear_user_input();
	this->input_line_->hideCursor();
	this->input_line_->setState(sfSelected, false);
	this->input_line_->draw();
};
void CommandLineWindow::enable_input_line()
{
	this->allow_input_ = true;
	this->input_line_->showCursor();
	this->input_line_->setState(sfSelected, true);
	this->input_line_->draw();
};

void CommandLineWindow::handleEvent(TEvent& _event)
{

	TWindow::handleEvent(_event);

	if (_event.what == evKeyDown)
	{
		switch (_event.keyDown.keyCode)
		{
		case kbEnter:
			this->got_user_input();
			break;
		case kbEsc:
			this->disable_input_line();
			break;
		case kbAltRight:
			this->terminal_->push_message(EMUTerminal::Message::Step{});
			break;
		default:
			break;
		};
		clearEvent(_event);
	}
	else if (_event.what == evMouseDown)
	{
		MouseEventType _s;
		switch (_event.mouse.buttons)
		{
		case mbLeftButton:
			this->enable_input_line();
			break;
		default:
			break;
		};
		clearEvent(_event);
	};

}

void CommandLineWindow::clear_user_input()
{
	this->input_line_->curPos = 0;
	std::memset(this->input_line_->data, 0, this->input_line_->dataSize());
};

void CommandLineWindow::got_user_input()
{
	auto _str = std::string{ this->input_line_->data };
	this->terminal_->push_message(EMUTerminal::Message::GotInput{ _str });
	this->clear_user_input();
	this->input_line_->draw();
};





class THelloApp : public TApplication
{
public:

	THelloApp(EMUTerminal* _terminal);

	virtual void handleEvent(TEvent& event);
	static TMenuBar* initMenuBar(TRect);
	static TStatusLine* initStatusLine(TRect);

	static inline std::optional<std::string> EDIT_FILE_PATH{ std::nullopt };

	void idle() override;
	void run() override;

	TPalette& getPalette()const override
	{
		static TPalette _plt{ EMULATOR_PALLETE_COLOR, sizeof(EMULATOR_PALLETE_COLOR) - 1 };
		return _plt;
	};

	~THelloApp();

private:
	EMUTerminal* terminal_;

	CommandLineWindow* cmd_line_ = nullptr;



	void open_registers_window();
	void open_terminal_window();
	void open_command_line_window();
	void open_file_editor();

};

void THelloApp::run()
{
	this->open_command_line_window();
	TApplication::run();
};

THelloApp::THelloApp(EMUTerminal* _terminal) :
	TProgInit{&THelloApp::initStatusLine, &THelloApp::initMenuBar, &THelloApp::initDeskTop},
	terminal_{ _terminal }
{

};
THelloApp::~THelloApp()
{
	this->terminal_->push_message(EMUTerminal::Message::Exited{});
};


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
void THelloApp::open_command_line_window()
{
	auto _thisRect = this->getBounds();
	TRect _r = _thisRect;
	_r.grow(-1, -1);
	
	_r.a.x = 0;
	_r.a.y = _thisRect.b.y - 3;

	_r.b.x = _thisRect.b.x;
	_r.b.y = _thisRect.b.y;

	auto* _window = new CommandLineWindow{ _r, "CommandLine", ++winNumber, this->terminal_ };
	this->deskTop->insert(_window);
	this->cmd_line_ = _window;

};

void THelloApp::open_file_editor()
{
	this->cmd_line_->disable_input_line();
	this->cmd_line_->setState(sfSelected, false);

	auto _r = this->getClipRect();
	TRect _editWindowRect{ 0, 0, _r.b.x - 16, 32 };
	auto* _window = new AssemblyEditorWindow{ _editWindowRect, EDIT_FILE_PATH->c_str(), ++winNumber, this->terminal_ };
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
		case cmOpenCommandLine:
			open_command_line_window();
			break;
		default:
			break;
		}
		clearEvent(_event);
	};
}

TMenuBar* THelloApp::initMenuBar(TRect r)
{

	r.b.y = r.a.y + 1;

	return new TMenuBar(r, 
		*new TSubMenu("View", kbAltH) +
		*new TMenuItem("Terminal", cmOpenTerminal, kbAltT) +
		*new TMenuItem("Registers", cmOpenRegisters, kbAltR) +
		*new TMenuItem("CommandLine", cmOpenCommandLine, kbAltC) +
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



void THelloApp::idle()
{
	TApplication::idle();

	if (!this->terminal_->is_open())
	{
		this->shutDown();
	};

	if (RegisterPrintout::has_active())
	{
		RegisterPrintout::get_active()->draw();
	};
	if (TerminalPrintout::has_active())
	{
		TerminalPrintout::get_active()->draw();
	};

	if (this->EDIT_FILE_PATH)
	{
		this->open_file_editor();
		this->EDIT_FILE_PATH = std::nullopt;
	};

};

namespace x69::emu::peripheral::terminal
{
	void open_terminal(uint8_t _width, uint8_t _height)
	{
		terminal_vals.width = _width;
		terminal_vals.height = _height;
		terminal_vals.data.resize((size_t)_width * (size_t)_height);
		//tthread_ = std::thread{ &terminal_thread };
	};
	void close_terminal()
	{
		//tthread_.join();
	};

	void set_character(char _c, uint8_t _x, uint8_t _y)
	{
		terminal_vals.data.at(((size_t)_y * (size_t)terminal_vals.width) + (size_t)_x) = _c;
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
	
	
	
	void clear_terminal()
	{
		std::memset(terminal_vals.data.data(), 0, terminal_vals.data.size() * sizeof(typename decltype(terminal_vals.data)::value_type));
	};

};

namespace x69::emu
{

	int Terminal::thread_main()
	{
		// Initialize 

		int _result = 0;

		this->set_open_flag(true);

		THelloApp helloWorld{ this };

		// Main

		helloWorld.run();
		helloWorld.shutDown();


		// Shutdown

		this->res_ = _result;
		return _result;
	};



	void Terminal::edit_file(const std::filesystem::path& _path)
	{
		THelloApp::EDIT_FILE_PATH = _path.string();
	};

	bool Terminal::has_message() const noexcept
	{
		this->msg_queue_mtx_.lock();
		auto _out = !this->msg_queue_.empty();
		this->msg_queue_mtx_.unlock();
		return _out;
	};
	Terminal::Message Terminal::next_message()
	{
		this->msg_queue_mtx_.lock();
		auto _out = std::move(this->msg_queue_.front());
		this->msg_queue_.pop();
		this->msg_queue_mtx_.unlock();
		return _out;
	};
	void Terminal::ignore_message() noexcept
	{
		this->msg_queue_mtx_.lock();
		this->msg_queue_.pop();
		this->msg_queue_mtx_.unlock();
	};
	void Terminal::clear_messages() noexcept
	{
		this->msg_queue_mtx_.lock();
		while (!this->msg_queue_.empty())
		{
			this->msg_queue_.pop();
		};
		this->msg_queue_mtx_.unlock();
	};

	void Terminal::push_message(const Message& _msg)
	{
		this->msg_queue_mtx_.lock();
		this->msg_queue_.push(_msg);
		this->msg_queue_mtx_.unlock();
	};
	void Terminal::push_message(Message&& _msg)
	{
		this->msg_queue_mtx_.lock();
		this->msg_queue_.push(std::move(_msg));
		this->msg_queue_mtx_.unlock();
	};

	bool Terminal::is_open() const noexcept
	{
		return this->is_open_;
	};
	Terminal::operator bool() const noexcept
	{
		return this->is_open();
	};

	void Terminal::set_open_flag(bool _b) noexcept
	{
		this->is_open_ = _b;
	};

	bool Terminal::open(std::chrono::milliseconds _timeoutAfter)
	{
		if (!this->is_open())
		{
			this->t_ = std::thread{ &Terminal::thread_main, this };
			
			auto _startTime = std::chrono::steady_clock::now();
			
			while (!this->is_open())
			{
				if (std::chrono::steady_clock::now() - _startTime > _timeoutAfter)
				{
					break;
				};
				std::this_thread::sleep_for(std::chrono::microseconds{ 10 });
			};
			return this->is_open();

		};
	};

	bool Terminal::open()
	{
		return this->open(Terminal::TIMEOUT_DEFAULT);
	};

	std::optional<int> Terminal::close(std::chrono::milliseconds _timeoutAfter)
	{
		if (this->is_open())
		{
			this->set_open_flag(false);
			auto _startTime = std::chrono::steady_clock::now();
			while (!this->t_.joinable())
			{
				if (std::chrono::steady_clock::now() - _startTime > _timeoutAfter)
				{
					return std::nullopt;
				};
				std::this_thread::sleep_for(std::chrono::microseconds{ 10 });
			};
			this->t_.join();
			return this->res_;
		};
	};
	std::optional<int> Terminal::close()
	{
		return this->close(this->TIMEOUT_DEFAULT);
	};



	Terminal::Terminal(std::streambuf* _einBuf, std::streambuf* _eoutBuf) : 
		ein_{ _einBuf }, eout_{ _eoutBuf }
	{};

	Terminal::~Terminal()
	{
		this->close();
	};


}
