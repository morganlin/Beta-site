#ifndef _type_otg_h_included_
#define _type_otg_h_included_

//OTG use 
#define _PTR_      *
#define _CODE_PTR_ *

typedef char _PTR_	char_ptr;    /* signed character       */
typedef unsigned char	uchar, 		_PTR_   uchar_ptr;   /* unsigned character     */
typedef signed   char 	int_8,		_PTR_   int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char	uint_8,		_PTR_   uint_8_ptr;  /* 8-bit signed integer   */
typedef short 		int_16,		_PTR_   int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16,	_PTR_  uint_16_ptr; /* 16-bit unsigned integer*/
typedef long  		int_32,		_PTR_   int_32_ptr;  /* 32-bit signed integer  */
typedef unsigned long  uint_32, 	_PTR_  uint_32_ptr; /* 32-bit unsigned integer*/

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

#endif

