#include <test_item.h>
#include "dependency.h"

#include "socle_nand_regs.h"

extern int nfc_axi_test(int autotest);
extern int nfc_ahb_test(int autotest);

struct test_item nfc_fpga_test_items[] =
{

	{
		"AXI Test",
		nfc_axi_test,
		1,
		1
	},
	{
		"AHB Test",
		nfc_ahb_test,
		1,
		1
	},
};

struct test_item_container nfc_fpga_test_container = 
{
    .menu_name = "nand fpga test",
    .shell_name = "NAND FPGA Test ",
    .items = nfc_fpga_test_items,
    .test_item_size = sizeof(nfc_fpga_test_items)
};

extern int interrupt_rnb_test(int autotest);
extern int interrupt_protect_test(int autotest);
extern int interrupt_dma_test(int autotest);
extern int interrupt_access_err_test(int autotest);

struct test_item nfc_ctrl_interrupt_test_items[] =
{
	{
		"Interrupt RnB Test",
		interrupt_rnb_test,
		1,
		1
	},
	{
		"Interrupt Protect Test",
		interrupt_protect_test,
		1,
		1
	},
	{
		"Interrupt Dma Test",
		interrupt_dma_test,
		1,
		DMA_IMPLEMENT
	},
	{
		"Interrupt fldata access errort test",
		interrupt_access_err_test,
		1,
		1
	},	
};

struct test_item_container nfc_ctrl_interrupt_test_container =
{
    .menu_name = "Interrupt TEST",
    .shell_name = "NFC Interrupt Test",
    .items = nfc_ctrl_interrupt_test_items,
    .test_item_size = sizeof(nfc_ctrl_interrupt_test_items)
};

//fllookup-------------------------------------------------------------------

extern int fllookup_reg0 (int autotest);
extern int fllookup_reg1 (int autotest);
extern int fllookup_reg2 (int autotest);
extern int fllookup_reg3 (int autotest);
extern int fllookup_reg4 (int autotest);
extern int fllookup_reg5 (int autotest);
extern int fllookup_reg6 (int autotest);
extern int fllookup_reg7 (int autotest);


struct test_item nfc_ctrl_fllookup_test_items[] =
{
	{
		"FLLOOKUP REG0 TEST",
		fllookup_reg0,
		1,
		1
	},
	{
		"FLLOOKUP REG1 TEST",
		fllookup_reg1,
		0,
		1
	},
	{
		"FLLOOKUP REG2 TEST",
		fllookup_reg2,
		0,
		1
	},
	{
		"FLLOOKUP REG3 TEST",
		fllookup_reg3,
		0,
		1
	},
	{
		"FLLOOKUP REG4 TEST",
		fllookup_reg4,
		0,
		1
	},
	{
		"FLLOOKUP REG5 TEST",
		fllookup_reg5,
		0,
		1
	},
	{
		"FLLOOKUP REG6 TEST",
		fllookup_reg6,
		0,
		1
	},
	{
		"FLLOOKUP REG7 TEST",
		fllookup_reg7,
		0,
		1
	},
};

struct test_item_container nfc_ctrl_fllookup_test_container =
{
    .menu_name = "NAND FLLOOKUP TEST",
    .shell_name = "NAND Fllookup Test",
    .items = nfc_ctrl_fllookup_test_items,
    .test_item_size = sizeof(nfc_ctrl_fllookup_test_items)
};

extern int ecc_errbit1_corrected(int autotest);
extern int ecc_errbit2_corrected(int autotest);
extern int ecc_errbit3_corrected(int autotest);
extern int ecc_errbit4_corrected(int autotest);
extern int ecc_errbit5_corrected(int autotest);
extern int ecc_errbit6_corrected(int autotest);
extern int ecc_errbit7_corrected(int autotest);
extern int ecc_errbit8_corrected(int autotest);
extern int ecc_errbit9_corrected(int autotest);
extern int ecc_errbit10_corrected(int autotest);
extern int ecc_errbit11_corrected(int autotest);
extern int ecc_errbit12_corrected(int autotest);
extern int ecc_errbit13_corrected(int autotest);
extern int ecc_errbit14_corrected(int autotest);
extern int ecc_errbit15_corrected(int autotest);
extern int ecc_errbit16_corrected(int autotest);

struct test_item nfc_ctrl_ecc_correct_test_items[] =
{
	{
		"1bit Error corrected",
		ecc_errbit1_corrected,
		1,
		BCH4_IMPLEMENT | BCH8_IMPLEMENT | BCH16_IMPLEMENT | HM_IMPLEMENT
	},      
	{
		"2bit Error corrected",
		ecc_errbit2_corrected,
		0,
		BCH4_IMPLEMENT | BCH8_IMPLEMENT | BCH16_IMPLEMENT//|HM_IMPLEMENT
	},      
	{
		"3bit Error corrected",
		ecc_errbit3_corrected,
		0,
		BCH4_IMPLEMENT | BCH8_IMPLEMENT | BCH16_IMPLEMENT
	},      
	{
		"4bit Error corrected",
		ecc_errbit4_corrected,
		0,
		BCH4_IMPLEMENT | BCH8_IMPLEMENT | BCH16_IMPLEMENT
	},      
	{
		"5bit Error corrected",
		ecc_errbit5_corrected,
		0,
		BCH8_IMPLEMENT | BCH16_IMPLEMENT//|BCH4_IMPLEMENT 
	},      
	{
		"6bit Error corrected",
		ecc_errbit6_corrected,
		0,
		BCH8_IMPLEMENT | BCH16_IMPLEMENT
	},      
	{
		"7bit Error corrected",
		ecc_errbit7_corrected,
		0,
		BCH8_IMPLEMENT | BCH16_IMPLEMENT
	},      
	{
		"8bit Error corrected",
		ecc_errbit8_corrected,
		0,
		BCH8_IMPLEMENT | BCH16_IMPLEMENT
	},      
	{
		"9bit Error corrected",
		ecc_errbit9_corrected,
		0,
		BCH16_IMPLEMENT//|BCH8_IMPLEMENT
	},      
	{
		"10bit Error corrected",
		ecc_errbit10_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"11bit Error corrected",
		ecc_errbit11_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"12bit Error corrected",
		ecc_errbit12_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"13bit Error corrected",
		ecc_errbit13_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"14bit Error corrected",
		ecc_errbit14_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"15bit Error corrected",
		ecc_errbit15_corrected,
		0,
		BCH16_IMPLEMENT
	},      
	{
		"16bit Error corrected",
		ecc_errbit16_corrected,
		0,
		BCH16_IMPLEMENT
	},      
		
};

struct test_item_container nfc_ctrl_ecc_correct_test_container = 
{
    .menu_name = "ECC Correction",
    .shell_name = "ECC Correction",
    .items = nfc_ctrl_ecc_correct_test_items,
    .test_item_size = sizeof(nfc_ctrl_ecc_correct_test_items)
};

extern int bch4_ecc_corrected(int autotest);
extern int bch8_ecc_corrected(int autotest);
extern int bch16_ecc_corrected(int autotest);
extern int hm_ecc_corrected(int autotest);

struct test_item nfc_ctrl_ecc_correct_mode_test_items[] =
{
	{
		"BCH4 Error corrected",
		bch4_ecc_corrected,
		1,
		BCH4_IMPLEMENT
	},      
	{
		"BCH8 Error corrected",
		bch8_ecc_corrected,
		1,
		BCH8_IMPLEMENT
	},
	{
		"BCH16 Error corrected",
		bch16_ecc_corrected,
		1,
		BCH16_IMPLEMENT
	},
	{
		"HM Error corrected",
		hm_ecc_corrected,
		1,
		HM_IMPLEMENT
	},
	
};

struct test_item_container nfc_ctrl_ecc_correct_mode_test_container = 
{
    .menu_name = "ECC Mode Correct",
    .shell_name = "ECC Mode Correct",
    .items = nfc_ctrl_ecc_correct_mode_test_items,
    .test_item_size = sizeof(nfc_ctrl_ecc_correct_mode_test_items)
};

extern int socle_nand_page_read_program_loopback_test(int autotest);
extern int socle_nand_word_read_program_loopback_test(int autotest);
extern int socle_nand_word_random_read_program_loopback_test(int autotest);
extern int socle_block_erase_test(int autotest);

struct test_item nfc_ctrl_event_items[] =
{
	{
		"Page Read/Program Loopback Test",
		socle_nand_page_read_program_loopback_test,
		1,
		1
	},
	{
		"Page Read1/Program1 Loopback Test",
		socle_nand_word_read_program_loopback_test,
		0,
		1
	},
	{
		"Random Page Read1/Program1 Loopback Test",
		socle_nand_word_random_read_program_loopback_test,
		0,
		1
	},
	{
		"Erase # Block Test",
		socle_block_erase_test,
		0,
		1
	},
};

struct test_item_container nfc_ctrl_event_container = 
{
    .menu_name = "Event Test",
    .shell_name = "Event Test",
    .items = nfc_ctrl_event_items,
    .test_item_size = sizeof(nfc_ctrl_event_items)
};

extern int dma_transfer_size_8bit(int autotest);
extern int dma_transfer_size_16bit(int autotest);
extern int dma_transfer_size_32bit(int autotest);

struct test_item nfc_ctrl_dma_transfer_size_test_items[] =
{
	{
		"8bit dma transfer size",
		dma_transfer_size_8bit,
		1,
		1
	},      
	{
		"16bit dma transfer size",
		dma_transfer_size_16bit,
		1,
		1
	},      
	{
		"32bit dma transfer size",
		dma_transfer_size_32bit,
		1,
		1
	},      
};

struct test_item_container nfc_ctrl_dma_transfer_size_test_container = 
{
    .menu_name = "DMA Width Mode",
    .shell_name = "DMA Width Mode",
    .items = nfc_ctrl_dma_transfer_size_test_items,
    .test_item_size = sizeof(nfc_ctrl_dma_transfer_size_test_items)
};

extern int dma_bust_single_decrement(int autotest);
extern int data_bust_unspecifed_addr_decrement(int autotest);
extern int dma_bust_single_increment(int autotest);
extern int data_bust_unspecifed_addr_increment(int autotest);
extern int dma_4bit_bust_addr_increment(int autotest);
extern int dma_8bit_bust_addr_increment(int autotest);
extern int dma_16bit_bust_addr_increment(int autotest);
extern int dma_stream_bust_addr_const(int autotest);

struct test_item nfc_ctrl_transfer_dma_mode_test_items[] =
{
	{
		"bust of unspecified length address increment",
		data_bust_unspecifed_addr_increment,
		1,
		0
	},
	{
		"single transfer address decrement",
		dma_bust_single_decrement,
		0,
		0
	},      
	{
		"bust of unspecified length address decrement",
		data_bust_unspecifed_addr_decrement,
		0,
		0
	},
	{
		"single transfer address increment",
		dma_bust_single_increment,
		1,
		1
	},      
	
	{
		"4 beat burst address increment",
		dma_4bit_bust_addr_increment,
		0,
		1
	},      
	{
		"8 beat burst address increment",
		dma_8bit_bust_addr_increment,
		0,
		1
	},
	{
		"16 beat burst address increment",
		dma_16bit_bust_addr_increment,
		0,
		1
	},      
	{
		"stream burst address const",
		dma_stream_bust_addr_const,
		0,
		0
	},
};

struct test_item_container nfc_ctrl_transfer_dma_mode_test_container = 
{
    .menu_name = "DMA Burst Mode",
    .shell_name = "DMA Burst Mode",
    .items = nfc_ctrl_transfer_dma_mode_test_items,
    .test_item_size = sizeof(nfc_ctrl_transfer_dma_mode_test_items)
};

extern int instruction_with_dma_test(int autotest);
extern int instruction_without_dma_test(int autotest);

struct test_item nfc_ctrl_data_transfer_dma_test_items[] =
{
	{
		"DMA ON",
		instruction_with_dma_test,
		1,
		DMA_IMPLEMENT
	},      
	{
		"DMA OFF",
		instruction_without_dma_test,
		1,
		1
	},
};

struct test_item_container nfc_ctrl_data_transfer_dma_test_container = 
{
    .menu_name = "DMA on/off",
    .shell_name = "DMA on/off",
    .items = nfc_ctrl_data_transfer_dma_test_items,
    .test_item_size = sizeof(nfc_ctrl_data_transfer_dma_test_items)
};

extern int instruction_test(int autotest);
extern int ecc_correction_test(int autotest);
extern int fllookup_test(int autotest);
extern int powersave_test(int autotest);
extern int interrupt_test(int autotest);
//extern int autoboot_page_program_0th(void);
extern int autoboot_page_program_0th(int autotest);	//jerry hsieh fix ; 2010/04/26
extern int autoboot_test(int autotest);

struct test_item nfc_ctrl_feature_test_items[] =
{

	{
		"Instruction Test",
		instruction_test,
		1,
		1
	},
	{
		"ECC Correction Test",
		ecc_correction_test,
		1,
		BCH4_IMPLEMENT | BCH8_IMPLEMENT | BCH16_IMPLEMENT | HM_IMPLEMENT 
	},      
	{
		"Page Remap Test(lookup)",
		fllookup_test,
		1,
		1
	},		
	{
		"Power Save Test",
		powersave_test,
		0,	//modify
		PSN_IMPLEMENT
	},
	{
		"Interrupt Test",
		interrupt_test,
		1,
		1
	},	
	{
		"program 0th page before auto boot test",
		autoboot_page_program_0th,
		0,
		1
	},
#if 0
	{
		"Auto Boot Up Test",
		autoboot_test,
		0,
		1
	},
#endif
};

struct test_item_container nfc_ctrl_feature_test_container = 
{
    .menu_name = "nand feature test",
    .shell_name = "NAND Test Main Menu",
    .items = nfc_ctrl_feature_test_items,
    .test_item_size = sizeof(nfc_ctrl_feature_test_items)
};



