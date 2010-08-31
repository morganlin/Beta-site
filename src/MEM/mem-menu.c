#include <test_item.h>
#include "dependency.h"
#include "mem-ctrl.h"
#include "mem-menu.h"

struct test_item mem_device_select_items[] =
{
    {
        "SDRAM Test",
        sdram_test,
        1,
        1
    },
    {
        "ITCM0 Test",
        itcm_test0,
        1,
        ITCM_Enable_Disable
    },
    {
        "ITCM1 Test",
        itcm_test1,
        1,
        ITCM1_ENABLE
    },
    {
        "DTCM0 Test",
        dtcm_test0,
        1,
        DTCM_Enable_Disable
    },
    {
        "DTCM1 Test",
        dtcm_test1,
        1,
        DTCM1_ENABLE
    },
    {
        "Another DTCM Test",
        dtcm_test0,
        1,
        ANOTHER_DTCM_Enable_Disable
    },
    {
        "FPGA SRAM Test",
        fpga_sram_test,
        1,
        FPGA_Enable_Disable
    },
    {
        "MDDR Test",
        mddr_test,
        1,
        MDDR_Enable_Disable
    },
    {
        "SRAM Test",
        sram_test,
        1,
        SRAM_Enable_Disable
    },
    {
        "DP-SRAM Test",
        dp_sram_test,
        1,
        DP_SRAM_Enable_Disable
    },
#ifdef CONFIG_MDK3D
	{
		"MDK3D-FPGA Test",
		mdk3d_fpga_test,
		0,
		1
	},
#endif
};

struct test_item_container mem_device_select_container =
{
    .menu_name = "Memory select Menu",
    .shell_name = "Memory select",
    .items = mem_device_select_items,
    .test_item_size = sizeof(mem_device_select_items)
};

struct test_item mem_fpga_test_items[] =
{
	{
		"AXI Test",
		mem_fpga_axi_test,
		1,
		1
	},
	{
		"AHB Test",
		mem_fpga_ahb_test,
		1,
		1
	},

};

struct test_item_container mem_fpga_container =
{
	.menu_name = "Memory MDK3D FPGA Test",
	.shell_name = "FPGA Test",
	.items = mem_fpga_test_items,
	.test_item_size = sizeof(mem_fpga_test_items)
};

struct test_item mem_fpga_mode_items[] =
{
	{
		"DDR2 Test",
		mem_fpga_ddr2_test,
		1,
		1
	},
	{
		"SRAM Test",
		mem_fpga_sram_test,
		1,
		1
	},

};

struct test_item_container mem_fpga_mode_container =
{
	.menu_name = "Memory MDK3D FPGA Mode",
	.shell_name = "FPGA Mode",
	.items = mem_fpga_mode_items,
	.test_item_size = sizeof(mem_fpga_mode_items)
};

struct test_item mem_main_test_items[] =
{
    {
        "Memeory Range Test",
        mem_range_test,
        1,
        1
    },
    {
        "MDDR Power Mode",
        mddr_power_mode_test,
        0,
        MDDR_Enable_Disable
    },
    {
        "Memory Bank Test",
        mem_bank_test,
        1,
        1
    },
/* added by morganlin */    
#ifdef GDR_MEM    
    {
        "Memory Bank Align Test",
        mem_align_test,
        1,
        1
    },
#else
    {
        "Memory Bank Align Test",
        mem_align_test,
        0,
        1
    },
#endif    
    {
        "Repeat Someone Address Test",
        repeat_write_test,
        0,
        1
    },

};

struct test_item_container mem_main_container =
{
    .menu_name = "Memory Test Main Menu",
    .shell_name = "Memory",
    .items = mem_main_test_items,
    .test_item_size = sizeof(mem_main_test_items)
};

struct test_item mem_bank_test_items[] =
{
/* added by morganlin */
#ifdef GDR_MEM        
    {
        "Bank Normal Test",
        mem_bank_normal_test,
        1,
        1
    },
#else
    {
        "Bank Normal Test",
        mem_bank_normal_test,
        0,
        1
    },            
#endif
    {
        "Bank Burn in Test",
        mem_bank_burn_in_test,
        0,
        1
    }
};

struct test_item_container mem_bank_test_container =
{
    .menu_name = "Memory Bank Test Menu",
    .shell_name = "Memory Bank",
    .items = mem_bank_test_items,
    .test_item_size = sizeof(mem_bank_test_items)
};

struct test_item mem_range_test_items[] =
{
    {
        "PATTERN0 Test",
        mem_pattern0_test,
        1,
        1
    },
    {
        "PATTERN1 Test",
        mem_pattern1_test,
        1,
        1
    },
    {
        "PATTERN2 Test",
        mem_pattern2_test,
        1,
        1
    },
    {
        "PATTERN3 Test",
        mem_pattern3_test,
        1,
        1
    },
    {
        "PATTERN4 Test",
        mem_pattern4_test,
        1,
        1
    },
    {
        "PATTERN5 Test",
        mem_pattern5_test,
        1,
        1
    },
    {
        "PATTERN6 Test",
        mem_pattern6_test,
        1,
        1
    },
    {
        "PATTERN7 Test",
        mem_pattern7_test,
        1,
        1
    },
    {
        "PATTERN8 Test",
        mem_pattern8_test,
        1,
        1
    },
};

struct test_item_container mem_range_test_container =
{
    .menu_name = "Memory Range Pattern Test Menu",
    .shell_name = "Memory Patterm",
    .items = mem_range_test_items,
    .test_item_size = sizeof(mem_range_test_items)
};

struct test_item mddr_power_mode_items[] =
{
    {
        "Power Mode 1",
        mddr_power_mode_1,
        1,
        1
    },
    {
        "Power Mode 2",
        mddr_power_mode_2,
        1,
        1
    },
    {
        "Power Mode 3",
        mddr_power_mode_3,
        1,
        1
    },
    {
        "Power Mode 4",
        mddr_power_mode_4,
        1,
        1
    },
    {
        "Power Auto Mode",
        mddr_power_auto_mode,
        0,
        1
    },
};

struct test_item_container mddr_power_mode_container =
{
    .menu_name = "mDDR Power Mode Test Menu",
    .shell_name = "mDDR Power Mode",
    .items = mddr_power_mode_items,
    .test_item_size = sizeof(mddr_power_mode_items)
};
