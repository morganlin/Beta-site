#include <test_item.h>
//#include "dependency.h"

extern int sq_spi_max1110_input_pwm_test(int autotest);
extern int sq_spi_max1110_input_ref_test(int autotest);

struct test_item sq_spi_max1110_items[] = {
	{
		"PWM Input Test",
		sq_spi_max1110_input_pwm_test,
		1,
		1
	},
	{
		"Ref3.3V Input Test",
		sq_spi_max1110_input_ref_test,
		0,
		1
	}
};

struct test_item_container sq_spi_max1110_test_container = {
	.menu_name = "SPI MAX1110 Test",
	.shell_name = "MAX1110",
	.items = sq_spi_max1110_items,
	.test_item_size = sizeof(sq_spi_max1110_items)
};


extern int sq_spi_max1110_sgl_test(int autotest);
extern int sq_spi_max1110_dif_test(int autotest);

struct test_item sq_spi_max1110_input_items[] = {
	{
		"Single ended Test",
		sq_spi_max1110_sgl_test,
		1,
		1
	},
	{
		"Differential Test",
		sq_spi_max1110_dif_test,
		1,
		1
	}
};

struct test_item_container sq_spi_max1110_input_container = {
	.menu_name = "MAX1110 Input Test",
	.shell_name = "Input",
	.items = sq_spi_max1110_input_items,
	.test_item_size = sizeof(sq_spi_max1110_input_items)
};


extern int sq_spi_max1110_ch0_test(int autotest);
extern int sq_spi_max1110_ch1_test(int autotest);
extern int sq_spi_max1110_ch2_test(int autotest);
extern int sq_spi_max1110_ch3_test(int autotest);
extern int sq_spi_max1110_ch4_test(int autotest);
extern int sq_spi_max1110_ch5_test(int autotest);
extern int sq_spi_max1110_ch6_test(int autotest);
extern int sq_spi_max1110_ch7_test(int autotest);

struct test_item sq_spi_max1110_sgl_items[] = {
	{
		"CH0 Test",
		sq_spi_max1110_ch0_test,
		1,
		1
	},
	{
		"CH1 Test",
		sq_spi_max1110_ch1_test,
		1,
		1
	},
	{
		"CH2 Test",
		sq_spi_max1110_ch2_test,
		1,
		1
	},
	{
		"CH3 Test",
		sq_spi_max1110_ch3_test,
		1,
		1
	},
	{
		"CH4 Test",
		sq_spi_max1110_ch4_test,
		1,
		1
	},
	{
		"CH5 Test",
		sq_spi_max1110_ch5_test,
		1,
		1
	},
	{
		"CH6 Test",
		sq_spi_max1110_ch6_test,
		1,
		1
	},
	{
		"CH7 Test",
		sq_spi_max1110_ch7_test,
		1,
		1
	}
};

struct test_item_container sq_spi_max1110_sgl_container = {
	.menu_name = "MAX1110 SGL Test",
	.shell_name = "SGL",
	.items = sq_spi_max1110_sgl_items,
	.test_item_size = sizeof(sq_spi_max1110_sgl_items)
};


extern int sq_spi_max1110_ch0_ch1_test(int autotest);
extern int sq_spi_max1110_ch1_ch0_test(int autotest);
extern int sq_spi_max1110_ch2_ch3_test(int autotest);
extern int sq_spi_max1110_ch3_ch2_test(int autotest);
extern int sq_spi_max1110_ch4_ch5_test(int autotest);
extern int sq_spi_max1110_ch5_ch4_test(int autotest);
extern int sq_spi_max1110_ch6_ch7_test(int autotest);
extern int sq_spi_max1110_ch7_ch6_test(int autotest);

struct test_item sq_spi_max1110_dif_items[] = {
	{
		"CH0+ ch1- Test",
		sq_spi_max1110_ch0_ch1_test,
		1,
		1
	},
	{
		"CH1+ ch0- Test",
		sq_spi_max1110_ch1_ch0_test,
		1,
		1
	},
	{
		"CH2+ ch3- Test",
		sq_spi_max1110_ch2_ch3_test,
		1,
		1
	},
	{
		"CH3+ ch2- Test",
		sq_spi_max1110_ch3_ch2_test,
		1,
		1
	},
	{
		"CH4+ ch5- Test",
		sq_spi_max1110_ch4_ch5_test,
		1,
		1
	},
	{
		"CH5+ ch4- Test",
		sq_spi_max1110_ch5_ch4_test,
		1,
		1
	},
	{
		"CH6+ ch7- Test",
		sq_spi_max1110_ch6_ch7_test,
		1,
		1
	},
	{
		"CH7+ ch6- Test",
		sq_spi_max1110_ch7_ch6_test,
		1,
		1
	}
};

struct test_item_container sq_spi_max1110_dif_container = {
	.menu_name = "MAX1110 DIF Test",
	.shell_name = "DIF",
	.items = sq_spi_max1110_dif_items,
	.test_item_size = sizeof(sq_spi_max1110_dif_items)
};

