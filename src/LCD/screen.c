#include "fb.h"

#define HD_READY 0

struct screen_info panel_35= {
	.xres=320,
	.yres=240,
	.pixclock			= 166666,
	.left_margin		= 38,
	.right_margin		= 20,
	.upper_margin	= 15,
	.lower_margin		= 5,
	.hsync_len		= 32,
	.vsync_len		= 3,
	.sync			= 0,
};

struct screen_info panel_48= {
	.xres			= 800,
	.yres			= 480,
	.pixclock			= 37037,
	.left_margin		= 87,
	.right_margin		= 16,
	.upper_margin	= 18,
	.lower_margin		= 8,
	.hsync_len		= 12,
	.vsync_len		= 1,
	.sync			= 0,
};

struct screen_info panel_70= {
	.xres			= 800,
	.yres			= 480,
	.pixclock			= 37037,
	.left_margin		= 150,
	.right_margin		= 50,
	.upper_margin	= 33,
	.lower_margin		= 10,
	.hsync_len		= 56,
	.vsync_len		= 2,
	.sync			= 0,
};

struct screen_info vga= {
	.xres			= 640,                                                      
	.yres			= 480,                                                      
	.pixclock			= 37037,                                                
	.left_margin		= 96,                                                 
	.right_margin		= 26,                                                 
	.upper_margin	= 33,                                                   
	.lower_margin		= 10,                                                 
	.hsync_len		= 96,                                                   
	.vsync_len		= 2,          
	.sync			= 0,
};

struct screen_info svga= {
	.xres			= 800,                                                      
	.yres			= 600,                                                      
	.pixclock			= 37037,                                                
	.left_margin		= 128,                                                 
	.right_margin		= 24,                                                 
	.upper_margin	= 22,                                                   
	.lower_margin		= 1,                                                 
	.hsync_len		= 72,                                                   
	.vsync_len		= 2,      
	.sync			= 0,
};

struct screen_info xvga= {
	.xres			= 1024,                                                      
	.yres			= 768,                                                      
	.pixclock			= 18518,                                                
	.left_margin		= 56,                                                 
	.right_margin		= 8,                                                 
	.upper_margin	= 40,                                                   
	.lower_margin		= 1,                                                 
	.hsync_len		= 176,                                                   
	.vsync_len		= 8,       
	.sync			= 0,
};

#if HD_READY
struct screen_info hd720= {
	.xres=320,
	.yres=240,
	.pixclock			= 166666,
	.left_margin		= 38,
	.right_margin		= 20,
	.upper_margin	= 15,
	.lower_margin		= 5,
	.hsync_len		= 30,
	.vsync_len		= 3,
	.sync			= 0,
};

struct screen_info hd1080= {
	.xres=320,
	.yres=240,
	.pixclock			= 166666,
	.left_margin		= 38,
	.right_margin		= 20,
	.upper_margin	= 15,
	.lower_margin		= 5,
	.hsync_len		= 30,
	.vsync_len		= 3,
	.sync			= 0,
};

#endif

#if defined CONFIG_PANEL48
extern int sam48_lcd_init(void);
#endif

struct screen_info* fb_get_platform_screen(void)
{
	
#ifdef CONFIG_PANEL35
	return &panel_35;
#elif	defined(CONFIG_PANEL48)
	sam48_lcd_init();
	return &panel_48;
#elif	defined(CONFIG_PANEL70)
	return &panel_70;
#else
	return &panel_70;
#endif

}

struct screen_info* fb_get_screen_by_type(int type)
{
	switch(type) {
		case PANEL35:
			return &panel_35;
#if defined(CONFIG_PANEL48)
		case PANEL48:
			sam48_lcd_init();
			return &panel_48;
#endif
		case PANEL70:
			return &panel_70;
		case VGA480:
			return &vga;
		case SVGA600:
			return &svga;
		case XVGA768:
			return &xvga;
#if HD_READY
		case HD720:
			return &hd720;
		case HD1080:
			return &hd1080;
#endif
		default:
			return &panel_70;
	}
	return &panel_70;
}