#include "includes.h"

// Assuming little-endian byte order

unsigned short ReadUShort(BYTE* data, size_t offset)
{
	return static_cast<unsigned short>(data[offset]) | (static_cast<unsigned short>(data[offset + 1]) << 8);
}

float ReadFloat(const BYTE* data, size_t offset)
{
    DWORD floatData = *(DWORD*)(data + offset);
    return *(float*)&floatData;
}