#include <test_item.h>
#include "dependency.h"

extern int sq_spi_slave0_test(int autotest);
extern int sq_spi_slave1_test(int autotest);

struct test_item sq_spi_slave_test_items[] = {
	{
		"SPI 0 Test",
		sq_spi_slave0_test,
		0,
		1
	},
	{
		"SPI 1 Test",
		sq_spi_slave1_test,
		0,
		1
	}
};

struct test_item_container sq_spi_slave_test_container = {
	.menu_name = "SPI SLAVE Test",
	.shell_name = "SPI",
	.items = sq_spi_slave_test_items,
	.test_item_size = sizeof(sq_spi_slave_test_items)
};

extern int sq_spi_slave_protocol_test(int autotest);
extern int sq_spi_slave_pure_test(int autotest);
extern int sq_spi_reg_rw_test(int autotest);

struct test_item sq_spi_slave_model_test_items[] = {
	{"SW Protocol Test",
	 sq_spi_slave_protocol_test,
	 0,
	 1},
	{"Pure TxRx Test",
	 sq_spi_slave_pure_test,
	 0,
	 1
	},
	{"Register R/W Test",
	sq_spi_reg_rw_test,
	0,
	1
	}
};

struct test_item_container sq_spi_slave_model_test_container = {
	.menu_name = "SPI SLAVE Model Test",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_model_test_items,
	.test_item_size = sizeof(sq_spi_slave_model_test_items)
};

extern int sq_spi_slave_mode0_test(int autotest);
extern int sq_spi_slave_mode1_test(int autotest);
extern int sq_spi_slave_mode2_test(int autotest);
extern int sq_spi_slave_mode3_test(int autotest);

struct test_item sq_spi_slave_mode_test_items[] = {
	{"Mode 0 (CPHA0 CPOL0)",
	 sq_spi_slave_mode0_test,
	 0,
	 1},
	{"Mode 1 (CPHA0 CPOL1)",
	 sq_spi_slave_mode1_test,
	 0,
	 1
	 },
	 {"Mode 2 (CPHA1 CPOL0)",
	 sq_spi_slave_mode2_test,
	 0,
	 1
	 },
	 {"Mode 3 (CPHA1 CPOL1)",
	 sq_spi_slave_mode3_test,
	 0,
	 1
	 }
};

struct test_item_container sq_spi_slave_mode_test_container = {
	.menu_name = "SPI SLAVE Mode Test",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_mode_test_items,
	.test_item_size = sizeof(sq_spi_slave_mode_test_items)
};

extern int sq_spi_slave_lsb_test(int autotest);
extern int sq_spi_slave_msb_test(int autotest);

struct test_item sq_spi_slave_sb_test_items[] = {
	{"LSB",
	 sq_spi_slave_lsb_test,
	 0,
	 1},
	{"MSB",
	 sq_spi_slave_msb_test,
	 0,
	 1
	 }
};

struct test_item_container sq_spi_slave_sb_test_container = {
	.menu_name = "SPI SLAVE SB Test",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_sb_test_items,
	.test_item_size = sizeof(sq_spi_slave_sb_test_items)
};

extern int sq_spi_slave_ch8_test(int autotest);
extern int sq_spi_slave_ch16_test(int autotest);

struct test_item sq_spi_slave_ch_test_items[] = {
	{"8 Bit",
	 sq_spi_slave_ch8_test,
	 0,
	 1
	 },
	 {"16 Bit",
	 sq_spi_slave_ch16_test,
	 0,
	 1
	 }
};

struct test_item_container sq_spi_slave_ch_test_container = {
	.menu_name = "SPI SLAVE CHARACTER Test",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_ch_test_items,
	.test_item_size = sizeof(sq_spi_slave_ch_test_items)
};



extern int sq_spi_slave_pure_normal_run(int autotest);
extern int sq_spi_slave_pure_hdma_test(int autotest);

struct test_item sq_spi_slave_pure_type_test_items[] = {
	{"Without HDMA",
	 sq_spi_slave_pure_normal_run,
	 0,
	 1},
	{"With HDMA",
	 sq_spi_slave_pure_hdma_test,
	 0,
	 1
	}
};

struct test_item_container sq_spi_slave_pure_type_test_container = {
	.menu_name = "SPI SLAVE Pure Type",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_pure_type_test_items,
	.test_item_size = sizeof(sq_spi_slave_pure_type_test_items)
};


extern int sq_spi_slave_hdma_burst_single(int autotest);
extern int sq_spi_slave_hdma_burst_incr4(int autotest);

struct test_item sq_spi_slave_burst_type_test_items[] = {
	{"Single",
	 sq_spi_slave_hdma_burst_single,
	 0,
	 1},
	{"INCR4",
	 sq_spi_slave_hdma_burst_incr4,
	 0,
	 1}
};

struct test_item_container sq_spi_slave_burst_type_test_container = {
	.menu_name = "SPI SLAVE HDMA Burst Type",
	.shell_name = "SPI_SLAVE",
	.items = sq_spi_slave_burst_type_test_items,
	.test_item_size = sizeof(sq_spi_slave_burst_type_test_items)
};
