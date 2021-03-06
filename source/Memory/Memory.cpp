#include "Memory.h"

namespace x69::emu
{

	void Memory::set_vmem_range(std::pair<address_type, address_type> _range)
	{
		this->vmem_bottom_ = _range.first;
		this->vmem_top_ = _range.second;
	};

	bool Memory::allow_vmemop(address_type _addr) const
	{
		return (_addr + this->vmem_bottom_ >= this->vmem_bottom_ && _addr + this->vmem_bottom_ < this->vmem_top_);
	};

	Memory::word_type Memory::vread(address_type _addr)
	{
		return (this->allow_vmemop(_addr)) ? this->read(_addr + this->vmem_bottom_) : 0x00;
	};

	void Memory::vwrite(address_type _addr, word_type _val)
	{
		if (this->allow_vmemop(_addr))
		{
			this->write(_addr + this->vmem_bottom_, _val);
		};
	};

	std::pair<Memory::address_type, Memory::address_type> Memory::get_vmem_range() const noexcept
	{
		return { this->vmem_bottom_, this->vmem_top_ };
	};

}

