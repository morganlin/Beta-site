#include "i2c.h"
#include "dependency.h"





struct i2c_client mt9v125_client;

static int inline mt9v125_read(const u8 reg)
{
	int ret;
	struct i2c_msg msg[2];
	u8 buf = reg;
	u8 ret_buf[2];
	int ret_val;
       
	memset((void *)msg, 0x00, 2*sizeof(struct i2c_msg));
	msg[0].addr = mt9v125_client.addr;
	msg[0].buf = &buf;
	msg[0].len = 1;
	
	msg[1].addr = mt9v125_client.addr;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = ret_buf;
	msg[1].len = 2;
	ret = i2c_transfer(msg, 2);
	if (ret != 2) {
		printf("mt9v125_read fail\n");
		return -1;
	}
	ret_val = (ret_buf[0]<<8) | ret_buf[1];
	return ret_val;
	
}

static int inline mt9v125_write(const u8 reg, const u16 val)
{
	int ret;
	u8 buf[3];
	buf[0] = reg;
	buf[1] = (val >>8) & 0xff;
	buf[2] = val & 0xff;
	ret = i2c_master_send(&mt9v125_client, (char *)buf, 3);
	if (ret != 3) {
		printf("mt9v125_write fail\n");
		return -1;
	}
	else
		return 0;
}

#if 0
static int inline ov2655_write_mask(const u8 reg,
				    const u8 value, const u8 mask)
{
	int ret = 0;

	if (mask != 0xff)
		ret = ov7620_read(reg);
	if (ret >= 0)
		ret = ov7620_write(reg, (ret & ~mask) | (value & mask));

	return ret;
}




static int ov2655_write_array(struct regval *rv)
{
	int ret;

	if (rv == NULL)
		return 0;

	while (rv->reg != 0xffff || rv->value != 0xff) {
		ret = ov2655_write(rv->reg, rv->value);
		if (ret < 0)
			return ret;
		rv++;
	}
	return 0;
}

#endif

extern int mt9v125_init(void)
{
	int ret=0;
	printf("mt9v125_init\n");
	i2c_master_initialize(SQ_I2C0, SQ_INTC_I2C0);
	mt9v125_client.addr = 0x5d;
	printf("chip id= %x\n",mt9v125_read(0));

	//for test pattern
#if 0
	mt9v125_write(0xf0,0x1);
	mt9v125_write(0x48,0x7);
#endif	
	return ret;
}

extern int mt9v125_start_capture(void)
{
	/* Enable the chip output */
	//ov7620_write_mask(OV9655_COM2, 0, OV9655_COM2_SLEEP);
	return 0;
}



