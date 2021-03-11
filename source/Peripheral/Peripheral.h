#pragma once

#include "Spec/Spec.h"

#include <cstdint>
#include <utility>
#include <optional>
#include <filesystem>
#include <vector>

namespace x69::emu
{
	constexpr static inline auto x69Peripheral_Write_Name = "x69_peripheral_write";
	constexpr static inline auto x69Peripheral_Read_Name = "x69_peripheral_read";
	constexpr static inline auto x69Peripheral_Size_Name = "x69_peripheral_size";
}

#if defined(WIN32) || defined(_WIN32) || defined(_WIN32_) || defined(_NT_)

#include <Windows.h>

#undef min
#undef max

namespace x69::emu
{
	using x69Peripheral_Write = void(__cdecl*)(uint8_t, uint8_t);
	using x69Peripheral_Read = uint8_t(__cdecl*)(uint8_t);
	using x69Peripheral_Size = uint8_t(__cdecl*)();

	struct x69Peripheral
	{
	public:
		bool is_valid() const noexcept { return this->size_ != NULL && this->write_ != NULL && this->read_ != NULL; };

		uint8_t size() const { return (*this->size_)(); };
		uint8_t read(uint8_t _offset) const { return (*this->read_)(_offset); };
		void write(uint8_t _offset, uint8_t _val) const { (*this->write_)(_offset, _val); };

		x69Peripheral(HINSTANCE _ins, x69Peripheral_Size _size, x69Peripheral_Read _read, x69Peripheral_Write _write) :
			hinst_lib_{ _ins }, size_{ _size }, read_{ _read }, write_{ _write }
		{};

		x69Peripheral(x69Peripheral&& other) noexcept :
			hinst_lib_{ std::exchange(other.hinst_lib_, HINSTANCE{}) },
			size_{ other.size_ }, read_{ other.read_ }, write_{ other.write_ }
		{};
		x69Peripheral& operator=(x69Peripheral&& other) noexcept
		{
			this->hinst_lib_ = std::exchange(other.hinst_lib_, HINSTANCE{});
			this->size_ = other.size_;
			this->read_ = other.read_;
			this->write_ = other.write_;
			return *this;
		};

		~x69Peripheral()
		{
			if (this->hinst_lib_ != NULL)
			{
				auto _fres = FreeLibrary(this->hinst_lib_);
			};
		};

	private:
		x69Peripheral_Size size_ = nullptr;
		x69Peripheral_Read read_ = nullptr;
		x69Peripheral_Write write_ = nullptr;

		HINSTANCE hinst_lib_;


	};

	static std::optional<x69Peripheral> open_peripheral_lib(const std::filesystem::path& _path)
	{
		BOOL _fRunTimeLinkSuccess = FALSE;

		// Get a handle to the DLL module

		auto _hintLib = LoadLibrary(TEXT(_path.string().c_str()));

		// If the handle is valid, try to get the function address

		if (_hintLib != NULL)
		{
			auto _size = (x69Peripheral_Size)GetProcAddress(_hintLib, x69Peripheral_Size_Name);
			auto _read = (x69Peripheral_Read)GetProcAddress(_hintLib, x69Peripheral_Read_Name);
			auto _write = (x69Peripheral_Write)GetProcAddress(_hintLib, x69Peripheral_Write_Name);

			return x69Peripheral{ _hintLib, _size, _read, _write };
		};

		// If unable to call the DLL function, use an alternative.
		return std::nullopt;

	};

};

#undef max
#undef min

#elif defined(__linux__) || defined(__unix__)
#else
#endif

namespace x69::emu
{

	struct PeripheralLayout
	{
		struct periph
		{
			std::filesystem::path path;
			uint16_t address;
		};
		std::vector<periph> peripherals{};
	};

	std::optional<PeripheralLayout> parse_peripherals_file(const std::filesystem::path& _path);

	struct PeripheralAPI
	{
		using cpu_traits = CPUVersionTraits;
		using address_type = typename cpu_traits::direct_address_type;
		using word_type = typename cpu_traits::word_type;

		virtual address_type size() const = 0;

		virtual void write(address_type _offset, word_type _val) = 0;
		virtual word_type read(address_type _offset) = 0;

		virtual ~PeripheralAPI() = default;
	};

};



