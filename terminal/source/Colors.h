#pragma once
#ifndef X69_TERMINAL_COLORS_H
#define X69_TERMINAL_COLORS_H

/*
[01:43] +w00tma5ter: 
	4-bit VGA colors, but the table is reversed, first hex digit is foreground(text) second digit is background
[01:43] +w00tma5ter: 
	0 Black
	1 Blue 
	2 Green 
	3 Cyan 
	4 Red 
	5 Magenta 
	6 Brown 
	7 White 
	8 Gray 
	9 Light Blue 
	10 Light Green 
	11 Light Cyan 
	12 Light Red 
	13 Light Magenta 
	14 Yellow 
	15 Bright White 

	https://www.fountainware.com/EXPL/vga_color_palettes.htm

	[01:46] +w00tma5ter: x0f is white text on black back
	[01:46] +w00tma5ter: first bit text color
	[01:46] +w00tma5ter: secont bit background color
	[01:46] +w00tma5ter: or hex digit rather


	Palette definitions for each type is found in the full reference (page 317+)
	UI Pages are closer to 450


*/

#define Uses_TPalette

#include <tvision/tv.h>

#include <string_view>
#include <string>

namespace x69::emu::tv
{
	enum class COLOR : char
	{
		black = 0,
		blue = 1,
		green = 2,
		cyan = 3,
		red = 4,
		magenta = 5,
		brown = 6,
		white = 7,
		gray = 8,
		light_blue = 9,
		light_green = 10,
		light_cyan = 11,
		light_red = 12,
		light_magenta = 13,
		yellow = 14,
		bright_white = 15
	};

	struct tvcolor
	{
		constexpr COLOR background() const noexcept 
		{ 
			return (COLOR)(this->col & 0x0F); 
		};
		constexpr COLOR foreground() const noexcept 
		{
			return (COLOR)(this->col >> 4);
		};

		void set_background(COLOR _col) noexcept 
		{ 
			this->col = ((uint8_t)_col) | ((uint8_t)this->foreground() << 4);
		};
		void set_foreground(COLOR _col) noexcept 
		{ 
			this->col = (((uint8_t)_col) << 4) | (uint8_t)this->background(); 
		};

		uint8_t col;
	};

	union tv_window_color
	{
		constexpr static inline auto DEFAULT_BLUE = L"\x07\x07\x0A\x01\x01\x1E\x71\x00";
		constexpr static inline auto DEFAULT_CYAN = L"\x70\x7F\x7A\x13\x13\x70\x7F\x00";
		constexpr static inline auto DEFAULT_GRAY = L"\x37\x3F\x3A\x13\x13\x3E\x21\x00";

		static auto df_blue()
		{
			tv_window_color _out{};
			std::memcpy(_out.data, DEFAULT_BLUE, 8);
			return _out;
		};
		static auto df_cyan()
		{
			tv_window_color _out{};
			std::memcpy(_out.data, DEFAULT_CYAN, 8);
			return _out;
		};
		static auto df_gray()
		{
			tv_window_color _out{};
			std::memcpy(_out.data, DEFAULT_GRAY, 8);
			return _out;
		};
		
		struct
		{
			tvcolor frame_passive;
			tvcolor frame_active;
			tvcolor frame_icon;
			tvcolor scroll_bar_page;
			tvcolor scroll_bar_reserved;
			tvcolor scroller_normal_text;
			tvcolor scroller_selected_text;
			tvcolor reserved;
		};
		tvcolor data[8]{};
	};

	union tv_dialog_box_color
	{
		constexpr static inline auto DEFAULT_BLUE = "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00";
		constexpr static inline auto DEFAULT_CYAN = "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00";
		constexpr static inline auto DEFAULT_GRAY = "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x38\x00";

		static auto df_blue()
		{
			tv_dialog_box_color _out{};
			std::memcpy(_out.data, DEFAULT_BLUE, 32);
			return _out;
		};
		static auto df_cyan()
		{
			tv_dialog_box_color _out{};
			std::memcpy(_out.data, DEFAULT_CYAN, 32);
			return _out;
		};
		static auto df_gray()
		{
			tv_dialog_box_color _out{};
			std::memcpy(_out.data, DEFAULT_GRAY, 32);
			return _out;
		};

		struct
		{
			tvcolor frame_passive;
			tvcolor frame_active;
			tvcolor frame_icon;
			tvcolor scrollbar_page;
			tvcolor scrollbar_controls;
			tvcolor statictext;
			tvcolor label_normal;
			tvcolor label_highlight;
			tvcolor label_shortcut;
			tvcolor button_normal;
			tvcolor button_default;
			tvcolor button_selected;
			tvcolor button_disabled;
			tvcolor button_shortcut;
			tvcolor button_shadow;
			tvcolor cluster_normal;
			tvcolor cluster_selected;
			tvcolor cluster_shortcut;
			tvcolor inputline_normal;
			tvcolor inputline_selected;
			tvcolor inputline_arrows;
			tvcolor history_arrow;
			tvcolor history_sides;
			tvcolor historywindow_scrollbar_page;
			tvcolor historywindow_scrollbar_controls;
			tvcolor listviewer_normal;
			tvcolor listviewer_focused;
			tvcolor listviewer_selected;
			tvcolor listviewer_divider;
			tvcolor infopane;
			tvcolor cluster_disabled;
			tvcolor reserved;
		};
		tvcolor data[32]{};
	};


	union tv_application_color
	{
		static inline auto make_default()
		{
			tv_application_color _out{};

			_out.background = tvcolor{ 0x71 };

			_out.normal_text = tvcolor{ 0x70 };
			_out.disabled_text = tvcolor{ 0x78 };

			_out.shortcut_text = tvcolor{ 0x74 };
			_out.normal_selection = tvcolor{ 0x20 };

			_out.disabled_selection = tvcolor{ 0x28 };
			_out.shortcut_selection = tvcolor{ 0x24 };

			_out.window[0] = tv_window_color::df_blue();
			_out.window[1] = tv_window_color::df_cyan();
			_out.window[2] = tv_window_color::df_gray();

			_out.dialog_box[0] = tv_dialog_box_color::df_gray();
			_out.dialog_box[1] = tv_dialog_box_color::df_blue();
			_out.dialog_box[2] = tv_dialog_box_color::df_cyan();

			return _out;
		};

		struct
		{
			tvcolor background;

			tvcolor normal_text;
			tvcolor disabled_text;
			tvcolor shortcut_text;
			tvcolor normal_selection;
			tvcolor disabled_selection;
			tvcolor shortcut_selection;

			tv_window_color window[3];
			tv_dialog_box_color dialog_box[3];
			
		};
		tvcolor data[127]{};

	};


};

#define EMU_BASIC "\x07\x07\x78\x79\x20\x28\x29"

#define EMU_WINDOW_0 "\x07\x07\x07\x07\x07\x07\x70\x07"
#define EMU_WINDOW_1 "\x37\x3F\x3A\x13\x13\x3E\x21\x00"
#define EMU_WINDOW_2 "\x70\x7F\x7A\x13\x13\x70\x7F\x00"

#define EMU_DIALOG_BOX_0 "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30\x3F\x07\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00"

#define EMU_PALETTE EMU_BASIC EMU_WINDOW_0 EMU_WINDOW_1 EMU_WINDOW_2 EMU_DIALOG_BOX_0 "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x38\x00"


#define EMU_PALETTE_DEFAULT "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x00\x37\x3F\x3A\x13\x13\x3E\x21\x00\x70\x7F\x7A\x13\x13\x70\x7F\x00\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x00\x00\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x38\x00"

namespace x69::emu::tv
{







}

#endif