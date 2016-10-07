#include "type.h"

#define MTXSEG = 0x1000


// Get byte from umode user space
u8 get_byte(u16 segment, u16 offset)
{
	u8 byte;
	setds(segment);
	byte = *(u8*)offset;

	setds(MTXSEG);
	return byte;
}

// Get a word from umode 
u16 get_word(u16 segment, u16 offset)
{
	u16 byte;
	setds(segment);
	byte = *(u16)offset;
	setds(MTXSEG);
	return byte;
}