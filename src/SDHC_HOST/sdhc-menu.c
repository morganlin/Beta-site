#include <test_item.h>
#include "dependency.h"

extern int socle_sdhc_host_slot0_test(int autotest);
extern int socle_sdhc_host_slot1_test(int autotest);
extern int socle_sdhc_host_slot2_test(int autotest);
extern int socle_sdhc_host_slot3_test(int autotest);

struct test_item socle_sdhc_host_slot_test_items[] = {
	{
	"SDHC Slot 0 Test",
	 socle_sdhc_host_slot0_test,
	 1,
	 SOCLE_SDHC_HOST_SLOT0
	 },
	{
	"SDHC Slot 1 Test",
	 socle_sdhc_host_slot1_test,
	 1,
	SOCLE_SDHC_HOST_SLOT1
	 },
	{
	"SDHC Slot 2 Test",
	 socle_sdhc_host_slot2_test,
	 1,
	SOCLE_SDHC_HOST_SLOT2
	 },
	{
	"SDHC Slot 3 Test",
	 socle_sdhc_host_slot3_test,
	 1,
	 SOCLE_SDHC_HOST_SLOT3
	 },
};

struct test_item_container socle_sdhc_host_slot_test_container = {
	.menu_name = "SDHC Host Slot Test",
	.shell_name = "sd",
	.items = socle_sdhc_host_slot_test_items,
	.test_item_size = sizeof(socle_sdhc_host_slot_test_items)
};

extern int socle_sdhc_host_0_test(int autotest);
extern int socle_sdhc_host_1_test(int autotest);

struct test_item socle_sdhc_host_test_items[] = {
	{"SDHC Host 0 Test",
	 socle_sdhc_host_0_test,
	 1,
	 SOCLE_SDHC_HOST_0_TEST},
	{"SDHC Host 1 Test",
	 socle_sdhc_host_1_test,
	 1,
	 SOCLE_SDHC_HOST_1_TEST},
};

struct test_item_container socle_sdhc_host_test_container = {
	.menu_name = "SDHC Host Test",
	.shell_name = "sd",
	.items = socle_sdhc_host_test_items,
	.test_item_size = sizeof(socle_sdhc_host_test_items)
};


extern int socle_sdhc_without_dma_test(int autotest);
extern int socle_sdhc_sdma_transfer_test(int autotest);
extern int socle_sdhc_adma1_transfer_test(int autotest);
extern int socle_sdhc_adma2_transfer_test(int autotest);

struct test_item socle_sdhc_dma_mode_test_items[] = {
	{"Transfer without DMA Test",
	 socle_sdhc_without_dma_test,
	 1,
	 1},
	{"SDMA Transfer Test",
	 socle_sdhc_sdma_transfer_test,
	 1,
	 1},
	{"ADMA1 Transfer Test",
	 socle_sdhc_adma1_transfer_test,
	 0,
	 0},
	{"ADMA2 Transfer Test",
	 socle_sdhc_adma2_transfer_test,
	 1,
	 1}
};

struct test_item_container socle_sdhc_dma_mode_test_container = {
	.menu_name = "SDHC DMA Mode Test",
	.shell_name = "sd",
	.items = socle_sdhc_dma_mode_test_items,
	.test_item_size = sizeof(socle_sdhc_dma_mode_test_items)
};

extern int socle_sdhc_hs_transfer_test(int autotest);
extern int socle_sdhc_normal_transfer_test(int autotest);

struct test_item socle_sdhc_speed_main_test_items[] = {
	{"High Speed Transfer Test (version > 1.1)",
	 socle_sdhc_hs_transfer_test,
	 1,
	 1},
	{"Normal Transfer Test",
	 socle_sdhc_normal_transfer_test,
	 1,
	 SDHC_NORMAL_TEST},
};

struct test_item_container socle_sdhc_speed_main_test_container = {
	.menu_name = "SDHC Speed Test",
	.shell_name = "speed",
	.items = socle_sdhc_speed_main_test_items,
	.test_item_size = sizeof(socle_sdhc_speed_main_test_items)
};

extern int socle_sdhc_sdio_wifi_test(int autotest);

struct test_item socle_sdhc_sdio_main_test_items[] = {
	{"Marvell wifi test",
	 socle_sdhc_sdio_wifi_test,
	 0,
	 1},
};

struct test_item_container socle_sdhc_sdio_main_test_container = {
	.menu_name = "SDIO Card Test",
	.shell_name = "sdio",
	.items = socle_sdhc_sdio_main_test_items,
	.test_item_size = sizeof(socle_sdhc_sdio_main_test_items)
};

extern int socle_sdhc_bus_1_transfer_test(int autotest);
extern int socle_sdhc_bus_4_transfer_test(int autotest);
extern int socle_sdhc_misc_command_test(int autotest);

struct test_item socle_sdmmc_sd_main_test_items[] = {
	{"1 Bit Bus Transfer Test",
	 socle_sdhc_bus_1_transfer_test,
	 1,
	 1},
	{"4 Bit Bus Transfer Test",
	 socle_sdhc_bus_4_transfer_test,
	 1,
	 1},
	{"Miscellaneous Command Test",
	 socle_sdhc_misc_command_test,
	 1,
	 1}
};

struct test_item_container socle_sdmmc_sd_main_test_container = {
	.menu_name = "SD Card Test",
	.shell_name = "sd",
	.items = socle_sdmmc_sd_main_test_items,
	.test_item_size = sizeof(socle_sdmmc_sd_main_test_items)
};

extern int socle_sdhc_without_auto_cmd12_test(int autotest);
extern int socle_sdhc_with_auto_cmd12_test(int autotest);

struct test_item socle_sdhc_auto_cmd12_test_items[] = {
	{"Without Auto CMD12",
	 socle_sdhc_without_auto_cmd12_test,
	 1,
	 1},
	{"With Auto CMD12",
	 socle_sdhc_with_auto_cmd12_test,
	 1,
	 1},
};

struct test_item_container socle_sdhc_auto_cmd12_test_container = {
	.menu_name = "SD CardA auto CMD12 Test",
	.shell_name = "auto CMD12",
	.items = socle_sdhc_auto_cmd12_test_items,
	.test_item_size = sizeof(socle_sdhc_auto_cmd12_test_items)
};
////
extern int socle_sdhc_manual_read_block_test(int autotest);
extern int socle_sdhc_manual_write_block_test(int autotest);
extern int socle_sdhc_manual_rw_loop_test(int autotest);

struct test_item socle_sdhc_manual_opmode_test_items[] = {
	{"Manual Read Block",
	 socle_sdhc_manual_read_block_test,
	 0,
	 1},
	{"Multiple Write Block",
	 socle_sdhc_manual_write_block_test,
	 0,
	 1},
	{"Manual R/W loop",
	 socle_sdhc_manual_rw_loop_test,
	 0,
	 1},
};

struct test_item_container socle_sdhc_manual_opmode_test_container = {
	.menu_name = "SD CardA Transfer Test",
	.shell_name = "sd",
	.items = socle_sdhc_manual_opmode_test_items,
	.test_item_size = sizeof(socle_sdhc_manual_opmode_test_items)
};
////
extern int socle_sdhc_manual_single_block_test(int autotest);
extern int socle_sdhc_manual_multiple_block_test(int autotest);

struct test_item socle_sdhc_manual_test_items[] = {
	{"Single Manual Block Transfer Test",
	 socle_sdhc_manual_single_block_test,
	 0,
	 1},
	{"Multiple Manual Block Transfer Test",
	 socle_sdhc_manual_multiple_block_test,
	 0,
	 1},
};

struct test_item_container socle_sdhc_manual_test_container = {
	.menu_name = "SD CardA Transfer Test",
	.shell_name = "sd",
	.items = socle_sdhc_manual_test_items,
	.test_item_size = sizeof(socle_sdhc_manual_test_items)
};

extern int socle_sdhc_single_block_transfer_test(int autotest);
extern int socle_sdhc_multiple_block_transfer_test(int autotest);
extern int socle_sdhc_sd_erase(int autotest);
extern int socle_sdhc_manual(int autotest);

struct test_item socle_sdmmc_sd_xfer_test_items[] = {
	{"Single Block Transfer Test",
	 socle_sdhc_single_block_transfer_test,
	 1,
	 1},
	{"Multiple Block Transfer Test",
	 socle_sdhc_multiple_block_transfer_test,
	 1,
	 1},
	{"Erasing Test",
	 socle_sdhc_sd_erase,
	 1,
	 1},
	{"Manual Test",
	 socle_sdhc_manual,
	 0,
	 1},
};

struct test_item_container socle_sdmmc_sd_xfer_test_container = {
	.menu_name = "SD CardA Transfer Test",
	.shell_name = "sd",
	.items = socle_sdmmc_sd_xfer_test_items,
	.test_item_size = sizeof(socle_sdmmc_sd_xfer_test_items)
};

extern int socle_sdhc_misc_command_9(int autotest);
extern int socle_sdhc_misc_command_10(int autotest);
extern int socle_sdhc_misc_command_13(int autotest);
extern int socle_sdhc_misc_command_15(int autotest);
extern int socle_sdhc_misc_app_command_51(int autotest);
extern int socle_sdhc_manual_command(int autotest);

struct test_item socle_sdhc_misc_command_test_items[] = {
	{"Command 9 (SEND_CSD)",
	 socle_sdhc_misc_command_9,
	 1,
	 1},
	{"Command 10 (SEND_CID)",
	 socle_sdhc_misc_command_10,
	 1,
	 1},
	{"Command 13 (SEND_STATUS)",
	 socle_sdhc_misc_command_13,
	 1,
	 1},
	{"Command 15 (GO_INACTIVE_STATE)",
	 socle_sdhc_misc_command_15,
	 0,
	 1},
	{"Application Specific Command 51 (SEND_SCR)",
	 socle_sdhc_misc_app_command_51,
	 1,
	 1},
	{"Send Command by manual",
	 socle_sdhc_manual_command,
	 0,
	 1}
};

struct test_item_container socle_sdhc_misc_command_test_container = {
	.menu_name = "SD Miscellaneouse Command Test Menu",
	.shell_name = "sd",
	.items = socle_sdhc_misc_command_test_items,
	.test_item_size = sizeof(socle_sdhc_misc_command_test_items)
};


