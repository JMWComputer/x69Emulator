#pragma once
#ifndef X69_TERMINAL_TERMINAL_VIEW_H
#define X69_TERMINAL_TERMINAL_VIEW_H

#define Uses_TView
#define Uses_TRect
#define Uses_TWindow
#define Uses_TPalette

#include "Colors.h"

#include <tvision/tv.h>

#include <mutex>
#include <cstdint>
#include <vector>

namespace x69::emu::tv
{

	class TerminalValues
	{
	public:
		using value_type = char;
		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;

	private:
		using ContainerT = std::vector<value_type>;

	public:
		using iterator = typename ContainerT::iterator;
		using const_iterator = typename ContainerT::const_iterator;

		iterator begin() noexcept { return this->data_.begin(); };
		const_iterator begin() const noexcept { return this->data_.cbegin(); };
		const_iterator cbegin() const noexcept { return this->data_.cbegin(); };

		iterator end() noexcept { return this->data_.end(); };
		const_iterator end() const noexcept { return this->data_.cend(); };
		const_iterator cend() const noexcept { return this->data_.cend(); };

		using size_type = typename ContainerT::size_type;
		using dimension_type = uint8_t;

		dimension_type width() const noexcept { return this->width_; };
		dimension_type height() const noexcept { return this->height_; };

		size_type area() const noexcept { return (size_type)this->width() * (size_type)this->height(); };
		size_type size() const noexcept { return this->area(); };

		void resize(dimension_type _width, dimension_type _height) 
		{
			this->width_ = _width;
			this->height_ = _height;
			this->data_.resize(this->size());
		};

		reference at(size_type _s) { return this->data_.at(_s); };
		const_reference at(size_type _s) const { return this->data_.at(_s); };

		reference operator[](size_type _s) { return this->at(_s); };
		const_reference operator[](size_type _s) const { return this->at(_s); };

		size_type pos_to_offset(dimension_type _x, dimension_type _y) const noexcept
		{
			return (size_type)(_y * this->width()) + (size_type)_x;
		};

		reference at(dimension_type _x, dimension_type _y) { return this->at( this->pos_to_offset(_x, _y)); };
		const_reference at(dimension_type _x, dimension_type _y) const { return this->at(this->pos_to_offset(_x, _y)); };

		template <typename _FwdITer>
		void assign(_FwdITer _begin, _FwdITer _end)
		{
			this->data_.assign(_begin, _end);
		};

		iterator insert(const_iterator _at, value_type _v)
		{
			this->data_.insert(_at, _v);
		};

		void clear() noexcept
		{
			this->width_ = 0;
			this->height_ = 0;
			this->data_.clear();
		};



	private:
		dimension_type width_ = 0;
		dimension_type height_ = 0;
		ContainerT data_{};

	};

	extern TerminalValues TERMINAL_VALUES;

	struct TerminalPrintout : public TView
	{
	public:
		void draw() override;
		TerminalPrintout(TRect _bounds);
	};
	
	struct TerminalWindow : public TWindow
	{
	public:

		static std::pair<int, int> get_default_size();


		TerminalWindow(const TRect& _r, const char* _title, short _num);

	private:


	};




}

#endif