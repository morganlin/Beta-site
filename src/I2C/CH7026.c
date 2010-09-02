#include "i2c.h"
#include "dependency.h"
#include "LCD/fb.h"

#define VGA 1
#define SDTV 2
#define CCIR656 3


struct i2c_client ch7026_client;

static int inline ch7026_read(const u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf;
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = ch7026_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	
	msg[1].addr = ch7026_client.addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = &ret_buf;
	msg[1].len = 1;
	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;
	ret_val = ret_buf;
	return ret_val;
	
}

static int inline ch7026_write(const u8 reg, const u8 val)
{
	int ret;
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;
	ret = i2c_master_send(&ch7026_client, (char *)buf, 2);
	if (ret != 2)
		return -1;
	else
		return 0;
}


extern int ch7026_init(int out)
{
	int ret=0,did,rid;
	int hti,hai,hw,ho,vti,vai,vw,vo;
	struct screen_info screen;
	
	printf("ch7026_init\n");
	i2c_master_initialize(SQ_I2C0, SQ_INTC_I2C0);
	ch7026_client.addr = 0x76;
	did=ch7026_read(0x00);
	rid=ch7026_read(0x01);	
	if(did!=0x54) {
		printf("ch7026 id wrong!! 0x%x != 0x54\n", did);
		return -1;
	}
	fb_get_screen_info(&screen);
	hti=screen.xres+screen.left_margin+screen.right_margin+screen.hsync_len;
	hai=screen.xres;
	hw=screen.hsync_len;
	ho=screen.right_margin;
	vti=screen.yres+screen.upper_margin+screen.lower_margin+screen.vsync_len;
	vai=screen.yres;
	vw=screen.vsync_len;
	vo=screen.lower_margin;
	
	if(out==VGA) {
		ch7026_write(0x02,0x01);
		ch7026_write(0x02,0x03);
		if(screen.xres>720)
			ch7026_write(0x06,0x6b);
		ch7026_write(0x07,0x24);
		ch7026_write(0x08,0x08);
		ch7026_write(0x09,0x80);
		ch7026_write(0x0c,0x80);
		ch7026_write(0x0d,0x08);
		//input timing
		ch7026_write(0x0f,(((hti>>8)&0x7)<<3) | ((hai>>8)&0x7));
		ch7026_write(0x10,hai&0xff);
		ch7026_write(0x11,hti&0xff);
		ch7026_write(0x12,(((hw>>8)&0x7)<<3) | ((ho>>8)&0x7) | 0x1<<6);
		ch7026_write(0x13,ho&0xff);
		ch7026_write(0x14,hw&0xff);
		ch7026_write(0x15,(((vti>>8)&0x7)<<3) | ((vai>>8)&0x7));
		ch7026_write(0x16,vai&0xff);
		ch7026_write(0x17,vti&0xff);
		ch7026_write(0x18,(((vw>>8)&0x7)<<3) | ((vo>>8)&0x7));
		ch7026_write(0x19,vo&0xff);
		ch7026_write(0x1a,vw&0xff);	
		//output timing
		ch7026_write(0x1B,0x23);
		ch7026_write(0x1C,0x20);
		ch7026_write(0x1D,0x20);
		ch7026_write(0x1F,0x25);
		ch7026_write(0x20,0x80);
		ch7026_write(0x21,0x12);
		ch7026_write(0x22,0x58);
		ch7026_write(0x23,0x74);
		ch7026_write(0x25,0x0A);
		ch7026_write(0x26,0x04);
		ch7026_write(0x37,0x20);
		ch7026_write(0x39,0x20);
		ch7026_write(0x3B,0x20);
		ch7026_write(0x41,0x9A);
		ch7026_write(0x4D,0x03);
		ch7026_write(0x4E,0x51);
		ch7026_write(0x4F,0x11);
		ch7026_write(0x50,0x11);
		ch7026_write(0x51,0x4B);
		ch7026_write(0x52,0x12);
		ch7026_write(0x53,0x13);
		ch7026_write(0x55,0xE5);
		ch7026_write(0x5E,0x80);
		if(screen.xres>720)
			ch7026_write(0x69,(hai/16)<<1);
		ch7026_write(0x7D,0x62);
		ch7026_write(0x04,0x00);
		if(screen.xres>720)
			ch7026_write(0x06,0x69);
		else
			ch7026_write(0x06,0x71);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		if(screen.xres>720)
			ch7026_write(0x06,0x68);
		else
			ch7026_write(0x06,0x70);
		ch7026_write(0x02,0x02);
		ch7026_write(0x02,0x03);
	}
	else if(out==SDTV) {
		ch7026_write(0x02,0x01);
		ch7026_write(0x02,0x03);
		ch7026_write(0x03,0x00);
		if(screen.xres>720)
			ch7026_write(0x06,0x6b);
		ch7026_write(0x07,0x24);
		ch7026_write(0x0c,0x80);
		//input timing
		ch7026_write(0x0f,(((hti>>8)&0x7)<<3) | ((hai>>8)&0x7));
		ch7026_write(0x10,hai&0xff);
		ch7026_write(0x11,hti&0xff);
		ch7026_write(0x12,(((hw>>8)&0x7)<<3) | ((ho>>8)&0x7) | 0x1<<6);
		ch7026_write(0x13,ho&0xff);
		ch7026_write(0x14,hw&0xff);
		ch7026_write(0x15,(((vti>>8)&0x7)<<3) | ((vai>>8)&0x7));
		ch7026_write(0x16,vai&0xff);
		ch7026_write(0x17,vti&0xff);
		ch7026_write(0x18,(((vw>>8)&0x7)<<3) | ((vo>>8)&0x7));
		ch7026_write(0x19,vo&0xff);
		ch7026_write(0x1a,vw&0xff);	
	
		ch7026_write(0x41,0x9A);
		ch7026_write(0x4D,0x04);
		ch7026_write(0x4E,0x80);
		ch7026_write(0x51,0x4B);
		ch7026_write(0x52,0x12);
		ch7026_write(0x53,0x1b);
		ch7026_write(0x55,0xE5);
		ch7026_write(0x5E,0x80);
		if(screen.xres>720)
			ch7026_write(0x69,(hai/16)<<1);
		ch7026_write(0x7D,0x62);
		ch7026_write(0x04,0x00);
		if(screen.xres>720)
			ch7026_write(0x06,0x69);
		else
			ch7026_write(0x06,0x71);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		ch7026_write(0x03,0x00);
		if(screen.xres>720)
			ch7026_write(0x06,0x68);
		else
			ch7026_write(0x06,0x70);
		ch7026_write(0x02,0x02);
		ch7026_write(0x02,0x03);
	}
	else if(out==CCIR656) {
#if 0  //use crystal out
	 ch7026_write( 0x02, 0x01 );
	 ch7026_write( 0x02, 0x03 );
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x09, 0x04 );
	 ch7026_write( 0x0B, 0x84 );
	 ch7026_write( 0x0C, 0x05 );
	 ch7026_write( 0x0D, 0x20 );
	 ch7026_write( 0x12, 0x40 );
	 ch7026_write( 0x15, 0x48 );
	 ch7026_write( 0x41, 0x9A );
	 ch7026_write( 0x4D, 0x04 );
	 ch7026_write( 0x4E, 0x80 );
	 ch7026_write( 0x51, 0x4B );
	 ch7026_write( 0x52, 0x12 );
	 ch7026_write( 0x53, 0x1B );
	 ch7026_write( 0x55, 0xFD );
	 ch7026_write( 0x5E, 0x80 );
	 ch7026_write( 0x7D, 0x62 );
	 ch7026_write( 0x04, 0x00 );
	 ch7026_write( 0x06, 0x71 );
#else  //use gclk out
	 ch7026_write( 0x02, 0x01 );
	 ch7026_write( 0x02, 0x03 );
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x09, 0x04 );
	 ch7026_write( 0x0B, 0x84 );
	 ch7026_write( 0x0C, 0x05 );
	 ch7026_write( 0x0D, 0x20 );
	 ch7026_write( 0x12, 0x40 );
	 ch7026_write( 0x15, 0x48 );
	 ch7026_write( 0x4D, 0x03 );
	 ch7026_write( 0x51, 0x70 );
	 ch7026_write( 0x55, 0xFC );
	 ch7026_write( 0x5E, 0x80 );
	 ch7026_write( 0x7D, 0x62 );
	 ch7026_write( 0x04, 0x00 );
	 ch7026_write( 0x06, 0x71 );

#endif
/*
NOTE: The following five repeated sentences are used here to wait memory initial complete, please don't remove...(you could refer to Appendix A of programming guide document (CH7025(26)B Programming Guide Rev2.03.pdf) for detailed information about memory initialization! 
*/
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x03, 0x00 );
	 ch7026_write( 0x03, 0x00 );
	
	 ch7026_write( 0x06, 0x70 );
	 ch7026_write( 0x02, 0x02 );
	 ch7026_write( 0x02, 0x03 );
	}
	else {
		printf("wrong output format!\n");
		return -1;
	}
	
	return ret;
}
