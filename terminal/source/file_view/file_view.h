#pragma once 
#ifndef X69_TERMINAL_FILE_VIEW_H
#define X69_TERMINAL_FILE_VIEW_H

#define Uses_TFileList
#define Uses_TRect
#define Uses_TScrollBar

#include <tvision/tv.h>

#include <string>

namespace x69::emu::tv
{

	struct FileViewer : public TFileList
	{
	public:

		FileViewer(TRect _r, const std::string& _dir);

	private:
		std::string current_dir_ = "";

	};



}

#endif