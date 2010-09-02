#include "ms6335.h"
#include "../I2C/i2c.h"
#include "../I2C/dependency.h"

struct i2c_client ms6335_client;

static int inline
ms6335_write(u8 val)
{
	int ret;
	u8 buf;

	buf = val;

	ret = i2c_master_send(&ms6335_client, (char *)&buf, 1);
	if (ret != 1)
	{
		printf("ms6335_write error ret = %x \n",ret);
		return -1;
	}
	else
		return 0;
}

extern int 
ms6335_dac_master_volume(int val)
{
	int ret;
	val = val & 0x1f;
	ret = ms6335_write(val);
	return ret;
}

extern int
ms6335_dac_mute(bool val)
{
	int ret;

	if (val == 1)
		ret = ms6335_write(MS6335_FUNC_PWR_MODE | MS6335_PWR_DWN_DACM);
	else 
		ret = ms6335_write(MS6335_FUNC_PWR_MODE);

	return ret;
}

extern int
ms6335_dac_initialize(void)
{
	int ret = 0;

	printf("ms6335_dac_initialize Line Out initialize\n");

	i2c_master_initialize(SQ_I2C0, SQ_INTC_I2C0);

	ms6335_client.addr = MS6335_SLAVE_ADDR_DAC;
	MSDELAY(5);
printf("@@--%s--MS6335_AUDIO_I2S_MODE\n", __func__);
	ret |= ms6335_write(MS6335_AUDIO_I2S_MODE);

printf("@@--%s--MS6335_FUNC_PWR_MODE\n", __func__);
	//0x7b
	ret |= ms6335_write(MS6335_FUNC_PWR_MODE | MS6335_PWR_DWN_CAPGD |
		MS6335_PWR_DWN_OPAPD |MS6335_PWR_DWN_DACPD |
		MS6335_PWR_DWN_HPPD |MS6335_PWR_DWN_DACM);

	//0x61
	ret |= ms6335_write(MS6335_FUNC_PWR_MODE | MS6335_PWR_DWN_DACM);

	//0x60
	ret |= ms6335_write(MS6335_FUNC_PWR_MODE);

	ret |= ms6335_write(0x0f);
	return ret;
}


