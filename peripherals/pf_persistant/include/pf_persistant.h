#pragma once
#ifndef X69_PF_PERSISTANT_H
#define X69_PF_PERSISTANT_H

#include <stdint.h>
#include <array>
#include <fstream>

extern "C"
{
	union pf_persistant
	{
		struct
		{
			uint8_t control;
			uint8_t count;
			uint8_t address[2];
			uint8_t buffer[64];
		};
		uint8_t data[68]{};
	};
};

extern pf_persistant PF_PERSISTANT;
extern std::fstream PF_FILE_IO;

#define EXPORT_NAME __declspec(dllexport)
#define DECL_PREFIX __cdecl

#ifdef __cplusplus
extern "C"
{
#endif

	EXPORT_NAME uint8_t DECL_PREFIX x69_peripheral_size();

	EXPORT_NAME uint8_t DECL_PREFIX x69_peripheral_read(uint8_t _offset);

	EXPORT_NAME void DECL_PREFIX x69_peripheral_write(uint8_t _offset, uint8_t _value);


#ifdef __cplusplus
};
#endif

#endif
