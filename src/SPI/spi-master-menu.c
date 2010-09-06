#include <test_item.h>
#include "dependency.h"

extern int sq_spi_master_test(int autotest);
extern int sq_spi_slave_test(int autotest);

struct test_item sq_spi_test_items[] = {
	{
		"SPI Master Test",
		sq_spi_master_test,
		1,
		1
	},
	{
		"SPI Slave Test",
		sq_spi_slave_test,
		0,
		SPI_SLAVE_TEST
	}
};

struct test_item_container sq_spi_test_container = {
	.menu_name = "SPI Test",
	.shell_name = "SPI",
	.items = sq_spi_test_items,
	.test_item_size = sizeof(sq_spi_test_items)
};

extern int sq_spi_master0_test(int autotest);
extern int sq_spi_master1_test(int autotest);

struct test_item sq_spi_master_test_items[] = {
	{
		"SPI 0 Test",
		sq_spi_master0_test,
		1,
		1
	},
	{
		"SPI 1 Test",
		sq_spi_master1_test,
		1,
		SQ_SPI_IP_1_TEST
	}
};

struct test_item_container sq_spi_master_test_container = {
	.menu_name = "SPI Master Test",
	.shell_name = "SPI",
	.items = sq_spi_master_test_items,
	.test_item_size = sizeof(sq_spi_master_test_items)
};

extern int sq_spi_internal_test(int autotest);
extern int sq_spi_eeprom(int autotest);
extern int sq_spi_marvel_wifi(int autotest);
extern int sq_spi_tsc2000_touch(int autotest);
extern int sq_spi_sq_slave_test(int autotest);

extern int sq_spi_ads7846_touch(int autotest);
extern int sq_spi_max1110_adc(int autotest);

struct test_item sq_spi_transfer_test_items[] = {
	{	
		"Internal Test",
		sq_spi_internal_test,
		1,
		1
	}, 
	{
		"With EEPROM Test",
		sq_spi_eeprom,
		1,
		1
	},
/* added by morganlin */	
#ifdef GDR_SPI	
        {
                "With Marvel WiFi Module Test",
                sq_spi_marvel_wifi,
                0,
                0
        },
#else
	{
		"With Marvel WiFi Module Test",
	 	sq_spi_marvel_wifi,
	 	1,
	 	1
	},
#endif

	{
		"With TCS2000 Touch Screen Test",
		 sq_spi_tsc2000_touch,
	 	0,
	 	1
	},
/* added by morganlin */
#ifdef GDR_SPI
        {
                "With ADS7846 Touch Screen Test",
                sq_spi_ads7846_touch,
                0,
                0
        },
#else
	{
		"With ADS7846 Touch Screen Test",
		sq_spi_ads7846_touch,
		0,
		1
	},
#endif	
	
	{
		"With MAX1110 ADC Test",
		sq_spi_max1110_adc,
	 	0,
	 	1
	},
	{
		"With Sq Slave Test",
		sq_spi_sq_slave_test,
		0,
		SPI_SLAVE_TEST
	}
};

struct test_item_container sq_spi_transfer_test_container = {
	.menu_name = "SPI Transfer Test",
	.shell_name = "SPI",
	.items = sq_spi_transfer_test_items,
	.test_item_size = sizeof(sq_spi_transfer_test_items)
};

extern int sq_spi_internal_normal_run(int auotest);
extern int sq_spi_internal_hdma_test(int auotest);

struct test_item sq_spi_internal_test_items[] = {
	{
		"Without HDMA Test",
	 	sq_spi_internal_normal_run,
	 	1,
	 	1
	},
	{
		"With HDMA Test",
	 	sq_spi_internal_hdma_test,
	 	1,
	 	SQ_SPI_HWDMA_PANTHER7_HDMA_TEST
	}
};

struct test_item_container sq_spi_internal_test_container = {
	.menu_name = "SPI Internal Test",
	.shell_name = "SPI",
	.items = sq_spi_internal_test_items,
	.test_item_size = sizeof(sq_spi_internal_test_items)
};

extern int sq_spi_hdma_burst_single_test(int autotest);
extern int sq_spi_hdma_burst_incr4_test(int autotest);

struct test_item sq_spi_hdma_burst_type_test_items[] = {
	{
		"Single",
	 	sq_spi_hdma_burst_single_test,
	 	1,
	 	1
	},
	{
		"INCR4",
	 	sq_spi_hdma_burst_incr4_test,
	 	1,
	 	1
	}
};

struct test_item_container sq_spi_hdma_burst_type_test_container = {
	.menu_name = "HDMA Burst Type",
	.shell_name = "SPI",
	.items = sq_spi_hdma_burst_type_test_items,
	.test_item_size = sizeof(sq_spi_hdma_burst_type_test_items)
};

extern int sq_spi_master_slave_protocol_test(int autotest);
extern int sq_spi_master_slave_pure_test(int autotest);
extern int sq_spi_master_slave_reg_test(int autotest);

struct test_item sq_spi_master_slave_model_test_items[] = {
	{
		"SW Protocol Test",
	 	sq_spi_master_slave_protocol_test,
	 	0,
	 	1
	},
	{
		"Pure TxRx Test",
	 	sq_spi_master_slave_pure_test,
	 	0,
	 	1
	},
	{
		"Register R/W Test",
	 	sq_spi_master_slave_reg_test,
	 	0,
	 	1
	}
};

struct test_item_container sq_spi_master_slave_model_test_container = {
	.menu_name = "Master With Slave Test Model",
	.shell_name = "SPI",
	.items = sq_spi_master_slave_model_test_items,
	.test_item_size = sizeof(sq_spi_master_slave_model_test_items)
};

extern int sq_spi_master_slave_mode0_test(int autotest);
extern int sq_spi_master_slave_mode1_test(int autotest);
extern int sq_spi_master_slave_mode2_test(int autotest);
extern int sq_spi_master_slave_mode3_test(int autotest);

struct test_item sq_spi_master_slave_mode_test_items[] = {
	{
		"Mode 0 (CPHA0 CPOL0)",
     	sq_spi_master_slave_mode0_test,
	   	0,
	   	1
	},
	{
		"Mode 1 (CPHA0 CPOL1)",
	   	sq_spi_master_slave_mode1_test,
	   	0,
	   	1
	},
    {
		"Mode 2 (CPHA1 CPOL0)",
	   	sq_spi_master_slave_mode2_test,
	   	0,
	   	1
	},
	{
		"Mode 3 (CPHA1 CPOL1)",
	   	sq_spi_master_slave_mode3_test,
	   	0,
	   	1
	}
};

struct test_item_container sq_spi_master_slave_mode_test_container = {
	.menu_name = "Master With Slave Test Mode",
	.shell_name = "SPI",
	.items = sq_spi_master_slave_mode_test_items,
	.test_item_size = sizeof(sq_spi_master_slave_mode_test_items)
};


extern int sq_spi_slave_normal_test(int autotest);
extern int sq_spi_slave_hdma_test(int autotest);
extern int sq_spi_slave_reset_test(int autotest);

struct test_item sq_spi_slave_type_test_items[] = {
	{
		"Without HDMA",
	 	sq_spi_slave_normal_test,
	 	0,
	 	1
	},
	{	"With HDMA",
	 	sq_spi_slave_hdma_test,
	 	0,
	 	1
	},
	{	"Reset test",
	 	sq_spi_slave_reset_test,
	 	0,
	 	0
	}
};

struct test_item_container sq_spi_slave_type_test_container = {
	.menu_name = "Sq SPI Slave Type Test",
	.shell_name = "SPI",
	.items = sq_spi_slave_type_test_items,
	.test_item_size = sizeof(sq_spi_slave_type_test_items)
};



extern int sq_spi_master_lsb_test(int autotest);
extern int sq_spi_master_msb_test(int autotest);

struct test_item sq_spi_master_sb_test_items[] = {
	{
		"LSB",
	 	sq_spi_master_lsb_test,
	 	0,
	 	1
	},
	{	"MSB",
	 	sq_spi_master_msb_test,
	 	0,
	 	1
	}
};

struct test_item_container sq_spi_master_sb_test_container = {
	.menu_name = "Sq SPI Slave SB Test",
	.shell_name = "SPI",
	.items = sq_spi_master_sb_test_items,
	.test_item_size = sizeof(sq_spi_master_sb_test_items)
};


extern int sq_spi_master_ch8_test(int autotest);
extern int sq_spi_master_ch16_test(int autotest);

struct test_item sq_spi_master_ch_test_items[] = {
	{	"8 Bit",
	 	sq_spi_master_ch8_test,
	 	0,
	 	1
	},
	{	"16 Bit",
	 	sq_spi_master_ch16_test,
	 	0,
	 	1
	}
};

struct test_item_container sq_spi_master_ch_test_container = {
	.menu_name = "Sq SPI Master CHARACTER Test",
	.shell_name = "SPI",
	.items = sq_spi_master_ch_test_items,
	.test_item_size = sizeof(sq_spi_master_ch_test_items)
};
