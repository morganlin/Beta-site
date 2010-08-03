#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include "I2C/i2c.h"
#include "I2C/TVP5145.h"
#include "I2C/CH700X.h"
#include "vip.h"
#include "vop.h"
#include "LCD/pwm-ctrl.h"
#include "LCD/fb.h"
#include "LCD/color_fill.h"
#ifdef CONFIG_PC9220
#include <pc9220-scu.h>
#endif

#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

#define VIP_TO_VOP 0
#define VOP_TO_VIP 1
#define VIP_TO_LCD 2
#define VOP_COLOR_DISPLAY 3

#define FRAME1_ADDR		(SOCLE_MEMORY_ADDR_START + 0x1800000)
#define FRAME2_ADDR		(SOCLE_MEMORY_ADDR_START + 0x1900000)

#define CCIR656 3
//.................................
static int format,size,output422,test_flag;
static int socle_vip_to_vop_run(void);
static int socle_vip_to_lcd_run(void);
static int socle_vop_color_display_run(void);

extern int ch7026_init(int out);
extern int mt9v125_init(void);

extern struct test_item_container socle_vip_vop_main_container;
extern struct test_item_container socle_viop_format_container;
extern struct test_item_container socle_viop_size_container;
extern struct test_item_container socle_vip_42x_container;

extern int
vip_vop_test(int autotest)
{
	int ret = 0;
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_I2C);
	socle_scu_dev_enable(SOCLE_DEVCON_LCDC_VOP);
#if defined(CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_VIP);
#endif
#endif
	ret = test_item_ctrl(&socle_vip_vop_main_container, autotest);
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_dev_disable(SOCLE_DEVCON_I2C);
	socle_scu_dev_disable(SOCLE_DEVCON_LCDC_VOP);
#if defined(CONFIG_PC9223)
	socle_scu_dev_disable(SOCLE_DEVCON_VIP);
#endif
#endif
	return ret;
}

extern int
socle_vip_to_vop_test(int autotest)
{
	int ret = 0;
	test_flag=VIP_TO_VOP;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	vipStop();
	vopStop();
	return ret;
}

extern int
socle_vip_to_lcd_test(int autotest)
{
	int ret = 0;
	test_flag=VIP_TO_LCD;
	Socle_PWM_init();
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	fb_disable();
	return ret;
}


extern int
socle_vop_color_display_test(int autotest)
{
	int ret = 0;
	test_flag=VOP_COLOR_DISPLAY;
	ret = test_item_ctrl(&socle_viop_format_container, autotest);
	return ret;
}

extern int
socle_viop_ntsc_test(int autotest)
{
	int ret = 0;
	format=FORMAT_NTSC;
	ret = test_item_ctrl(&socle_viop_size_container, autotest);
	return ret;
}

extern int
socle_viop_pal_test(int autotest)
{
	int ret = 0;
	format=FORMAT_PAL;
	ret = test_item_ctrl(&socle_viop_size_container, autotest);
	return ret;
}

extern int
socle_viop_qvga_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_QVGA;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
                case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
        }
	return ret;
}

extern int
socle_viop_vga_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_VGA;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int
socle_viop_d1_test(int autotest)
{
	int ret = 0;
	size=FRAMESIZE_D1;
	switch(test_flag) {
		case VIP_TO_VOP:
			ret=socle_vip_to_vop_run();
			break;
		case VIP_TO_LCD:
			ret=test_item_ctrl(&socle_vip_42x_container, autotest);
			break;
		case VOP_COLOR_DISPLAY:
			ret=socle_vop_color_display_run();
			break;
	}
	return ret;
}

extern int socle_vip_422_test(int autotest)
{
	int ret = 0;
	output422=1;
	ret = socle_vip_to_lcd_run();
	return ret;
	
}

extern int socle_vip_420_test(int autotest)
{
	int ret = 0;
	output422=0;
	ret = socle_vip_to_lcd_run();
	return ret;
}

static int
socle_vip_to_vop_run(void)
{
	int ret=0;
	int offset=0x100000;
	vipReset();
	vopReset();
#if defined(CONFIG_LDK3V21)	
	ret |= tvp5145I2C_init();
	ret |= ch700x_init(format);
#elif defined(CONFIG_PC9223)
	ret |= mt9v125_init();
	ret |= ch7026_init(CCIR656);
#else
	printf("platform not support!!\n");
	return -1;
#endif
	ret |= vipSetOutFormat(format);
	ret |= vopSetOutFormat(format);
	ret |= vipSetFrameSize(size);
	ret |= vopSetFrameSize(size);
	ret |= vipSetFrameCaptureAddr(FRAME1, FRAME1_ADDR, FRAME1_ADDR+offset, FRAME1_ADDR+2*offset);
	ret |= vipSetFrameCaptureAddr(FRAME2, FRAME2_ADDR, FRAME2_ADDR+offset, FRAME2_ADDR+2*offset);
	ret |= vopSetFrameDisplayAddr(FRAME1, FRAME1_ADDR, FRAME1_ADDR+offset, FRAME1_ADDR+2*offset);
	ret |= vopSetFrameDisplayAddr(FRAME2, FRAME2_ADDR, FRAME2_ADDR+offset, FRAME2_ADDR+2*offset);
	ret |= vipSetFrameMode(CONTINUOUS);
	ret |= vopSetFrameMode(TWO_FRAME);
	vipSet422Output(0);
	if(ret)
		return -1;
		
	vipStart();
	vopStart();
	
	return ret;
}

static int
socle_vip_to_lcd_run(void)
{
	int ret=0;
	struct screen_info screen;
	struct mem_info mem;
	int vip_xres,vip_yres;
	char *vip_y, *vip_u, *vip_v;
#if defined(CONFIG_MDK3D)
	u32 *fb_rgb;
	int i,j;
#elif defined(CONFIG_PC9223)
	char *fb_y, *fb_u, *fb_v;
	int i;
#else
	printf("platform not support!!\n");
	return -1;
#endif
	
  ret |= mt9v125_init();
	
	switch(size) {
		case FRAMESIZE_QVGA:
			vip_xres=320;
			vip_yres=240;
			break;
		case FRAMESIZE_VGA:
			vip_xres=640;
			vip_yres=480;
			break;
		case FRAMESIZE_D1:
			vip_xres=720;
			vip_yres=480;
			break;
		default:
			printf("size not support");
			return -1;
	}
	
	vip_y=(char *)FRAME1_ADDR;
	vip_u=(char *)(FRAME1_ADDR+vip_xres*vip_yres);
	if(output422)
		vip_v=(char *)(FRAME1_ADDR+vip_xres*vip_yres+vip_xres*vip_yres/2);
	else
		vip_v=(char *)(FRAME1_ADDR+vip_xres*vip_yres+vip_xres*vip_yres/4);
	
	vipReset();
	ret |= vipSetOutFormat(format);
	ret |= vipSetFrameSize(size);
	ret |= vipSetFrameCaptureAddr(FRAME1, FRAME1_ADDR, (u32)vip_u, (u32)vip_v);
	ret |= vipSetFrameMode(ONE_FRAME);
	ret |= vipSet422Output(output422);
	vip_irq_flag=0;

	vipStart();
	while(vip_irq_flag==0)
		ret=1;
	ret=0;
	vipStop();
	
#if defined(CONFIG_MDK3D)
	fb_init(RGB888);
	fb_get_screen_info(&screen);
	fb_get_mem_info(&mem);
	fb_rgb=(u32 *)mem.page0_addr;
	
	if(vip_xres>screen.xres || vip_yres > screen.yres) {
		printf("input size is larger than screen\n");
		return -1;
	}
		
	for(i=0;i<(screen.xres*screen.yres);i++)
			fb_rgb[i]=0;
	
	for(i=0;i<vip_yres;i++) {
		for(j=0;j<vip_xres;j++) {
			socle_yuv2rgb(*vip_y, *vip_u, *vip_v, fb_rgb++);
			vip_y++;
			if(output422) {
				if(j&0x1) {
					vip_u++;
					vip_v++;
				}
			}
			else {
				if(!((j+1)%4)) {
					vip_u++;
					vip_v++;
				}
			}
		}
		fb_rgb+=(screen.xres-vip_xres);
	}	
#elif defined(CONFIG_PC9223)
	if(output422)
		fb_init(YUV422);
	else
		fb_init(YUV420);
	fb_get_screen_info(&screen);
	fb_get_mem_info(&mem);
	
	if(vip_xres>screen.xres || vip_yres > screen.yres) {
		printf("input size is larger than screen\n");
		return -1;
	}
	
	fb_y=(char *)mem.page0_addr;
	fb_u=(char *)(mem.page0_addr+screen.xres*screen.yres);
	if(output422) {
		fb_v=(char *)(mem.page0_addr+screen.xres*screen.yres+screen.xres*screen.yres/2);
		for(i=0;i<vip_yres;i++) {
			memcpy(fb_y, vip_y, vip_xres);
			memcpy(fb_u, vip_u, vip_xres/2);
			memcpy(fb_v, vip_v, vip_xres/2);
			
			fb_y += screen.xres;
			fb_u += screen.xres/2;
			fb_v += screen.xres/2;
			vip_y += vip_xres;
			vip_u += vip_xres/2;
			vip_v += vip_xres/2;	
		}
	}
	else {
		fb_v=(char *)(mem.page0_addr+screen.xres*screen.yres+screen.xres*screen.yres/4);
		for(i=0;i<vip_yres;i++) {
			memcpy(fb_y, vip_y, vip_xres);
			memcpy(fb_u, vip_u, vip_xres/4);
			memcpy(fb_v, vip_v, vip_xres/4);
			
			fb_y += screen.xres;
			fb_u += screen.xres/4;
			fb_v += screen.xres/4;
			vip_y += vip_xres;
			vip_u += vip_xres/4;
			vip_v += vip_xres/4;	
		}
	}
#else
	printf("platform not support!!\n");
	return -1;
#endif
	fb_enable();
	return ret;
}

static int
socle_vop_color_display_run(void)
{
	int ret=0,vop_xres,vop_yres,offset;
	char buf;
	struct color_info cinfo;
	
	vopReset();
	
#if defined(CONFIG_PC9220) ||  defined(CONFIG_LDK3V21)
	ret |= ch700x_init(format);
#elif defined(CONFIG_PC9223)
	ret |= ch7026_init(CCIR656);
#else
	printf("platform not support!!\n");
	return -1;
#endif

	switch(size) {
		case FRAMESIZE_QVGA:
			vop_xres=320;
			vop_yres=240;
			break;
		case FRAMESIZE_VGA:
			vop_xres=640;
			vop_yres=480;
			break;
		case FRAMESIZE_D1:
			vop_xres=720;
			vop_yres=480;
			break;
		default:
			printf("size not support");
			return -1;
	}
	offset=vop_xres*vop_yres;
	cinfo.xres=vop_xres;
	cinfo.yres=vop_yres;
	cinfo.color=COLORBAR;
	cinfo.mode=YUV420;
	cinfo.addr=FRAME1_ADDR;
	socle_color_fill(&cinfo);
	
	ret |= vopSetOutFormat(format);
	ret |= vopSetFrameSize(size);
	ret |= vopSetFrameDisplayAddr(FRAME1, FRAME1_ADDR, FRAME1_ADDR+offset, FRAME1_ADDR+offset+offset/4);
	ret |= vopSetFrameMode(ONE_FRAME);
	if(ret)
		return -1;
	vopStart();
	printf("=================\n");
	printf("Displaying...\n");
	printf("Press enter to stop display...\n");
	scanf("%c",&buf);
	if(buf){
		vopStop();
	}
	return ret;
}
