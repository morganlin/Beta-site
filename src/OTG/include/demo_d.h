#ifndef __demo_d_h__
#define __demo_d_h__
/*******************************************************************************
** File          : $HeadURL$ 
** Author        : $Author$
** Project       : HSCTRL 
** Instances     : 
** Creation date : 
********************************************************************************
********************************************************************************
** ChipIdea Microelectronica - IPCS
** TECMAIA, Rua Eng. Frederico Ulrich, n 2650
** 4470-920 MOREIRA MAIA
** Portugal
** Tel: +351 229471010
** Fax: +351 229471011
** e_mail: chipidea.com
********************************************************************************
** ISO 9001:2000 - Certified Company
** (C) 2005 Copyright Chipidea(R)
** Chipidea(R) - Microelectronica, S.A. reserves the right to make changes to
** the information contained herein without notice. No liability shall be
** incurred as a result of its use or application.
********************************************************************************
** Modification history:
** $Date$
** $Revision$
*******************************************************************************
*** Description:      
***  This file contains the VUSB32 demo application header.
***                                                               
**************************************************************************
**END*********************************************************/

#define  TOTAL_LOGICAL_ADDRESS_BLOCKS     (400)
#define  LENGTH_OF_EACH_LAB               (512)

#define  USB_DCBWSIGNATURE       (0x43425355)
#define  USB_DCSWSIGNATURE       (0x53425355)
#define  USB_CBW_DIRECTION_BIT   (0x80)

/* USB 1.1 Setup Packet */
typedef struct setup_struct {
   uint_8      REQUESTTYPE;
   uint_8      REQUEST;
   uint_16     VALUE;
   uint_16     INDEX;
   uint_16     LENGTH;
} SETUP_STRUCT, _PTR_ SETUP_STRUCT_PTR;


/* USB Mass storage Inquiry Command */
typedef struct mass_storage_inquiry {
   uchar    OPCODE;
   uchar    LUN;
   uchar    PAGE_CODE;
   uchar    RESERVED1;
   uchar    ALLOCATION_LENGTH;
   uchar    RESERVED2[7];
} MASS_STORAGE_INQUIRY_STRUCT, _PTR_ MASS_STORAGE_INQUIRY_PTR;

/* USB Mass storage READ CAPACITY Data */
typedef struct mass_storage_read_capacity {
   uchar    LAST_LOGICAL_BLOCK_ADDRESS[4];
   uchar    BLOCK_LENGTH_IN_BYTES[4];
} MASS_STORAGE_READ_CAPACITY_STRUCT, _PTR_ MASS_STORAGE_READ_CAPACITY_STRUCT_PTR;

/* USB Mass storage Device information */
typedef struct mass_storage_device_info {
   uchar    PERIPHERAL_DEVICE_TYPE;    /* Bits 0-4. All other bits reserved */
   uchar    RMB;                       /* Bit 7. All other bits reserved */
   uchar    ANSI_ECMA_ISO_VERSION;     /* ANSI: bits 0-2, ECMA: bits 3-5, 
                                       ** ISO: bits 6-7 
                                       */
   uchar    RESPONSE_DATA_FORMAT;      /* bits 0-3. All other bits reserved */
   uchar    ADDITIONAL_LENGTH;         /* For UFI device: always set to 0x1F */
   uchar    RESERVED1[3];
   uchar    VENDOR_INFORMATION[8];
   uchar    PRODUCT_ID[16];
   uchar    PRODUCT_REVISION_LEVEL[4];
} MASS_STORAGE_DEVICE_INFO_STRUCT, _PTR_ MASS_STORAGE_DEVICE_INFO_PTR;

#define MIN(a,b) ((a) < (b) ? (a) : (b))      

//#define BUFFERSIZE      1023
#define BUFFERSIZE      0x10000
#define EP_TEMP_BUFFERSIZE	   (32)
#define MASS_STORAGE_INTERFACE (0)

typedef struct double_buffer_struct { /* Body */
   uint_8      buffer[2][BUFFERSIZE];
   boolean     whichbuffer;
} DOUBLE_BUFFER_STRUCT, _PTR_ DOUBLE_BUFFER_STRUCT_PTR;

#define INIT_DBUF(x)    ((x.whichbuffer) = 0)
#define TOGGLE_DBUF(x)  ((x.whichbuffer) ^= 1)
#define DBUF(x)         (x.buffer[x.whichbuffer])
      
#endif
/* EOF */
