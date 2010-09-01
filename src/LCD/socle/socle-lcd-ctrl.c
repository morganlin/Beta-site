#include <genlib.h>
#include <test_item.h>
#include "regs-lcd.h"
#include "LCD/color_fill.h"
#include "LCD/fb.h"
#include "LCD/dependency.h"
#include "LCD/pwm-ctrl.h"
#ifdef CONFIG_PC9220	
#include <pc9220-scu.h>
#endif

#ifdef CONFIG_PC9223	
#include <pc9223-scu.h>
#endif

#define SOCLE_LCD_DEBUG 1

struct soclefb_info {
	struct screen_info *screen;
	struct mem_info mem;
	unsigned int reg_base;
	unsigned int ctrl0;
	unsigned int ctrl2;
	unsigned int htiming;
	unsigned int vtiming;
	unsigned int resolution;
	int irq_num;
	int mode;
	int swap;
	int int_en;
};

struct soclefb_info sfb_info;

static inline u32 socle_lcd_read(struct soclefb_info *sfb, u32 reg)
{
	return ioread32(sfb->reg_base + reg);
}

static inline void socle_lcd_write(struct soclefb_info *sfb, u32 val, u32 reg)
{
	iowrite32(val, sfb->reg_base + reg);
}

static int lcdc_regs_set(struct soclefb_info *sfb)
{
	struct screen_info *screen=sfb->screen;
	int in_clk,pix_clk,clk_div=2;

	if(socle_lcd_read(sfb, SOCLE_LCD_INTR_VER)!=0x0b01) {    
		if(screen->xres==1024)
			sfb->htiming=(screen->left_margin <<16) | (screen->right_margin << 24) | 0x2;	
		else
			sfb->htiming=(screen->left_margin <<16) | (screen->right_margin << 24) | (screen->xres & 0x3fc);

		if(screen->hsync_len!=1)
			sfb->htiming |= ((screen->hsync_len>>2) <<10);
		sfb->vtiming= (screen->upper_margin << 16) | (screen->lower_margin << 24) | (screen->yres);
		sfb->vtiming |= (screen->vsync_len <<10);
		
	}
	else {   //L603
		sfb->htiming=(screen->left_margin <<8) | (screen->right_margin << 20) ;
		if(screen->hsync_len!=1)
			sfb->htiming |= (screen->hsync_len>>2);		
		sfb->vtiming=(screen->upper_margin << 8) | (screen->lower_margin << 20) | (screen->vsync_len);
		sfb->resolution=(screen->yres<<16) | (screen->xres);
	}
	
	sfb->ctrl0 = 0;
	sfb->ctrl2 = 0;
	sfb->ctrl0 |= screen->sync & FB_SYNC_HOR_HIGH_ACT  ? 0 : SOCLE_LCD_CTRL0_HSYNC;
	sfb->ctrl0 |= screen->sync & FB_SYNC_VERT_HIGH_ACT ? 0 : SOCLE_LCD_CTRL0_VSYNC;

#if 0 && defined(CONFIG_PC9223) && defined(CONFIG_PANEL35)
	sfb->ctrl0 |= SOCLE_LCD_CTRL0_DTMG;
#endif

	switch(sfb->mode) {
		case RGB888:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_24BPP;
			break;
		case RGB655:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_COLOUR_RED;
			break;
		case RGB565:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_COLOUR_GREEN;
			break;
		case RGB556:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_COLOUR_BLUE;
			break;
		case RGB555:
			sfb->ctrl0 &= ~SOCLE_LCD_CTRL0_COLOUR;
			sfb->ctrl0 &= ~SOCLE_LCD_CTRL0_LUMCONFIG;
			break;
		case RGB555_R:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_LUMCONFIG_R;
			break;
		case RGB555_G:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_LUMCONFIG_G;
			break;
		case RGB555_B:
			sfb->ctrl0 |= SOCLE_LCD_CTRL0_LUMCONFIG_B;
			break;
		case YUV422:
			sfb->ctrl2 |= SOCLE_LCD_YUV422;
			sfb->ctrl2 |= SOCLE_LCD_YUV2RGB_EN;
			break;
		case YUV420:
			sfb->ctrl2 |= SOCLE_LCD_YUV420;
			sfb->ctrl2 |= SOCLE_LCD_YUV2RGB_EN;
			break;
	}

#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	in_clk=sq_scu_apb_clock_get()/1000000;  //get_apb_clk
#elif defined(CONFIG_MDK3D)
	in_clk=54;
#else
	in_clk=sq_scu_ahb_clock_get()/1000000;
#endif
	pix_clk=(1000000/(screen->pixclock));

	while(pix_clk) {
		if(pix_clk*clk_div>=in_clk)
			break;
		clk_div++;
	}

	if(clk_div>255)
		clk_div=255;

	//clk_div = 8;
		
	sfb->ctrl0 &= ~(0xff<<8);
	sfb->ctrl0 |= (clk_div<<8);

 	//yuv mode
 	if(sfb->mode==YUV422 || sfb->mode==YUV420) {
 		//yuv page0
 		socle_lcd_write(sfb, sfb->mem.page0_addr, SOCLE_LCD_Y_PAGE0_ADDR);
 		socle_lcd_write(sfb, sfb->mem.page0_addr + screen->xres * screen->yres, SOCLE_LCD_Cb_PAGE0_ADDR);
 		if(sfb->mode==YUV422)
 			socle_lcd_write(sfb, sfb->mem.page0_addr + screen->xres * screen->yres + screen->xres * screen->yres/2, SOCLE_LCD_Cr_PAGE0_ADDR);
 		else
 			socle_lcd_write(sfb, sfb->mem.page0_addr + screen->xres * screen->yres + screen->xres * screen->yres/4, SOCLE_LCD_Cr_PAGE0_ADDR);	
 		//yuv page1
 		if(sfb->swap) {
 			socle_lcd_write(sfb, sfb->mem.page1_addr, SOCLE_LCD_Y_PAGE1_ADDR);
 			socle_lcd_write(sfb, sfb->mem.page1_addr + screen->xres * screen->yres, SOCLE_LCD_Cb_PAGE1_ADDR);
 			if(sfb->mode==YUV422)
 				socle_lcd_write(sfb, sfb->mem.page1_addr + screen->xres * screen->yres + screen->xres * screen->yres/2, SOCLE_LCD_Cr_PAGE1_ADDR);
 			else
 				socle_lcd_write(sfb, sfb->mem.page1_addr + screen->xres * screen->yres + screen->xres * screen->yres/4, SOCLE_LCD_Cr_PAGE1_ADDR);
 			sfb->ctrl0 |= SOCLE_LCD_CTRL0_PAGESWAP;
 		}
	}
	else {
		//rgb page0
		socle_lcd_write(sfb, sfb->mem.page0_addr, SOCLE_LCD_PAGE0_ADDR);
		//rgb page1
		if(sfb->swap) {
 			socle_lcd_write(sfb, sfb->mem.page1_addr, SOCLE_LCD_PAGE1_ADDR);
 			sfb->ctrl0 |= SOCLE_LCD_CTRL0_PAGESWAP;
 		}
	}
	
	//write regs
	socle_lcd_write(sfb, sfb->ctrl0, SOCLE_LCD_CTRL0);
	socle_lcd_write(sfb, sfb->ctrl2, SOCLE_LCD_YUV2RGB_CTRL);
	socle_lcd_write(sfb, sfb->htiming, SOCLE_LCD_H_TIMING);
	socle_lcd_write(sfb, sfb->vtiming, SOCLE_LCD_V_TIMING);
	//L603
	if(socle_lcd_read(sfb, SOCLE_LCD_INTR_VER) == 0x0b01)  
		socle_lcd_write(sfb, sfb->resolution, SOCLE_LCD_RESOLUTION);
	
	
#if SOCLE_LCD_DEBUG	
	printf("ctrl0=%x\n", socle_lcd_read(sfb, SOCLE_LCD_CTRL0));
	printf("ctrl2=%x\n", socle_lcd_read(sfb, SOCLE_LCD_YUV2RGB_CTRL));
	printf("tim2=%x\n", socle_lcd_read(sfb, SOCLE_LCD_H_TIMING));
	printf("ctrl=%x\n", socle_lcd_read(sfb, SOCLE_LCD_V_TIMING));
	printf("addr=%x\n", socle_lcd_read(sfb, SOCLE_LCD_PAGE0_ADDR));
#endif	
	return 0;
}

int fb_init(int mode)
{
	
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_PWM0);
	socle_scu_dev_enable(SOCLE_DEVCON_LCDC);
	socle_scu_lcdc_clk_input_mpll_outpput();
#endif
	
	sfb_info.reg_base=SOCLE_LCD_BASE;
	sfb_info.irq_num=SOCLE_LCD_IRQ;
	sfb_info.mem.page0_addr=PAGE0_ADDR;
	sfb_info.mem.page1_addr=PAGE1_ADDR;
	sfb_info.mem.lut_addr=LUT_ADDR;
	sfb_info.screen=fb_get_platform_screen();
	if(mode==NONE)
		return 0;
	sfb_info.mode=mode;
	lcdc_regs_set(&sfb_info);
	return 0;
}

int fb_set_screen_info(struct screen_info *screen)
{
	sfb_info.screen=screen;
	lcdc_regs_set(&sfb_info);
	return 0;
}

int fb_get_screen_info(struct screen_info *screen)
{
	memcpy((char *)screen, (char *)sfb_info.screen, sizeof(struct screen_info));
	return 0;
}

int fb_get_mem_info(struct mem_info *mem)
{
	memcpy((char *)mem, (char *)&sfb_info.mem, sizeof(struct mem_info));
	return 0;
}

int fb_enable(void)
{
	socle_lcd_write(&sfb_info, socle_lcd_read(&sfb_info, SOCLE_LCD_CTRL0) | SOCLE_LCD_CTRL0_ENABLE, SOCLE_LCD_CTRL0);
	return 0;
}

int fb_disable(void)
{
	socle_lcd_write(&sfb_info, socle_lcd_read(&sfb_info, SOCLE_LCD_CTRL0)& ~SOCLE_LCD_CTRL0_ENABLE, SOCLE_LCD_CTRL0);
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_disable(SOCLE_DEVCON_PWM0);
	socle_scu_dev_disable(SOCLE_DEVCON_LCDC);
#endif
	return 0;
}


//***********test item*************//

struct test_item_container socle_lcd_output_container;
struct test_item_container socle_lcd_page_container;
struct test_item_container socle_lcd_mode_container;
struct test_item_container socle_lcd_intr_container;
struct test_item_container socle_lcd_color_fill_container;

int lcd_test (int autotest)
{
	int ret = 0;

	// for i2c controller
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_I2C);
#endif

	fb_init(NONE);
	ret = test_item_ctrl(&socle_lcd_output_container, autotest);
	fb_disable();
	
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_disable(SOCLE_DEVCON_I2C);	
#endif
	return ret;
}

extern int panel_output_test(int autotest)
{
	int ret=0;
	sfb_info.screen=fb_get_platform_screen();
	Socle_PWM_init();
	ret = test_item_ctrl(&socle_lcd_page_container, autotest);
	return ret;
}

extern int lcd_one_page(int autotest)
{
	int ret=0;
	sfb_info.swap=0;
	ret = test_item_ctrl(&socle_lcd_mode_container, autotest);
	return ret;
}

extern int lcd_two_page(int autotest)
{
	int ret=0;
	sfb_info.swap=1;
	ret = test_item_ctrl(&socle_lcd_mode_container, autotest);
	return ret;
}

extern int lcd_rgb_888_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB888;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_655_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB655;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_565_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB565;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_556_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB556;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_555_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB555;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_555_r_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB555_R;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_555_g_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB555_G;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_rgb_555_b_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=RGB555_B;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_yuv_422_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=YUV422;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_yuv_420_mode(int autotest)
{
	int ret=0;
	sfb_info.mode=YUV420;
	ret = test_item_ctrl(&socle_lcd_intr_container, autotest);
	return ret;
}

extern int lcd_color_test(int autotest)
{
	int ret=0;
	ret = test_item_ctrl(&socle_lcd_color_fill_container, autotest);
	
	return ret;
}

extern int color_fill_black(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=BLACK;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
			
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_red(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=RED;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_green(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=GREEN;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_blue(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=BLUE;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_cyan(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=CYAN;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_magenta(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=MAGENTA;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_yellow(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=YELLOW;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_white(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=WHITE;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int color_fill_bar(int autotest)
{
	struct color_info cinfo;
	
	cinfo.xres=sfb_info.screen->xres;
	cinfo.yres=sfb_info.screen->yres;
	cinfo.color=COLORBAR;
	cinfo.mode=sfb_info.mode;
	cinfo.addr=sfb_info.mem.page0_addr;
	socle_color_fill(&cinfo);
	
	if(sfb_info.swap) {
		cinfo.addr=sfb_info.mem.page1_addr;
		socle_color_fill(&cinfo);
	}
	
	lcdc_regs_set(&sfb_info);
	fb_enable();
	return 0;
}

extern int lcd_intr_test(int autotest)
{
	return 0;
}

#define VGA 1
#define SDTV 2

extern int ch7026_init(int out);

extern int vga_output_test(int autotest)
{
	

	sfb_info.screen=fb_get_screen_by_type(VGA480);
	sfb_info.mode=RGB888;
	sfb_info.swap=0;
	color_fill_bar(0);
	ch7026_init(VGA);
	return 0;
}

extern int sdtv_output_test(int autotest)
{

	sfb_info.screen=fb_get_screen_by_type(VGA480);
	sfb_info.mode=RGB888;
	sfb_info.swap=0;
	color_fill_bar(0);
	ch7026_init(SDTV);
	return 0;
}

extern int cat6613_init(void);
	
extern int hdmi_output_test(int autotest)
{

	sfb_info.screen=fb_get_screen_by_type(VGA480);
	sfb_info.mode=RGB888;
	sfb_info.swap=0;
	color_fill_bar(0);
	cat6613_init();
	return 0;
}
