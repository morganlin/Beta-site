
#include "i2c.h"
#include "dependency.h"

#include "cat6023.h"

struct i2c_client cat6023_client;

static int inline
cat6023_write(u8 reg, u8 val)
{
	int ret;
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;
	ret = i2c_master_send(&cat6023_client, (char *)buf, 2);
	if (ret != 2)
		return -1;
	else
		return 0;
}

static int inline
cat6023_read(u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf;
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = cat6023_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	
	msg[1].addr = cat6023_client.addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = &ret_buf;
	msg[1].len = 1;
	ret = i2c_transfer(msg, 2);
	if (ret != 2)
		return -1;
	ret_val = ret_buf;
	return ret_val;
}



extern int cat6023_init(u8 format)
{
	i2c_master_initialize(SOCLE_I2C0, SOCLE_INTC_I2C0);
	cat6023_client.addr = 0x48;
	
	printf("devid=%x%x\n",cat6023_read(0x03),cat6023_read(0x02));
	
	//cat6023_write(0x89,0x3f);
	//cat6023_write(0x1d,0x70);
	if (format == EMB_YUV) {
		cat6023_write(0x3d,0x40); //set yuv422 output
		//cat6023_write(0x1C,0x10); //EnUdFilt
		cat6023_write(0x1c,0x40); //emb sync
		cat6023_write(0x20,0x12); //RGB2YUV

		cat6023_write(0x24,0xB2); //RGB2YUV
		cat6023_write(0x25,0x04); //RGB2YUV
		cat6023_write(0x26,0x64); //RGB2YUV
		cat6023_write(0x27,0x02); //RGB2YUV
		cat6023_write(0x28,0xE9); //RGB2YUV
		cat6023_write(0x29,0x00); //RGB2YUV
		cat6023_write(0x2A,0x93); //RGB2YUV
		cat6023_write(0x2B,0x3C); //RGB2YUV
		cat6023_write(0x2C,0x16); //RGB2YUV
		cat6023_write(0x2D,0x04); //RGB2YUV
		cat6023_write(0x2E,0x56); //RGB2YUV
		cat6023_write(0x2F,0x3F); //RGB2YUV
		cat6023_write(0x30,0x49); //RGB2YUV
		cat6023_write(0x31,0x3D); //RGB2YUV
		cat6023_write(0x32,0x9F); //RGB2YUV
		cat6023_write(0x33,0x3E); //RGB2YUV
		cat6023_write(0x34,0x16); //RGB2YUV
		cat6023_write(0x35,0x04); //RGB2YUV
	} else if (format == EMB_YUV656) {
		cat6023_write(0x3d,0x40); //set yuv422 output
		cat6023_write(0x1c,0x44); //emb 656
		cat6023_write(0x20,0x12); //RGB2YUV

		cat6023_write(0x24,0xB2); //RGB2YUV
		cat6023_write(0x25,0x04); //RGB2YUV
		cat6023_write(0x26,0x64); //RGB2YUV
		cat6023_write(0x27,0x02); //RGB2YUV
		cat6023_write(0x28,0xE9); //RGB2YUV
		cat6023_write(0x29,0x00); //RGB2YUV
		cat6023_write(0x2A,0x93); //RGB2YUV
		cat6023_write(0x2B,0x3C); //RGB2YUV
		cat6023_write(0x2C,0x16); //RGB2YUV
		cat6023_write(0x2D,0x04); //RGB2YUV
		cat6023_write(0x2E,0x56); //RGB2YUV
		cat6023_write(0x2F,0x3F); //RGB2YUV
		cat6023_write(0x30,0x49); //RGB2YUV
		cat6023_write(0x31,0x3D); //RGB2YUV
		cat6023_write(0x32,0x9F); //RGB2YUV
		cat6023_write(0x33,0x3E); //RGB2YUV
		cat6023_write(0x34,0x16); //RGB2YUV
		cat6023_write(0x35,0x04); //RGB2YUV
	} else if (format == EXT_BGR) {
		cat6023_write(0x3d,0x00); //set rgb output
	} else {
		printf("Not support this format! (%d)\n", format);
		return -1;
	}

	printf("0x3d=%x\n",cat6023_read(0x3d));

	return 0;
}
