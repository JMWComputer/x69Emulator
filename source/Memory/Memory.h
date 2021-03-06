#pragma once

#include "Peripheral/Peripheral.h"
#include "Spec/Spec.h"

#include <array>



namespace x69::emu
{

	/**
	 * @brief Holds the memory for the emulator, this is the direct memory and is not persistant
	*/
	class Memory
	{
	public:
		using cpu_traits = CPUVersionTraits;
		using word_type = typename cpu_traits::word_type;
		using address_type = cpu_traits::direct_address_type;

	private:
		using ContainerT = std::array<word_type, cpu_traits::direct_memory_size()>;

		struct Periph
		{
			uint8_t size() const { return this->pf.size(); };
			uint8_t read(uint8_t _offset) const { return this->pf.read(_offset); };
			void write(uint8_t _offset, uint8_t _val) { this->pf.write(_offset, _val); };

			x69Peripheral pf;
			address_type base_address;

			Periph(x69Peripheral _pf, address_type _baseAddress) :
				pf{ std::move(_pf) }, base_address{ _baseAddress }
			{};
		};

	public:
		using iterator = typename ContainerT::iterator;
		using const_iterator = typename ContainerT::const_iterator;

		using reverse_iterator = typename ContainerT::reverse_iterator;
		using const_reverse_iterator = typename ContainerT::const_reverse_iterator;

		iterator begin() noexcept { return this->data_.begin(); };
		const_iterator begin() const noexcept { return this->data_.cbegin(); };
		const_iterator cbegin() const noexcept { return this->data_.cbegin(); };

		iterator end() noexcept { return this->data_.end(); };
		const_iterator end() const noexcept { return this->data_.cend(); };
		const_iterator cend() const noexcept { return this->data_.cend(); };

		reverse_iterator rbegin() noexcept { return this->data_.rbegin(); };
		const_reverse_iterator rbegin() const noexcept { return this->data_.crbegin(); };
		const_reverse_iterator crbegin() const noexcept { return this->data_.crbegin(); };

		reverse_iterator rend() noexcept { return this->data_.rend(); };
		const_reverse_iterator rend() const noexcept { return this->data_.crend(); };
		const_reverse_iterator crend() const noexcept { return this->data_.crend(); };

	protected:
		Periph* find_periph(address_type _addr)
		{
			for (auto& p : this->peripherals_)
			{
				if (_addr >= p.base_address && _addr < p.base_address + p.size())
				{
					return &p;
				};
			};
			return nullptr;
		};
		const Periph* find_periph(address_type _addr) const
		{
			for (auto& p : this->peripherals_)
			{
				if (_addr >= p.base_address && _addr < p.base_address + p.size())
				{
					return &p;
				};
			};
			return nullptr;
		};

	public:

		word_type read(address_type _addr) const noexcept
		{
			auto _p = this->find_periph(_addr);
			if (_p)
			{
				return _p->read(_addr - _p->base_address);
			}
			else
			{
				return this->data_.at(_addr);
			};
		};

		/**
		 * @brief Preforms a raw write to memory
		 * @param _addr Address to write to
		 * @param _data Value to write
		*/
		void write(address_type _addr, word_type _data) noexcept
		{
			auto _p = this->find_periph(_addr);
			if (_p)
			{
				return _p->write(_addr - _p->base_address, _data);
			}
			else
			{
				this->data_.at(_addr) = _data;
			};
		};

		/**
		 * @brief Checks if a address can be used in virtual read/write
		 * @param _addr Address to check
		 * @return true if allowed
		*/
		bool allow_vmemop(address_type _addr) const;

		/**
		 * @brief Preform virtual memory write, will disallow reading from outside the address range
		 * @param _addr Address to write to (un-adjusted for vmem)
		 * @param _val Value to write
		*/
		word_type vread(address_type _addr);

		/**
		 * @brief Preform virtual memory write, will prevent writing to outside the address range
		 * @param _addr Address to write to (un-adjusted for vmem)
		 * @param _val Value to write
		*/
		void vwrite(address_type _addr, word_type _val);

		/**
		 * @brief Registers a peripheral at the address provided.
		 * @param _pf Peripheral data
		 * @param _baseAddress Peripheral base address
		*/
		void register_peripheral(x69Peripheral _pf, uint16_t _baseAddress)
		{
			Periph _p{ std::move(_pf), _baseAddress };
			this->peripherals_.push_back(std::move(_p));
		};

		void set_vmem_range(std::pair<address_type, address_type> _range);

		std::pair<address_type, address_type> get_vmem_range() const noexcept;

	private:
		address_type paddr_end_ = 0xF000;

		address_type vmem_bottom_{ 0x1000 };
		address_type vmem_top_{ 0x1000 };

		std::vector<Periph> peripherals_{};

		ContainerT data_{};

	};


};
