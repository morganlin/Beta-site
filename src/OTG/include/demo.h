#ifndef _demo_h_
#define _demo_h_ 1

extern void service_ep0(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void reset_ep0(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void detect_speed(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void service_ep1(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void service_ep2(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void initialize_ep2(void);

extern void service_ep3(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
extern void service_ep4(_usb_device_handle, boolean, uint_8, uint_8_ptr, uint_32, uint_8);
//leonid+ for sio
extern void initialize_ep4(void);

extern unsigned long initialize_device(void);

#endif
/* EOF */
