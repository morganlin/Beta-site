//#define _DEVICE_DEBUG_

#include "usbprv_dev.h"

#define __VUSBHS__

#ifdef _DEVICE_DEBUG_
#define DEBUG_LOG_TRACE(fmt, args ...)   printf("%s: \n" fmt, __FUNCTION__ , ## args)

#else
#define DEBUG_LOG_TRACE(fmt, args ...)
#endif

USB_DEV_STATE_STRUCT_PTR global_usb_device_state_struct_ptr;
int in_init;

