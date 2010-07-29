#include <test_item.h>
#include "macctrl.h"
#include "gMAC.h"

struct test_item mac_test_items[] = {
	{"MAC 0 test", 
	 mac_0_test,
	 1,
	 1},
	{"MAC 1 test", 
	 mac_1_test,
	 1,
	 MAC_1_test},
};

struct test_item_container mac_main_container = {
	.shell_name = "mac test",
	.items = mac_test_items,
	.test_item_size = sizeof(mac_test_items)
};

struct test_item mac_mode_test_items[] = {
	{"Normal test", 
	 mac_normal_test,
	 1,
	 1},
	{"PHY clock test", 
	 mac_phy_clock_test,
	 0,
	 1},
};

struct test_item_container mac_mode_container = {
	.shell_name = "mac mode test",
	.items = mac_mode_test_items,
	.test_item_size = sizeof(mac_mode_test_items)
};

struct test_item mac_phy_test_items[] = {
	{"MAC PHY clock on test", 
	 mac_phy_clock_on_test,
	 0,
	 1},
	{"MAC PHY clock off test", 
	 mac_phy_clock_off_test,
	 0,
	 1},
};

struct test_item_container mac_phy_container = {
	.shell_name = "mac phy clock test",
	.items = mac_phy_test_items,
	.test_item_size = sizeof(mac_phy_test_items)
};
