#include <test_item.h>
#include "dependency.h"

extern int otg_host_test(int autotest);
extern int otg_device_test(int autotest);
extern int otg_dule_role_test(int autotest);

struct test_item otg_main_test_items[] = {
	{
		"OTG HOST Test",
		otg_host_test,
		1,
		OTG_HOST_TEST
	},
	{
		"OTG DEVICE Test",
		otg_device_test,
		0,
		OTG_DEVICE_TEST
	},
	{
		"OTG DULE ROLE Test",
		otg_dule_role_test,
		1,
		OTG_DULE_ROLE_TEST
	},
};

struct test_item_container otg_main_container = {
     .menu_name = "OTG Test Main Menu",
     .shell_name = "OTG",
     .items = otg_main_test_items,
     .test_item_size = sizeof(otg_main_test_items)
};

extern int otg0_device_test(int autotest);
extern int otg1_device_test(int autotest);
extern int otg2_device_test(int autotest);

struct test_item otg_channel_set_items[] = {
	{
		"OTG 0 Device Test",
		otg0_device_test,
		1,
		OTG0_DEV_TEST
	},
	{
		"OTG 1 Device Test",
		otg1_device_test,
		1,
		OTG1_DEV_TEST
	},
	{
		"OTG 2 Device Test",
		otg2_device_test,
		1,
		OTG2_DEV_TEST
	},
};

struct test_item_container otg_channel_container = {
     .menu_name = "OTG Channel Select Menu",
     .shell_name = "OTG",
     .items = otg_channel_set_items,
     .test_item_size = sizeof(otg_channel_set_items)
};



