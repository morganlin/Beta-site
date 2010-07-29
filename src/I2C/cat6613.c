
#include "i2c.h"
#include "dependency.h"



struct i2c_client cat6613_client;

static int inline
cat6613_write(u8 reg, u8 val)
{
	int ret;
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;
	ret = i2c_master_send(&cat6613_client, (char *)buf, 2);
	if (ret != 2)
		return -1;
	else
		return 0;
}

static int inline
cat6613_read(u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf;
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = cat6613_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	
	msg[1].addr = cat6613_client.addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = &ret_buf;
	msg[1].len = 1;
	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;
	ret_val = ret_buf;
	return ret_val;
}



extern int cat6613_init(void)
{
	int ret=0;
	u8 buf[3];
	i2c_master_initialize(SOCLE_I2C0, SOCLE_INTC_I2C0);
	cat6613_client.addr = 0x4d;
	
	buf[0]=cat6613_read(0x00);
	buf[1]=cat6613_read(0x01);
	buf[2]=cat6613_read(0x02);
	buf[3]=cat6613_read(0x03);
	
	printf("vid=%x %x did=%x %x\n",buf[0],buf[1],buf[2],buf[3]);
	
		//reset
		cat6613_write( 0x0F , 0x00);
		cat6613_write( 0x04 , 0x3d);
		MSDELAY(2);
		cat6613_write( 0x04 , 0x1d);
		//set init int
		//cat6613_write(0x09,0xfc);
		//cat6613_write(0x0a,0xf8);
		//cat6613_write(0x0b,0xf7);
		//disable out
		cat6613_write(0xc0, 0x00);
		cat6613_write(0xc1, 0x03);
   
   cat6613_write( 0x04 , 0x14);
   MSDELAY(2);
   cat6613_write( 0x04 , 0x00);
   
   cat6613_write(0x61,0x03);
	cat6613_write(0x62,0x18);
	cat6613_write(0x63,0x10);
	cat6613_write(0x64,0x04);
	
   
   
	cat6613_write(0x90,0x6);
	cat6613_write(0x72,0x80);
	cat6613_write(0xc1,0);
	cat6613_write(0xc6,0x3);
	
	return ret;
}
