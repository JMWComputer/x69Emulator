#include "file_editor.h"

#include "x69Terminal.h"

namespace x69::emu::tv
{
	AssemblyEditor::AssemblyEditor(const TRect& _bounds, TScrollBar* _aHScrollBar,
		TScrollBar* _aVScrollBar, TIndicator* _indicator, const char* _filePath) :
		TFileEditor{ _bounds, _aHScrollBar, _aVScrollBar, _indicator, _filePath }
	{};
}

namespace x69::emu::tv
{

	void AssemblyEditorWindow::save_and_run()
	{
		std::string _msgStr{};
		_msgStr.append("as ");
		_msgStr.append(this->file_path_);
		_msgStr += ' ';
		_msgStr.append("out.o");
		this->terminal_->push_message(Terminal::Message::GotInput{ _msgStr });

		_msgStr.clear();
		_msgStr.append("do out.o");
		this->terminal_->push_message(Terminal::Message::GotInput{ _msgStr });

	};

	void AssemblyEditorWindow::handleEvent(TEvent& _event)
	{
		TWindow::handleEvent(_event);

		if (_event.what == evCommand)
		{
			switch (_event.message.command)
			{
			case cmSaveAndRun:
				this->save_and_run();
				break;
			default:
				break;
			};
			clearEvent(_event);
		}
		else if (_event.what == evKeyDown)
		{
			switch (_event.keyDown.keyCode)
			{
			case kbAltS:
				this->editor_->saveFile();
				break;
			default:
				break;
			};
			clearEvent(_event);
		};

	};

	void AssemblyEditorWindow::draw()
	{
		TWindow::draw();
		return;
	};






	AssemblyEditorWindow::AssemblyEditorWindow(const TRect& _r, const char* _fileName, short _anum, Terminal* _terminal) :
		TWindow{ _r, _fileName, _anum },
		TWindowInit{ &AssemblyEditorWindow::initFrame },
		terminal_{ _terminal }, file_path_{ _fileName }
	{
		auto _thisRect = this->getClipRect();
		_thisRect.grow(-1, -1);

		auto _scrollRect = _thisRect;
		_scrollRect.a.y = _scrollRect.b.y - 1;
		auto _hScroll = new TScrollBar{ _scrollRect };
		this->hscroll_ = _hScroll;
		this->insert(_hScroll);

		_scrollRect = _thisRect;
		_scrollRect.a.x = _scrollRect.b.x - 1;
		auto _vScroll = new TScrollBar{ _scrollRect };
		this->vscroll_ = _vScroll;
		this->insert(_vScroll);

		auto _indicRect = _thisRect;
		_indicRect.b.y = _indicRect.a.y + 1;
		_indicRect.b.x = _indicRect.a.x + 6;

		auto _indic = new TIndicator{ _indicRect };
		this->insert(_indic);

		auto _editorRect = _thisRect;
		_editorRect.a.y += 2;
		--_editorRect.b.x;
		--_editorRect.b.y;

		this->editor_ = new AssemblyEditor{ _editorRect, _hScroll, _vScroll, _indic, _fileName };
		this->insert(this->editor_);

		int _bWidth = 12;

		_editorRect.b.y = _editorRect.a.y;
		_editorRect.a.y = _thisRect.a.y;
		_editorRect.b.x = _editorRect.a.x + _bWidth;

		this->save_button_ = new TButton{ _editorRect, "save", cmSave, 0 };
		this->insert(this->save_button_);

		_editorRect.a.x = _editorRect.b.x + 2;
		_editorRect.b.x = _editorRect.a.x + _bWidth;

		this->run_button_ = new TButton{ _editorRect, "run", cmSaveAndRun, 0 };
		this->insert(this->run_button_);

	};



	
	
}