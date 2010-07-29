#ifndef _CAT6023_H_
#define _CAT6023_H_

#include <type.h>

extern int cat6023_init(u8 format);
enum output_format {
	EMB_YUV,
	EMB_YUV656,
	EXT_BGR,
};

#endif //_CAT6023_H_
