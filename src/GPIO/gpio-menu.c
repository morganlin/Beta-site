#include <test_item.h>
#include "dependency.h"

extern int sq_gpio_onboard_test(int autotest);
extern int sq_gpio_fpga_test(int autotest);

struct test_item sq_gpio_main_test_items[] = {
	{
		"General Test",
		sq_gpio_onboard_test,
		1,
		1
	},
};

struct test_item_container sq_gpio_main_container = {
     .menu_name = "Sq GPIO Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_main_test_items,
     .test_item_size = sizeof(sq_gpio_main_test_items)
};

extern int sq_gpio_test_mode(int autotest);
extern int sq_gpio_normal_mode(int autotest);

struct test_item sq_gpio_mode_test_items[] = {
	{
		"Test Mode Test",
		sq_gpio_test_mode,
		1,
		1
	},
/* added by morganlin */
#ifndef GDR_GPIO	
	{
		"Normal Mode Test",
		sq_gpio_normal_mode,
		1,
		1
	}
#endif
};

struct test_item_container sq_gpio_mode_container = {
     .menu_name = "Sq GPIO Test Mode Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_mode_test_items,
     .test_item_size = sizeof(sq_gpio_mode_test_items)
};


extern int sq_gpio_flash_leds(int autotest);
extern int sq_gpio_kpd_test(int autotest);

struct test_item sq_gpio_normal_test_items[] = {
	{
		"Flash LEDs Test",
		sq_gpio_flash_leds,
		1,
		SQ_GPIO_FLASH_LED_TEST
	},
	{
		"KeyPad Test",
		sq_gpio_kpd_test,
		0,
		SQ_GPIO_KPD_TEST
	},
};

struct test_item_container sq_gpio_normal_container = {
     .menu_name = "Sq GPIO Normal Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_normal_test_items,
     .test_item_size = sizeof(sq_gpio_normal_test_items)
};


extern int sq_gpio_without_int(int autotest);
extern int sq_gpio_with_int(int autotest);

struct test_item sq_gpio_int_test_items[] = {
	{
		"Without Interrupt Test",
		sq_gpio_without_int,
		1,
		1
	},
	{
		"With Interrupt Test",
		sq_gpio_with_int,
		1,
		SQ_GPIO_SUPPORT_INT
	}
};

struct test_item_container sq_gpio_int_main_container = {
     .menu_name = "Sq GPIO Interrupt Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_int_test_items,
     .test_item_size = sizeof(sq_gpio_int_test_items)
};



extern int sq_gpio_port_a_to_d(int autotest);
extern int sq_gpio_port_e_to_h(int autotest);
extern int sq_gpio_port_i_to_l(int autotest);
extern int sq_gpio_port_m_to_p(int autotest);

struct test_item sq_gpio_rw_test_items[] = {
	{
		"Port A ~ D",
		sq_gpio_port_a_to_d,
		1,
		1
	},
	{
		"Port E ~ H",
		sq_gpio_port_e_to_h,
		1,
		SQ_GPIO_GP1
	},
	{
		"Port I ~ L",
		sq_gpio_port_i_to_l,
		1,
		SQ_GPIO_GP2
	},
	{
		"Port M ~ P",
		sq_gpio_port_m_to_p,
		1,
		SQ_GPIO_GP3
	},
};

struct test_item_container sq_gpio_rw_test_container = {
     .menu_name = "Sq GPIO Read Write Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_rw_test_items,
     .test_item_size = sizeof(sq_gpio_rw_test_items)
};



extern int sq_gpio_level_sense(int autotest);
extern int sq_gpio_edge_sense(int autotest);

struct test_item sq_gpio_sense_test_items[] = {
	{
		"Level Sensitive",
		sq_gpio_level_sense,
		1,
		1
	},
	{
		"Edge Sensitive",
		sq_gpio_edge_sense,
		1,
		1
	}
};

struct test_item_container sq_gpio_sense_container = {
     .menu_name = "Sq GPIO Sense Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_sense_test_items,
     .test_item_size = sizeof(sq_gpio_sense_test_items)
};


extern int sq_gpio_lo_event(int autotest);
extern int sq_gpio_hi_event(int autotest);

struct test_item sq_gpio_event_test_items[] = {
	{
		"Falling Edge or Low Level",
		sq_gpio_lo_event,
		1,
		1
	},
	{
		"Rising Edge or High Level",
		sq_gpio_hi_event,
		1,
		1
	}
};

struct test_item_container sq_gpio_event_container = {
     .menu_name = "Sq GPIO Event Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_event_test_items,
     .test_item_size = sizeof(sq_gpio_event_test_items)
};


extern int sq_gpio_single_edge(int autotest);
extern int sq_gpio_both_edge(int autotest);

struct test_item sq_gpio_edge_test_items[] = {
	{
		"Single Edge",
		sq_gpio_single_edge,
		1,
		1
	},
	{
		"Both Edge",
		sq_gpio_both_edge,
		1,
		1
	}
};

struct test_item_container sq_gpio_edge_container = {
     .menu_name = "Sq GPIO Event Test Main Menu",
     .shell_name = "GPIO",
     .items = sq_gpio_edge_test_items,
     .test_item_size = sizeof(sq_gpio_edge_test_items)
};


