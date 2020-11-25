#pragma once

#define Uses_TFileEditor
#define Uses_TScrollBar
#define Uses_TIndicator
#define Uses_TWindow
#define Uses_TButton


#include <tvision/tv.h>


namespace x69::emu
{
	class Terminal;
}

namespace x69::emu::tv
{
	


	class AssemblyEditor : public TFileEditor
	{
	public:
		AssemblyEditor(const TRect& _bounds, TScrollBar* _aHScrollBar, TScrollBar* _aVScrollBar, TIndicator* _indicator, const char* _filePath);

	private:

	};

	class AssemblyEditorWindow : public TWindow
	{
	public:
		void handleEvent(TEvent& _event) override;
		void draw() override;

		AssemblyEditorWindow(const TRect& _r, const char* _fileName, short _anum, Terminal* _terminal);

	private:
		void save_and_run();

		std::string file_path_;

		Terminal* terminal_ = nullptr;

		AssemblyEditor* editor_ = nullptr;
		TScrollBar* hscroll_ = nullptr;
		TScrollBar* vscroll_ = nullptr;

		TButton* save_button_ = nullptr;
		TButton* run_button_ = nullptr;

	};




}