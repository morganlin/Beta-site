#include <test_item.h>

extern int adc_individual_channel_test(int autotest);
extern int adc_all_channel_test(int autotest);
extern int adc_touch_screen_test(int autotest);

struct test_item adc_main_test_items[] = {
	{
		"ADC individual channel test",
		adc_individual_channel_test,
		1,
		1
	},
	{
		"ADC all channel test",
		adc_all_channel_test,
		1,
		1
	},
	{
		"ADC Touch Screen test",
		adc_touch_screen_test,
		1,
		0
	},
};

struct test_item_container adc_main_container = {
     .menu_name = "ADC Test Main Menu",
     .shell_name = "ADC Main",
     .items = adc_main_test_items,
     .test_item_size = sizeof(adc_main_test_items)
};

extern int adc_polling_test(int autotest);
extern int adc_interrupt_test(int autotest);

struct test_item adc_mode_test_items[] = {
	{
		"ADC with polling mode",
		adc_polling_test,
		1,
		1
	},
	{
		"ADC with interrupt",
		adc_interrupt_test,
		1,
		1
	},
};

struct test_item_container adc_mode_container = {
     .menu_name = "ADC Mode Menu",
     .shell_name = "ADC Mode",
     .items = adc_mode_test_items,
     .test_item_size = sizeof(adc_mode_test_items)
};

extern int adc_channel_0_test(int autotest);
extern int adc_channel_1_test(int autotest);
extern int adc_channel_2_test(int autotest);
extern int adc_channel_3_test(int autotest);
extern int adc_channel_4_test(int autotest);
extern int adc_channel_5_test(int autotest);
extern int adc_channel_6_test(int autotest);
extern int adc_channel_7_test(int autotest);

struct test_item adc_channel_test_items[] = {
	{
		"channel 0 test",
		adc_channel_0_test,
		1,
		1
	},
	{
		"channel 1 test",
		adc_channel_1_test,
		1,
		1
	},
	{
		"channel 2 test",
		adc_channel_2_test,
		1,
		1
	},
	{
		"channel 3 test",
		adc_channel_3_test,
		1,
		1
	},
	{
		"channel 4 test",
		adc_channel_4_test,
		1,
		1
	},
	{
		"channel 5 test",
		adc_channel_5_test,
		1,
		1
	},
	{
		"channel 6 test",
		adc_channel_6_test,
		1,
		1
	},
	{
		"channel 7 test",
		adc_channel_7_test,
		1,
		1
	},
};

struct test_item_container adc_channel_container = {
     .menu_name = "ADC Select Channel",
     .shell_name = "ADC channel",
     .items = adc_channel_test_items,
     .test_item_size = sizeof(adc_channel_test_items)
};

