#include <platform.h>
#include <global.h>
#include <genlib.h>
#include <test_item.h>
#include "dependency.h"

u32 otg_base;
u32 otg_irq;

extern struct test_item_container otg_main_container;

extern int
otg_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&otg_main_container, autotest);
	 
	return ret;
}

extern int 
otg_host_test(int autotest)
{
	int ret = 0;

	printf("otg_host_test\n");
	//ret = test_item_ctrl(&otg_host_main_container, autotest);

        return ret;
}

extern struct test_item_container otg_channel_container;

extern int 
otg_device_test(int autotest)
{	
        int ret = 0;

	ret = test_item_ctrl(&otg_channel_container, autotest);

        return ret;
}

extern int otg_device_start(int autotest);

extern int
otg0_device_test(int autotest)
{
	int ret = 0;
	
	otg_base = SOCLE_OTG0;
	otg_irq = SOCLE_INTC_OTG0;

	printf("otg0_device_test\n");
	ret = otg_device_start(autotest);
	//ret = test_item_ctrl(&otg_device_main_container, autotest);
	return ret;
}

extern int
otg1_device_test(int autotest)
{
	int ret = 0;
	
	otg_base = SOCLE_OTG1;
	otg_irq = SOCLE_INTC_OTG1;

	printf("otg1_device_test\n");
        ret = otg_device_start(autotest);
	//ret = test_item_ctrl(&otg_device_main_container, autotest);
	return ret;
}

extern int
otg2_device_test(int autotest)
{
	int ret = 0;
	
	otg_base = SOCLE_OTG2;
	otg_irq = SOCLE_INTC_OTG2;

	printf("otg2_device_test\n");
        ret = otg_device_start(autotest);
	//ret = test_item_ctrl(&otg_device_main_container, autotest);
	return ret;
}

extern int 
otg_dule_role_test(int autotest)
{	
        int ret = 0;

	printf("otg_dule_role_test\n");
        //ret = test_item_ctrl(&otg_dule_role_main_container, autotest);

        return ret;
}


