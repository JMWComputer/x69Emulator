#include "pf_persistant.h"

#include <filesystem>

pf_persistant PF_PERSISTANT{};
std::fstream PF_FILE_IO{};


EXPORT_NAME uint8_t DECL_PREFIX x69_peripheral_size()
{
	return sizeof(PF_PERSISTANT);
};

EXPORT_NAME uint8_t DECL_PREFIX x69_peripheral_read(uint8_t _offset)
{
	return PF_PERSISTANT.data[_offset];
};

EXPORT_NAME void DECL_PREFIX x69_peripheral_write(uint8_t _offset, uint8_t _value)
{
	if (_offset == 0)
	{
		if ((_value & 0b00001000) != 0)
		{
			// Create file
			//PF_FILE_IO = std::fstream{"pst.bin"};
		}
		else if ((_value & 0b00000100) != 0)
		{
			// Preform close
			//PF_FILE_IO.close();
		}
		else if ((_value & 0b00000010) != 0)
		{
			// Preform write
			std::ofstream _ofstr{ "pst.bin" };
			_ofstr.seekp(((std::streampos)PF_PERSISTANT.address[0] | ((std::streampos)PF_PERSISTANT.address[1] << 8)));
			_ofstr.write((const char*)PF_PERSISTANT.buffer, PF_PERSISTANT.count);
			_ofstr.flush();
			_ofstr.close();

		}
		else if ((_value & 0b00000001) != 0)
		{
			// Preform read
			std::ifstream _ifstr{ "pst.bin" };
			_ifstr.seekg(((std::streampos)PF_PERSISTANT.address[0] | ((std::streampos)PF_PERSISTANT.address[1] << 8)));
			_ifstr.read((char*)PF_PERSISTANT.buffer, PF_PERSISTANT.count);
			_ifstr.close();

		};
	}
	else
	{
		PF_PERSISTANT.data[_offset] = _value;
	};

};
