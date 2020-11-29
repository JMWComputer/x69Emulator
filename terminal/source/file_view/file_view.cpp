#include "file_view.h"


namespace x69::emu::tv
{

	FileViewer::FileViewer(TRect _r, const std::string& _dir) : 
		TFileList{ _r, new TScrollBar{ { _r.b.x - 2, _r.a.y, _r.b.x, _r.b.y } } }, 
		current_dir_{ _dir }
	{
		this->growMode = gfGrowHiX | gfGrowHiY | gfGrowLoX;
		this->options = options | ofFramed;
	};


}