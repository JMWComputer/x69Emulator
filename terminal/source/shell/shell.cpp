#include "shell.h"

#include <numeric>

namespace x69::emu::tv
{

	ShellView::ShellView(TRect _r) :
		TTerminal{ _r, new TScrollBar{ TRect{ 0, 0, 1, _r.b.y } }, nullptr, 64 }
	{
		this->growMode = gfGrowHiX | gfGrowHiY;
		this->options = options | ofFramed;
	};
}

namespace x69::emu::tv
{


	ShellInputLine::ShellInputLine(TRect _r) :
		TInputLine{ _r, 64 }
	{
		this->growMode = gfGrowHiX | gfGrowHiY | gfGrowLoY;
		this->options = options | ofFramed;
	};

}



namespace x69::emu::tv
{
	struct ILineData
	{
		char data[128]{};
	};

	void ShellWindow::set_input_text(const std::string& _str)
	{
		std::memcpy(this->input_->data, _str.data(), std::min<ushort>(_str.size(), this->input_->dataSize()));
	};

	void ShellWindow::clear_input_text()
	{
		std::memset(this->input_->data, 0, this->input_->dataSize());
		this->input_->curPos = 0;
		this->input_->draw();
	};

	void ShellWindow::append_shell(const std::string& _str)
	{
		this->sview_->do_sputn(_str.c_str(), _str.size());
	};

	std::string ShellWindow::get_input_text()
	{
		return std::string{ this->input_->data };
	};

	void ShellWindow::write_to_emulator(const std::string& _str)
	{
		this->terminal_->push_message(Terminal::Message::GotInput{ _str });
		this->clear_input_text();
		this->append_shell(_str);
	};


	void ShellWindow::handleEvent(TEvent& _event)
	{
		TWindow::handleEvent(_event);
		if (_event.what == evKeyDown)
		{
			switch (_event.keyDown.keyCode)
			{
			case kbEnter:
				if (this->input_->getState(sfSelected))
				{
					this->write_to_emulator(this->get_input_text());
					clearEvent(_event);
				}
				else if (this->sview_->getState(sfSelected))
				{
					clearEvent(_event);
				};
				break;
			default:
				break;
			};
		};
	};

	void ShellWindow::draw()
	{
		TWindow::draw();

		


	};

	ShellWindow::ShellWindow(TRect _r, short _num, Terminal* _terminal) :
		TWindow{ _r, "shell", _num },
		TWindowInit{ &ShellWindow::initFrame },
		terminal_{ _terminal }
	{
		auto _rect = this->getClipRect();
		_rect.grow(-1, -1);
		--_rect.b.y;
		this->sview_ = new ShellView{ _rect };
		this->insert(this->sview_);

		_rect.a.y = _rect.b.y;
		++_rect.b.y;
		this->input_ = new ShellInputLine{ _rect };
		this->insert(this->input_);

	};
	
}
