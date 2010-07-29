#include <test_item.h>
#include "LCD/dependency.h"

extern int panel_output_test(int autotest);
extern int vga_output_test(int autotest);
extern int sdtv_output_test(int autotest);
extern int hdmi_output_test(int autotest);

struct test_item socle_lcd_output_items[] = {
	{"Panel Output Test", 
	 panel_output_test,
	 1,
	 1
	},
	{"VGA Output Test",
		vga_output_test,
		VGA_OUT,
		VGA_OUT
	},
	{"SDTV Output Test",
		sdtv_output_test,
		SDTV_OUT,
		SDTV_OUT
	},
	{"HDMI Output Test",
		hdmi_output_test,
		HDMI_OUT,
		HDMI_OUT
	},
};

struct test_item_container socle_lcd_output_container = {
	.menu_name = "SOCLE LCD Output Menu",
	.shell_name = "socle_lcd_output",
	.items = socle_lcd_output_items,
	.test_item_size = sizeof(socle_lcd_output_items)
};

extern int lcd_one_page(int autotest);
extern int lcd_two_page(int autotest);

struct test_item socle_lcd_page_items[] = {
	{"One Page", 
	 lcd_one_page,
	 1,
	 1},
	{"Two Page", 
	 lcd_two_page,
	 1,
	 1},
};

struct test_item_container socle_lcd_page_container = {
	.menu_name = "SOCLE LCD Page Menu",
	.shell_name = "socle_lcd_page",
	.items = socle_lcd_page_items,
	.test_item_size = sizeof(socle_lcd_page_items)
};

extern int lcd_rgb_888_mode(int autotest);
extern int lcd_rgb_655_mode(int autotest);
extern int lcd_rgb_565_mode(int autotest);
extern int lcd_rgb_556_mode(int autotest);
extern int lcd_rgb_555_mode(int autotest);
extern int lcd_rgb_555_r_mode(int autotest);
extern int lcd_rgb_555_g_mode(int autotest);
extern int lcd_rgb_555_b_mode(int autotest);
extern int lcd_yuv_422_mode(int autotest);
extern int lcd_yuv_420_mode(int autotest);

struct test_item socle_lcd_mode_items[] = {
	 {"RGB888", 
	 lcd_rgb_888_mode,
	 1,
	 1},
	 {"RGB655", 
	 lcd_rgb_655_mode,
	 1,
	 1},
	 {"RGB565", 
	 lcd_rgb_565_mode,
	 1,
	 1},
	 {"RGB556", 
	 lcd_rgb_556_mode,
	 1,
	 1},
	 {"RGB555", 
	 lcd_rgb_555_mode,
	 1,
	 1},
	 {"RGB555 with extra R", 
	 lcd_rgb_555_r_mode,
	 1,
	 1},
	 {"RGB555 with extra G", 
	 lcd_rgb_555_g_mode,
	 1,
	 1},
	 {"RGB555 with extra B", 
	 lcd_rgb_555_b_mode,
	 1,
	 1},
	 {"YUV 422", 
	 lcd_yuv_422_mode,
	 1,
	 1},
	 {"YUV 420", 
	 lcd_yuv_420_mode,
	 1,
	 1},
};

struct test_item_container socle_lcd_mode_container = {
	.menu_name = "SOCLE LCD Mode Menu",
	.shell_name = "socle_lcd_mode",
	.items = socle_lcd_mode_items,
	.test_item_size = sizeof(socle_lcd_mode_items)
};



extern int lcd_intr_test(int autotest);
extern int lcd_color_test(int autotest);

struct test_item lcd_intr_test_items[] = {
	{"Color Test", 
	 lcd_color_test,
	 1,
	 1},
	 {"Interrupt Test", 
	 lcd_intr_test,
	 0,
	 0},
};

struct test_item_container socle_lcd_intr_container = {
	.menu_name = "LCD INTR Menu",
	.shell_name = "socle_lcd_intr",
	.items = lcd_intr_test_items,
	.test_item_size = sizeof(lcd_intr_test_items)
};

extern int color_fill_black(int autotest);
extern int color_fill_red(int autotest);
extern int color_fill_green(int autotest);
extern int color_fill_blue(int autotest);
extern int color_fill_cyan(int autotest);
extern int color_fill_magenta(int autotest);
extern int color_fill_yellow(int autotest);
extern int color_fill_white(int autotest);
extern int color_fill_bar(int autotest);

struct test_item color_fill_test_items[] = {
	{"black", 
	 color_fill_black,
	 1,
	 1},
	{"red", 
	 color_fill_red,
	 1,
	 1},
	 {"green", 
	 color_fill_green,
	 1,
	 1},
	 {"blue", 
	 color_fill_blue,
	 1,
	 1},
	 {"cyan", 
	 color_fill_cyan,
	 1,
	 1},
	 {"magenta", 
	 color_fill_magenta,
	 1,
	 1},
	 {"yellow", 
	 color_fill_yellow,
	 1,
	 1},
	 {"white", 
	 color_fill_white,
	 1,
	 1},
	 {"color bar", 
	 color_fill_bar,
	 1,
	 1},
};

struct test_item_container socle_lcd_color_fill_container = {
	.menu_name = "CLCD Color Fill Menu",
	.shell_name = "socle_lcd_color",
	.items = color_fill_test_items,
	.test_item_size = sizeof(color_fill_test_items)
};


