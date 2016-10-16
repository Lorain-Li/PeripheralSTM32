#ifndef __TYPES_H__
#define __TYPES_H__

typedef enum
{
	FALSE = 0, TRUE = (!FALSE)
} bool;

typedef unsigned char 	u8;
typedef unsigned short 	u16;
typedef unsigned int 	u32;
typedef unsigned char 	uint8_t;
typedef unsigned short 	uint16_t;
typedef unsigned int 	uint32_t;

#define U32_MAX ((u32)0xffffffff)
#define U16_MAX ((u16)0xffff)
#define U8_MAX 	((u8)0xff)

#endif