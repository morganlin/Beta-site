#include <test_item.h>

extern int irq_test_mode_test(int autotest);
extern int fiq_test_mode_test(int autotest);

struct test_item interrupt_main_test_items[] = {
	{
		"Local Interrupt to Local CPU Test (IRQ)",
		irq_test_mode_test,
		1,
		1
	},

	{
		"Local Interrupt to Local CPU Test (FIQ)",
		fiq_test_mode_test,
		0,
		0
	},

};

struct test_item_container interrupt_main_container = {
     .menu_name = "Interrupt Test Main Menu",
     .shell_name = "Interrupt",
     .items = interrupt_main_test_items,
     .test_item_size = sizeof(interrupt_main_test_items)
};


