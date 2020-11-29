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

#include "Colors.h"

#include "file_editor/file_editor.h"
#include "terminal_view/terminal_view.h"
#include "register_view/register_view.h"
#include "file_view/file_view.h"
#include "shell/shell.h"

#include "RealTime.h"

#include <tvision/tv.h>

#include <chrono>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

namespace etv = x69::emu::tv;

const int cmOpenRegisters = 100;
const int cmOpenTerminal = 101;
const int cmOpenShell = 102;
const int cmOpenAll = 103;
const int cmOpenIDE = 104;

static inline short winNumber = 0;

using EMUTerminal = x69::emu::Terminal;


struct IDEWindow : public TWindow
{
public:

	void edit_file(const std::string& _filePath)
	{
		auto _r = this->getClipRect();
		_r.grow(-1, -1);

		std::filesystem::path _fp{ _filePath };
		if (!_fp.has_extension())
		{
			this->current_dir_.append("./" + _filePath);
			this->file_list_->readDirectory(_filePath.c_str(), "*");
			this->file_list_->draw();
		}
		else
		{

			_r.b.x = this->file_list_->getBounds().a.x - 5;
			this->vscroll_ = new TScrollBar{ { _r.b.x + 1, _r.a.y, _r.b.x + 2, _r.b.y } };		
			this->insert(this->vscroll_);

			this->hscroll_ = new TScrollBar{ { _r.a.x, _r.b.y - 1, _r.b.x + 3, _r.b.y } };
			this->insert(this->hscroll_);
			_r.b.y -= 2;

			this->asm_editor_ = new x69::emu::tv::AssemblyEditor(_r, this->hscroll_, this->vscroll_, nullptr, _filePath.c_str());
			this->insert(this->asm_editor_);

		};

	};

	void handleEvent(TEvent& _event) override
	{
		TPoint _temp{};
		TWindow::handleEvent(_event);
		if (_event.what == evBroadcast)
		{
			char _buff[128]{};
			switch (_event.message.command)
			{
			case cmFileDoubleClicked:
				this->file_list_->getText(_buff, this->file_list_->cursor.y, sizeof(_buff) - 1);
				this->edit_file(_buff);
				clearEvent(_event);
				break;
			default:
				break;
			};
		};

	};

	IDEWindow(const TRect& _r, const char* _fileName, short _anum, const std::string& _currentDir = "./") :
		TWindow{ _r, _fileName, _anum },
		TWindowInit{ &IDEWindow::initFrame }, current_dir_{ _currentDir }
	{

		auto _rec = this->getBounds();
		_rec.a.x = _rec.b.x - 32;
		_rec.a.y += 4;
		_rec.b.y -= 4;
		_rec.b.x -= 1;
		this->file_list_ = new x69::emu::tv::FileViewer{ _rec, _currentDir.c_str() };
		this->file_list_->readDirectory(this->current_dir_.string().c_str(), "*");
		this->insert(this->file_list_);
	};

private:

	std::filesystem::path current_dir_ = "";

	x69::emu::tv::FileViewer* file_list_ = nullptr;

	TScrollBar* hscroll_ = nullptr;
	TScrollBar* vscroll_ = nullptr;
	x69::emu::tv::AssemblyEditor* asm_editor_ = nullptr;


};




class THelloApp : public TApplication
{
public:

	THelloApp(EMUTerminal* _terminal);


	virtual void handleEvent(TEvent& event);
	static TMenuBar* initMenuBar(TRect);
	static TStatusLine* initStatusLine(TRect);

	TPalette& getPalette() const override
	{
		static TPalette _plt{ EMU_PALETTE, sizeof(EMU_PALETTE) - 1 };
		return _plt;
	};

	static inline std::optional<std::string> EDIT_FILE_PATH{ std::nullopt };



	void idle() override;
	void run() override;
	


	~THelloApp();

private:
	EMUTerminal* terminal_;

	etv::RegistersWindow* reg_window_ = nullptr;
	x69::emu::tv::TerminalWindow* terminal_window_ = nullptr;

	void open_registers_window();
	void open_terminal_window();
	void open_file_editor();
	void open_shell();
	void open_ide();

};

void THelloApp::run()
{
	this->open_shell();
	TApplication::run();
};

THelloApp::THelloApp(EMUTerminal* _terminal) :
	TProgInit{&THelloApp::initStatusLine, &THelloApp::initMenuBar, &THelloApp::initDeskTop},
	terminal_{ _terminal }
{};
THelloApp::~THelloApp()
{
	this->terminal_->push_message(EMUTerminal::Message::Exited{});
};

void THelloApp::open_registers_window()
{
	if (this->reg_window_)
	{
		this->remove(this->reg_window_);
	};

	TRect _r{ 0, 0, 26, 7 };
	this->reg_window_ = new etv::RegistersWindow{ _r, "Registers", ++winNumber };
	this->deskTop->insert(this->reg_window_);
	
};
void THelloApp::open_terminal_window()
{
	if (this->terminal_window_)
	{
		this->remove(this->terminal_window_);
	};

	TRect _r{ 0, 0, 26, 7 };
	this->terminal_window_ = new x69::emu::tv::TerminalWindow{ _r, "Terminal", ++winNumber };
	this->terminal_window_->palette = 0;
	this->deskTop->insert(this->terminal_window_);
};

void THelloApp::open_file_editor()
{
	auto _r = this->getClipRect();
	TRect _editWindowRect{ 0, 0, _r.b.x - 16, 32 };
	auto* _window = new x69::emu::tv::AssemblyEditorWindow{ _editWindowRect, EDIT_FILE_PATH->c_str(), ++winNumber, this->terminal_ };
	this->deskTop->insert(_window);

};

void THelloApp::open_shell()
{
	TRect _rect = this->deskTop->getClipRect();
	auto _window = new x69::emu::tv::ShellWindow{ _rect, ++winNumber, this->terminal_ };
	this->deskTop->insert(_window);
};

void THelloApp::open_ide()
{
	TRect _rect = this->deskTop->getClipRect();
	auto _window = new IDEWindow{ _rect, "ide", ++winNumber };
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
		case cmOpenShell:
			this->open_shell();
			break;
		case cmOpenIDE:
			this->open_ide();
			break;
		case cmOpenAll:
			this->open_registers_window();
			this->open_terminal_window();
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
		*new TMenuItem("All", cmOpenAll, kbAltA) +
		*new TMenuItem("Shell", cmOpenShell, kbAltS) +
		*new TMenuItem("IDE", cmOpenIDE, kbAltE) +
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

	if (this->terminal_window_)
	{
		this->terminal_window_->redraw();
	};
	if (this->reg_window_)
	{
		this->reg_window_->redraw();
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
		tv::TERMINAL_VALUES.resize(_width, _height);
		//tthread_ = std::thread{ &terminal_thread };
	};
	void close_terminal()
	{
		//tthread_.join();
	};

	void set_character(char _c, uint8_t _x, uint8_t _y)
	{
		tv::TERMINAL_VALUES.at(_x, _y) = _c;
	};
	char get_character(uint8_t _x, uint8_t _y)
	{
		return tv::TERMINAL_VALUES.at(_x, _y);
	};
	void update_register_value(uint8_t _reg, uint8_t _val)
	{
		etv::register_vals.regs[_reg] = _val;
	};
	void update_special_register(SPECIAL_REG _reg, uint16_t _val)
	{
		switch (_reg)
		{
		case SPECIAL_REG::PC:
			etv::register_vals.pc = _val;
			break;
		case SPECIAL_REG::LR:
			etv::register_vals.lr = _val;
			break;
		case SPECIAL_REG::SP:
			etv::register_vals.sp = _val;
			break;
		case SPECIAL_REG::ADDR:
			etv::register_vals.addr = _val;
			break;
		default:
			abort();
		};
	};
	
	
	
	void clear_terminal()
	{
		std::fill(tv::TERMINAL_VALUES.begin(), tv::TERMINAL_VALUES.end(), 0);
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
		ein{ _einBuf }, eout{ _eoutBuf }
	{};

	Terminal::~Terminal()
	{
		this->close();
	};


}
