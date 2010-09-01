#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include <sq-scu.h>
#include "nand.h"
#include "dependency.h"
#include "socle_nand_regs.h"


// #define NFC_CTRL_DEBUG 
#ifdef NFC_CTRL_DEBUG
#define NDEBUG(fmt, args...) printf(fmt, ## args)
#else
#define NDEBUG(fmt, args...)
#endif

#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

enum nfc_mode_select {
		normal = 0,
		fpga_axi,
		fpga_ahb,
	} nfc_mode;

enum {
	BUFFER_INITIAL = 0,
	BUFFER_READ	= 1,
	BUFFER_WRITE	= 2,
};

typedef struct
{
	u16    logic_block_number;//User area; 
	u16    physical_block_number;//Reserve area
}sFLLookUpPattern; 
/*
//Block number mapping
#define LogBlock0    20
#define LogBlock1    21
#define LogBlock2    22
#define LogBlock3    23
#define LogBlock4    24
#define LogBlock5    25
#define LogBlock6    26
#define LogBlock7    27

#define PhyBlock0    10
#define PhyBlock1    11
#define PhyBlock2    12
#define PhyBlock3    13
#define PhyBlock4    14
#define PhyBlock5    15
#define PhyBlock6    16
#define PhyBlock7    17

sFLLookUpPattern FLLookUpPattern[8] =
{
	{    LogBlock0, PhyBlock0    },
	{    LogBlock1, PhyBlock1    },
	{    LogBlock2, PhyBlock2    },
	{    LogBlock3, PhyBlock3    },
	{    LogBlock4, PhyBlock4    },
	{    LogBlock5, PhyBlock5    },
	{    LogBlock6, PhyBlock6    },
	{    LogBlock7, PhyBlock7    },
};*/

static u32 page_number = 0;
static u32 reference_page;
static u32 target_page;

//*****************************************************************************************************
#define NAND_TEST_PAGE_ADDR 	(64 + 0)

#define TO_THRESHOLD_CNT (1024 * 1024)

#define TEST_PATTERN_TX_ADDR 0x00a00000
#define TEST_PATTERN_RX_ADDR 0x00a10000

#define TEST_MEM_PATTERN_ADDR TEST_PATTERN_TX_ADDR
#define TEST_MEM_CMPR_ADDR TEST_PATTERN_RX_ADDR


#if 0
#define TEST_PATTERN_BUF_SIZE 4096
#define MAX_SPARE_SIZE		128
#else
u32 TEST_PATTERN_BUF_SIZE = 2048;
u32 MAX_SPARE_SIZE = 64;
#endif

#define TEST_BEGIN_PROTECT_BLOCK	10
#define TEST_END_PROTECT_BLOCK	15

//*****************************************************************************************************

extern struct mtd_info socle_mtd_info;
extern struct socle_nand_info *nand_info;
extern struct socle_nand_info socle_info;

struct mtd_info *mtd = &socle_mtd_info;


extern void socle_power_save_on_off(u8 power_mode);
extern void socle_nfc_set_ecc(struct socle_nand_info *info);
void socle_nfc_release_all_lookup(void);
u32 socle_nfc_read_reg_ecc(void);
extern void socle_nfc_set_dma(struct socle_nand_info *info);
extern u8 socle_nfc_read_status(struct socle_nand_info *info);
extern void socle_init_nfc_int(void);
extern void socle_exit_nfc_int(void);
extern u8 socle_nfc_data_reg_transfer( u8 *buf, u32 buf_size, u8 direction);
extern void socle_nfc_init(void);
extern inline u32 socle_nfc_read(u32 reg);
extern u8 socle_nfc_lock(struct socle_nand_info *info);
extern u8 socle_nfc_block_erase(struct socle_nand_info *info, u32 block_number);
extern u8 socle_nfc_page_program(struct socle_nand_info *info, u32 page_number);
extern u8 socle_nfc_page_program_memory(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern void socle_nfc_set_lookup(u16 log_address, u16 phy_address, u32 channel);
extern u8 socle_nfc_page_read(struct socle_nand_info *info, u32 page_number);
extern u8 socle_nfc_page_read_memory(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern void socle_nfc_prepare_dma_transfer(struct socle_nand_info *info, u32 data_size, u32 data_offset);
extern u8 socle_nfc_page_read1(struct socle_nand_info *info,u32 page_num, u16 page_offset);
extern u8 socle_nfc_page_read1_memory(struct socle_nand_info *info, u32 page_num, u32 page_offset, u8 *buf, u32 buf_size);
extern u8 socle_nfc_page_program1(struct socle_nand_info *info, u32 page_num, u32 page_offset);
extern u8 socle_nfc_page_program1_memory(struct socle_nand_info *info, u32 page_num, u32 page_offset, u8 *buf, u32 buf_size);
extern void socle_nfc_set_protected_area(u16 begin_protect_area, u16 end_protect_area);
extern int socle_nfc_write_command(struct socle_nand_info *info);
extern int socle_nfc_read_command(struct socle_nand_info *info);
extern u8 socle_nfc_internal_data_move(struct socle_nand_info *info, u32 source_page_num, u32 target_page_num);
extern u8 socle_nfc_read_status(struct socle_nand_info *info);
extern void socle_nfc_select_chip (struct mtd_info *mtd, int chip);
extern u8  nand_detect_flash(struct socle_nand_info *info);
extern u8 socle_nfc_dma_transfer(struct socle_nand_info *info, u32 data_size, u32 data_offset);
extern u8 socle_nfc_interrupt_page_read(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern u8 socle_nfc_interrupt_page_program(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern u8 socle_nfc_dma_interrupt_page_read(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern u8 socle_nfc_dma_interrupt_page_program(struct socle_nand_info *info, u32 page_number, u32 data_size);
extern int socle_nand_badblock_scan_test(struct socle_nand_info *info);
extern u8 socle_nfc_random_data_input(struct socle_nand_info *info,  u32 page_offset);
extern u8 socle_nfc_random_data_read(struct socle_nand_info *info, u16 page_offset);
extern u8 socle_nfc_random_data_input_memory(struct socle_nand_info *info,  u32 page_offset, u8 *buf, u32 buf_size);
extern u8 socle_nfc_random_data_read_memory(struct socle_nand_info *info, u32 page_offset, u8 *buf, u32 buf_size);
extern u8 socle_nfc_read_status(struct socle_nand_info *info);
extern u8 socle_nfc_internal_data_move1(struct socle_nand_info *info, u32 source_page_num, u32 target_page_num,  u32 page_offset, u8 *buf, u32 buf_size);
extern u8 socle_nfcg_page_program_cache(struct socle_nand_info *info, u32 page_num, u16 buf_size);
extern u8 socle_nfcg_page_program_cache1(struct socle_nand_info *info, u32 page_num, u8 *buf, u16 buf_size);
extern void socle_nfc_isr(void *data);
extern u8 socle_nfc_check_ecc_register(void);
extern int socle_init_badblock_remap(struct socle_nand_info*info);

static void make_error_bit_pattern(u8 *ext_buf,u8 err_num, u8 sub_page_num);
static int socle_nand_ecc_correction_test(u32 PageNum1, u32 PageNum2 ,u8 ErrBitNum);
extern inline void socle_nfc_write(u32 val, u32 reg);
extern volatile int socle_nfc_int_flag;
extern volatile u32 socle_nfc_int_state;
extern volatile u32 socle_nfc_protect_err_flag;
extern volatile u32 socle_nfc_fldata_access_err_flag;
//------------------------------------------------------------------------------

/**********************Pattern Prepare *******************************************/

static void
socle_nand_initial_buf(u8 mode)
{
	if (mode == BUFFER_INITIAL) {
		memset((char *) TEST_PATTERN_TX_ADDR, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);
		memset((char *) TEST_PATTERN_RX_ADDR, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);
	} 
	else if (mode == BUFFER_READ) {
		nand_info->dma_buffer = TEST_PATTERN_RX_ADDR;
		memset((char *) TEST_PATTERN_RX_ADDR, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);		
	}
	else if (mode == BUFFER_WRITE) {
		nand_info->dma_buffer = TEST_PATTERN_TX_ADDR;
	}
	memset((char *) nand_info->buf_addr, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);
}

static void 
socle_nand_prepare_pattern(u8 *buf, u32 size)
{
	int i;

	for (i = 0; i < size ;i=i+4 ) {
		buf[i]= i;
		buf[i+1]= i >>  8;
		buf[i+2]= i >> 16;
		buf[i+3]= i >> 24;
	}
}


static u8 
socle_nand_compare_pattern(u32 data_a, u32 data_b, u32 size)
{
	u32 i =0;

	while(i*4<size) {
		if(*(u32*) (data_a + (i*4)) != *(u32*) (data_b + (i*4)))
		{
			printf("i =0x%x, buf_a =0x%x, buf_b=0x%x \n",i, *(u32*) (data_a + (i*4)),*(u32*) (data_b + (i*4)));
			printf("Compare Data Error\n");
			return -1;
		}
		i++;
	}
	return 0;
}
/************************************************************************************************/
extern u8
socle_nfc_data_reg_transfer( u8 *buf, u32 buf_size, u8 direction)
{
	u32 *dbuf;

	// Does parameters are valids?
	dbuf = (u32 *) buf;
	buf_size /= 4; /* 1 DWORD = 4 BYTE*/
    
	// Copy bytes from data register
	if(direction == NF_TRANSFER_READ) {
		while(buf_size--) {
			*dbuf = socle_nfc_read( NF_SFR_FLDATA);
			dbuf ++;
		}
	} else {// Copy bytes to data register
		while(buf_size--) {            
			socle_nfc_write( *dbuf, NF_SFR_FLDATA);
			dbuf ++;
		}
	}
	return 0;
}

extern u32
socle_get_test_page_number(void)
{
	NDEBUG("get test page number\n");
	struct socle_nand_flash *flash_info= nand_info->flash_info;
	int block_erase_num;
	//erase block 
	if (page_number == 0 || ((page_number+1) %flash_info->page_per_block)==0)
	{
		block_erase_num = (int)NAND_TEST_PAGE_ADDR/flash_info->page_per_block; 		
		NDEBUG("Block erase number = 0x%x\n", block_erase_num);
		if(socle_nfc_block_erase(nand_info, block_erase_num))
			return -1;
		page_number = 0;
	}
	
	return NAND_TEST_PAGE_ADDR + page_number++;
}

/************************************************************************************************/

extern int
interrupt_rnb_test(int autotest)
{
	NDEBUG("---Interrupt RnB Test---\n");
	socle_init_nfc_int();
	socle_nfc_int_flag = 0;
	socle_nand_initial_buf(BUFFER_READ);
	if(socle_nfc_page_read(nand_info, NAND_TEST_PAGE_ADDR)) {
		printf("%s: Page read fail!\n", __func__);
		socle_exit_nfc_int();
		return -1;
	}
	socle_exit_nfc_int();
	return 0;
}

extern int
interrupt_protect_test(int autotest)
{
	NDEBUG("---Interrupt Protect Test---\n");
	socle_init_nfc_int();
	u32 i, page_number;
	socle_nfc_set_protected_area(TEST_BEGIN_PROTECT_BLOCK, TEST_END_PROTECT_BLOCK);

	memcpy((char *)nand_info->buf_addr, (char *)TEST_PATTERN_TX_ADDR, nand_info->flash_info->page_size);
	for( i=TEST_BEGIN_PROTECT_BLOCK; i<TEST_END_PROTECT_BLOCK; i++) {
		socle_nfc_int_flag = 0;
		socle_nfc_protect_err_flag = 0;
		page_number = i*nand_info->flash_info->page_per_block;
		socle_nfc_page_program(nand_info, page_number);
	}
	socle_nfc_set_protected_area(0, 0);
	socle_exit_nfc_int();
	return socle_nfc_protect_err_flag ? 0 : -1;
}

extern int
interrupt_dma_test(int autotest)
{
	NDEBUG("---Interrupt DMA Test---\n");
	socle_init_nfc_int();
	nand_info->dma_on= 1;
	nand_info->dma_size= NF_DMA_SIZE_32;//NF_DMA_SIZE_16
	nand_info->dma_mode = NF_DMA_BURST_8_ADDR_INC;
	nand_info->dma_direct = NF_DMA_READ;
	nand_info->dma_buffer = TEST_PATTERN_RX_ADDR;
	socle_nfc_set_dma(nand_info);
	socle_nfc_prepare_dma_transfer(nand_info, nand_info->flash_info->page_size, 0);

	socle_nand_initial_buf(BUFFER_READ);
	if(socle_nfc_page_read(nand_info, NAND_TEST_PAGE_ADDR)) {
		printf("%s: Page read fail!\n", __func__);
		socle_exit_nfc_int();
		return -1;
	}
	socle_exit_nfc_int();
	return 0;
}

extern int
interrupt_access_err_test(int autotest)
{
	socle_init_nfc_int();
	socle_nfc_fldata_access_err_flag = 0;
	u32 tmp_buf = 0x1234;
	socle_nand_initial_buf(BUFFER_READ);
	socle_nfc_page_read1(nand_info, 0, 0);
	socle_nfc_data_reg_transfer((u8 *)(&tmp_buf), 4, NF_TRANSFER_WRITE);
	socle_exit_nfc_int();
	if(socle_nfc_fldata_access_err_flag == 0)
		return -1;
	else
		return 0;
}


extern struct test_item_container nfc_ctrl_interrupt_test_container;

extern int 
interrupt_test(int autotest)
{
#if 1
	int ret = 0;

	nand_info->irq_on = 1;
	//socle_init_nfc_int();	//all interrupt, DMA, RnB, Protect

	NDEBUG("Start Test Interrupt\n");
	ret = test_item_ctrl(&nfc_ctrl_interrupt_test_container, autotest);

	nand_info->irq_on = 0;
	//socle_exit_nfc_int();
	return ret;

#else
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;
	u32 block_erase_num = 0;

	socle_nfc_int_flag = 0;
	socle_nfc_int_state = 0;

	nand_info->dma_on = 0;
	nand_info->ecc_on = 0;
	socle_nfc_set_ecc(nand_info);

	//test RnB interrupt
	NDEBUG("------------Int test 1--------------------\n");
	nand_info->dma_on = 1;
	nand_info->dma_size= NF_DMA_SIZE_8;//NF_DMA_SIZE_16
	nand_info->dma_mode = NF_DMA_BURST_8_ADDR_INC;

	socle_nand_initial_buf(BUFFER_READ);
	if (!(socle_nfc_interrupt_page_read(nand_info, NAND_TEST_PAGE_ADDR, flash_info->page_size) & NAND_STATUS_READY))
		return -1;
	 free_irq(NAND_INT);
	 
#if 0//def NFC_CTRL_DEBUG
	show_data((u8 *)nand_info->buf_addr, 128);
	show_data((u8 *)TEST_PATTERN_RX_ADDR, 128);
#endif
	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,nand_info->buf_addr, flash_info->page_size))
		ret= -1;


	//test Prot_IE  interrupt
	NDEBUG("------------Int test 2--------------------\n");
	socle_nfc_set_protected_area(TEST_BEGIN_PROTECT_BLOCK, TEST_END_PROTECT_BLOCK);

    	socle_nand_initial_buf(BUFFER_WRITE);
	if (!(socle_nfc_interrupt_page_program(nand_info, (TEST_BEGIN_PROTECT_BLOCK+1) *(flash_info->page_per_block),flash_info->page_size) & NAND_STATUS_WP))
		ret = -1;
	free_irq(NAND_INT);
	NDEBUG(" Protect  socle_nfc_int_state =0x%x\n",socle_nfc_int_state);
	socle_nfc_set_protected_area(0, 0);


	//Test DMA IE
	NDEBUG("------------Int test 3--------------------\n");
	nand_info->dma_on= 1;
	nand_info->dma_size= NF_DMA_SIZE_8;//NF_DMA_SIZE_16
	nand_info->dma_mode = NF_DMA_BURST_8_ADDR_INC;

	if ((page_number %flash_info->page_per_block)==0)
	{
		block_erase_num = (int)(NAND_TEST_PAGE_ADDR + page_number)/flash_info->page_per_block; 		
		NDEBUG("Block erase number = 0x%x\n", block_erase_num);
		if(socle_nfc_block_erase(nand_info, block_erase_num))
			return -1;
		if (socle_nfc_read_status(nand_info) & NAND_STATUS_FAIL)
			return -1;
		page_number = 0;
	}
	page_number ++;
	NDEBUG("===Int write page\n");	
	socle_nand_initial_buf(BUFFER_WRITE);
	if (!(socle_nfc_dma_interrupt_page_program(nand_info, NAND_TEST_PAGE_ADDR + page_number, flash_info->page_size) & NAND_STATUS_READY))
		return -1;
	free_irq(NAND_INT);

	NDEBUG("===Int read page\n");	
	socle_nand_initial_buf(BUFFER_READ);		
	if (!(socle_nfc_dma_interrupt_page_read(nand_info, NAND_TEST_PAGE_ADDR + page_number, flash_info->page_size) & NAND_STATUS_READY))
		return -1;
	free_irq(NAND_INT);

	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_TX_ADDR,TEST_PATTERN_RX_ADDR, nand_info->flash_info->page_size))
		ret= -1;


	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) &~ (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);        

	return ret;
#endif
}

extern int 
powersave_test (int autotest)
{

	int ret = 0;
	u32 register_1 = 0;
	u32 register_2 = 0;

	nand_info->dma_on = 0;
	nand_info->ecc_on = 1;
	socle_nfc_set_ecc(nand_info);

	register_1 = socle_nfc_read(NF_SFR_FLCTRL);
	//entry power save mode
	socle_power_save_on_off(0);

	nand_info->ecc_on = 0;
	socle_nfc_set_ecc(nand_info);
	//leave power save mode
	socle_power_save_on_off(1);

	register_2 = socle_nfc_read(NF_SFR_FLCTRL);
	if (register_1 != register_2){
		NDEBUG(" 'Power save mode' didn't work !!!\n");		
		ret = -1;
	}

	return ret;
}

extern int
//autoboot_page_program_0th(void)
autoboot_page_program_0th(int autotest)		//jerry hsieh fix ; 2010/04/26
{
	//struct socle_nand_flash *flash_info = nand_info->flash_info;		//jerry hsieh mask	 ; 2010/04/26
	if(socle_nfc_block_erase(nand_info, 0)) {
		printf("Block 0 erase fail\n");
		return -1;
	}
	socle_nand_prepare_pattern((u8 *)nand_info->buf_addr, TEST_PATTERN_BUF_SIZE);
	return socle_nfc_page_program(nand_info, 0);
}
/*
static void show_nand_reg(void)
{
	u32 *ptr = (u32 *) 0x18141300;
	int i;
	printf("REGSTER:\n");
	for(i=0;i<8;i++) {
		printf("0x%08X ", *ptr++);
		if(i%4==3) printf("\n");
	}
}

static void show_data(void)
{
	u32 *ptr = (u32 *) 0x18140000;
	int i;
	printf("BUFFER:\n");
	for(i=0;i<40;i++) {
		printf("0x%08X ", *ptr++);
                if(i%4==3) printf("\n");
	}
}*/

extern int 
autoboot_test (void)
{
	int ret = 0;

	//Those setting match with image default setting
	nand_info->flash_info->page_size = 2048;
	nand_info->flash_info->sp_size = 64;
	
	socle_nand_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, TEST_PATTERN_BUF_SIZE);
#if 0//def NFC_CTRL_DEBUG
	show_nand_reg();
	show_data((u8 *)nand_info->buf_addr,nand_info->flash_info->page_size);
	show_spara_data((u8 *)nand_info->buf_addr, nand_info->flash_info->sp_size);
#endif

    	if(socle_nand_compare_pattern(nand_info->buf_addr,TEST_PATTERN_TX_ADDR, nand_info->flash_info->page_size)) 
		return -1;
	return ret;
}

//-------------Start of LookUp table TEST ------------------------------------------------


static int 
socle_nand_select_lookup_test (u8 channel)
{
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;
	u16 logic_block_num = 0;
	u16 physical_block_num = 0;

	u32 test_program_page_addr = socle_get_test_page_number();
	socle_nand_initial_buf(BUFFER_WRITE);
	if(socle_nfc_page_program_memory(nand_info, test_program_page_addr, flash_info->page_size))
		return -1;

	physical_block_num = test_program_page_addr/flash_info->page_per_block;
	logic_block_num = 0;
	socle_nfc_set_lookup( logic_block_num, physical_block_num, channel);
	
	u32 test_read_page_addr = test_program_page_addr & 0x007F;

	socle_nand_initial_buf(BUFFER_READ);
	if(socle_nfc_page_read_memory(nand_info, test_read_page_addr, flash_info->page_size))
		return -1;
	NDEBUG("Compare TX RX in %s_function\n",__func__);
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,TEST_PATTERN_TX_ADDR, flash_info->page_size))
		return -1;

	socle_nfc_release_all_lookup();
	return ret;
}

extern int 
fllookup_reg0 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(0);
	return ret;
}

extern int 
fllookup_reg1 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(1);
	return ret;
}

extern int 
fllookup_reg2 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(2);
	return ret;
}

extern int 
fllookup_reg3 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(3);
	return ret;
}

extern int 
fllookup_reg4 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(4);
	return ret;
}

extern int 
fllookup_reg5 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(5);
	return ret;
}

extern int 
fllookup_reg6 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(6);
	return ret;
}

extern int 
fllookup_reg7 (int autotest)
{
	int ret = 0;

	ret = socle_nand_select_lookup_test(7);
	return ret;
}
//-------------End of LookUp table TEST ------------------------------------------------



extern struct test_item_container nfc_ctrl_fllookup_test_container;

extern int fllookup_test(int autotest)
{
	int ret = 0;

	nand_info->dma_on =0;
	nand_info->ecc_on = 0;
	socle_nfc_set_ecc(nand_info);
	ret = test_item_ctrl(&nfc_ctrl_fllookup_test_container, autotest);
	return ret;

}

static void make_error_bit_pattern(u8 *ext_buf,u8 err_num, u8 sub_page_num)
{

	u32 *dbuf = (u32 *) ext_buf;
	u32 *dbuf_temp;
	int i;
	int j;

	dbuf_temp = dbuf;
	for (j = 0; j <= sub_page_num; j++) {
		dbuf = dbuf_temp + (128 * j );
		for (i = 0; i < err_num; i ++) {
			if ((*dbuf & 0x8000000) == 0)
				*dbuf |= 0x8000000;
			else
				*dbuf &= ~0x8000000;
			//NDEBUG("[0x%x]=0x%8x", dbuf,*dbuf );
			dbuf = dbuf + 1 ;
		}
	}
}
//#define debug_ecc
 static int 
 socle_nand_ecc_correction_test(u32 page_num1, u32 page_num2, u8 err_bit_num)
 {
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;

	nand_info->dma_on = 0;
	socle_nand_initial_buf(BUFFER_READ);
	socle_nfc_set_ecc(nand_info);
	if (socle_nfc_page_read_memory(nand_info, page_num1, flash_info->page_size+ flash_info->sp_size))
		return -1;

	make_error_bit_pattern((u8 *)TEST_PATTERN_RX_ADDR,err_bit_num, 0);// BitErr=(1~9 ), MainPageNum = (0~7)

	nand_info->ecc_on = 0;
	socle_nfc_set_ecc(nand_info);
	nand_info->dma_buffer = TEST_PATTERN_RX_ADDR;
	memset((char *) nand_info->buf_addr, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);
	if (socle_nfc_page_program_memory(nand_info,page_num2, flash_info->page_size+ flash_info->sp_size))
		return -1;
	
	nand_info->ecc_on = 1;
	socle_nfc_set_ecc(nand_info);
	socle_nand_initial_buf(BUFFER_READ);
	if (socle_nfc_page_read_memory(nand_info, page_num2, flash_info->page_size+flash_info->sp_size))
		return -1;
	
	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,TEST_PATTERN_TX_ADDR, flash_info->page_size))
		return -1;

    	return ret;
 }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/


extern int 
ecc_errbit1_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 1);
	return ret;
}

extern int 
ecc_errbit2_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 2);
	return ret;
}

extern int 
ecc_errbit3_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 3);
	return ret;
}

extern int 
ecc_errbit4_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 4);
	return ret;
}

extern int 
ecc_errbit5_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 5);
	return ret;
}

extern int 
ecc_errbit6_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 6);
	return ret;
}

extern int 
ecc_errbit7_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 7);
	return ret;
}

extern int 
ecc_errbit8_corrected(int autotest)
{
	int ret = 0;
	
	ret = socle_nand_ecc_correction_test( reference_page, target_page, 8);
	return ret;
}

extern int 
ecc_errbit9_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 9);
	return ret;
}

extern int 
ecc_errbit10_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 10);
	return ret;
}

extern int 
ecc_errbit11_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 11);
	return ret;
}

extern int 
ecc_errbit12_corrected(int autotest)
{
	int ret = 0;
	ret = socle_nand_ecc_correction_test( reference_page, target_page, 12);
	return ret;}

extern int 
ecc_errbit13_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 13);
	return ret;
}

extern int 
ecc_errbit14_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 14);
	return ret;
}

extern int 
ecc_errbit15_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 15);
	return ret;

}

extern int 
ecc_errbit16_corrected(int autotest)
{
	int ret = 0;

	ret = socle_nand_ecc_correction_test( reference_page, target_page, 16);
	return ret;
}

//*********************************************************************************
extern struct test_item_container nfc_ctrl_ecc_correct_test_container;

extern int 
bch4_ecc_corrected(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 0;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;
}

extern int 
bch8_ecc_corrected(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 1;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;
	
}

extern int 
bch16_ecc_corrected(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 2;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;

}

extern int 
hm_ecc_corrected(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 3;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;

}

//*********************************************************************************
extern struct test_item_container nfc_ctrl_ecc_correct_mode_test_container;

extern int 
ecc_correction_test(int autotest)
{
	int ret = 0;

	nand_info->dma_on = 0;
	nand_info->ecc_on = 1;
	socle_nfc_set_ecc(nand_info);
		
	reference_page = socle_get_test_page_number();
	target_page = reference_page+1;
	page_number++;
	
	socle_nand_initial_buf(BUFFER_WRITE);
	if (socle_nfc_page_program_memory(nand_info, reference_page, nand_info->flash_info->page_size))
		return -1;
	
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_mode_test_container, autotest);

	nand_info->ecc_on = 0;
	socle_nfc_set_ecc(nand_info);
	return ret;
}
//*********************************************************************************

extern int
misc_nand_lock_unlock_test(int autotest)
{

	socle_nfc_lock(nand_info);

	return 0;
}

extern int
misc_nand_cache_write_test(int autotest)
{
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	u32 page_num = 5;

	socle_nand_initial_buf(BUFFER_WRITE);
	if (!autotest) {
		printf("cache Write Page: \n");
		scanf("%d\n", &page_num);
	}
	socle_nfc_set_ecc(nand_info);
	if (!(socle_nfcg_page_program_cache(nand_info, page_num,flash_info->page_size) & NAND_STATUS_READY))
		return -1;
	
	if (!(socle_nfcg_page_program_cache1(nand_info, page_num+1,(u8 *)TEST_PATTERN_TX_ADDR,flash_info->page_size) & NAND_STATUS_READY))
		return -1;
	
	return 0;
}


extern int 
misc_nand_dma_read_write_test(int autotest)
{
	int ret = 0;
	u32 data_size = nand_info->flash_info->page_size;
	u32 data_offset = 0;

	if (!autotest){
		printf(" DMA data_size:\n");
		scanf("%d\n", &data_size);
		printf(" DMA data_offset:\n");
		scanf("%d\n", &data_offset);

	}

	nand_info->dma_on= 1;
	NDEBUG("DMA from External to Internal\n");	
	socle_nand_initial_buf(BUFFER_WRITE);	

	nand_info->dma_direct = NF_DMA_WRITE;
	if ((nand_info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (nand_info->dma_mode== NF_DMA_BURST_ADRR_DEC))
		nand_info->dma_buffer =	nand_info->dma_buffer +data_size-1 ;
	if (socle_nfc_dma_transfer(nand_info, data_size, data_offset))
		return -1;

	NDEBUG("DMA from Internal to External\n");
	nand_info->dma_buffer = TEST_PATTERN_RX_ADDR;
	memset((char *) TEST_PATTERN_RX_ADDR, 0xff, TEST_PATTERN_BUF_SIZE + MAX_SPARE_SIZE);		

	nand_info->dma_direct = NF_DMA_READ;
	if ((nand_info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (nand_info->dma_mode== NF_DMA_BURST_ADRR_DEC))
		nand_info->dma_buffer =	nand_info->dma_buffer +data_size-1;
	if (socle_nfc_dma_transfer(nand_info, data_size, data_offset))
		return -1;

	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_TX_ADDR,TEST_PATTERN_RX_ADDR, data_size))
		ret= -1;

	nand_info->dma_on= 0;	
	return ret;

}

#if 0
extern int 
misc_nand_badblock_scan_test(int autotest)
{
	int ret = 0;

	ret = socle_nand_badblock_scan_test(nand_info);
	return ret;
}


extern int
misc_nand_protect_setting_test(int autotest)
{
	int ret = 0;
	u32 begin_protect_area = 0;
	u32 end_protect_area = 0;

	printf("Begin Protect Block Number:\n");
	scanf("%d\n", &begin_protect_area);
	printf("End Protect Block Number:\n");
	scanf("%d\n", &end_protect_area);

	
	socle_nfc_set_protected_area(begin_protect_area, end_protect_area);
	
	return ret;

}

extern int 
misc_nand_block_erase_test (int autotest)
{
	int ret = 0;
	u32 start_block_num = 0;
	u32 end_block_num = 0;
	u32 i =0;	

	socle_nfc_set_ecc(nand_info);
	if (autotest) {
		start_block_num = 5;
		end_block_num =5;
	} else {
		printf("Erase Block: Star_of_BlockNum\n");
		scanf("%d\n", &start_block_num);
		printf("Erase Block: End_of_BlockNum\n");
		scanf("%d\n", &end_block_num);
	}

	for (i = start_block_num; i<=end_block_num; i++) {
              NDEBUG(" Erase Block =%d\n", i);
		if (socle_nfc_block_erase(nand_info, i))
			return -1;
	}
	return ret;
}
#endif
//*********************************************************************************
extern u8 
socle_nfc_page_program_memory(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	//DMA on
	if(info->dma_on) {
		info->dma_direct = NF_DMA_WRITE;
		if ((info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (info->dma_mode== NF_DMA_BURST_ADRR_DEC))
			info->dma_buffer =	info->dma_buffer +data_size-1;
		socle_nfc_prepare_dma_transfer(info, data_size, 0);
#if 0 //dma trigger off
		if (socle_nfc_dma_transfer(nand_info, flash_info->page_size, 0))
			return -1;
#endif
	}
	else {//DMA off
		memcpy((char *)info->buf_addr, (char *)info->dma_buffer, data_size);
	}
	//program page
	if (socle_nfc_page_program(info, page_number))
		return -1;

	return 0;
}

extern u8 socle_nfc_finish_dma_transfer(void);		//jerry hsieh add ; 2010/04/26

extern u8 
socle_nfc_page_read_memory(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	//DMA on
	if(info->dma_on) {
		info->dma_direct = NF_DMA_READ;
		if ((info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (info->dma_mode== NF_DMA_BURST_ADRR_DEC))
			info->dma_buffer = info->dma_buffer +data_size-1;
		socle_nfc_prepare_dma_transfer(info, data_size, 0);
#if 0 //dma trigger off
		if (socle_nfc_dma_transfer(nand_info, flash_info->page_size, 0))
			return -1;
#endif
	}
	
	//page read 
	socle_nfc_page_read(info, page_number);

	//DMA on
	if(info->dma_on) {
		socle_nfc_finish_dma_transfer();
	} else {
		memcpy((char *)info->dma_buffer,(char *)info->buf_addr, data_size );	
	}
	
	
	return 0;
}

extern int 
socle_nand_page_read_program_loopback_test(void)		
{
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;
	u32 nf_test_page_addr;
	
	nf_test_page_addr = socle_get_test_page_number();
	
	socle_nand_initial_buf(BUFFER_WRITE);
	if(socle_nfc_page_program_memory(nand_info, nf_test_page_addr, flash_info->page_size))
		return -1;

	socle_nand_initial_buf(BUFFER_READ);	
	if(socle_nfc_page_read_memory(nand_info, nf_test_page_addr, flash_info->page_size))
		return -1;

	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,TEST_PATTERN_TX_ADDR, flash_info->page_size))
		return -1;

	return ret;

}

//*********************************************************************************

extern u8
socle_nfc_page_program1_memory(struct socle_nand_info *info, u32 page_num, u32 page_offset, u8 *buf, u32 buf_size)
{
	if(socle_nfc_page_program1(info,page_num,page_offset))
		return -1;
	// Copy bytes to data register
	if (socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_WRITE))
		return -1;
	return 0;
}

extern u8 
socle_nfc_page_read1_memory(struct socle_nand_info *info, u32 page_num, u32 page_offset, u8 *buf, u32 buf_size)
{
	if (socle_nfc_page_read1(info,page_num,page_offset))
		return -1;
	// Copy bytes to buffer 
	if (socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_READ))
		return -1;
	return 0;
}

extern int 
socle_nand_word_read_program_loopback_test(void)		
{
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;
	u32 test_page_number;
	
	test_page_number = socle_get_test_page_number();
	NDEBUG("current page =0x%x\n",test_page_number);
	//program page
	socle_nand_initial_buf(BUFFER_WRITE);
	if (socle_nfc_page_program1_memory(nand_info, test_page_number, 0, (u8 *) nand_info->dma_buffer, flash_info->page_size))
		return -1;
	// FSM go to IDLE STATE
	socle_nfc_write_command(nand_info);
	
	//page read 
	socle_nand_initial_buf(BUFFER_READ);
	if (socle_nfc_page_read1_memory(nand_info, test_page_number, 0, (u8 *)nand_info->dma_buffer, flash_info->page_size))
		return -1;
	// FSM go to IDLE STATE
	socle_nfc_read_command(nand_info);
	
	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,TEST_PATTERN_TX_ADDR, flash_info->page_size))
		return -1;

	return ret;

}
/******************************************************************************************/
extern u8
socle_nfc_random_data_input_memory(struct socle_nand_info *info, u32 page_offset, u8 *buf, u32 buf_size)
{
	if(socle_nfc_random_data_input(info,page_offset))
		return -1;
	// Copy bytes to data register
	if (socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_WRITE))
		return -1;
	return 0;
}

extern u8 
socle_nfc_random_data_read_memory(struct socle_nand_info *info, u32 page_offset, u8 *buf, u32 buf_size)
{
	if (socle_nfc_random_data_read(info, page_offset))
		return -1;
	// Copy bytes to buffer 
	if (socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_READ))
		return -1;

	return 0;
}



extern int
socle_nand_word_random_read_program_loopback_test(void)		
{
	struct socle_nand_flash *flash_info = nand_info->flash_info;
	int ret = 0;
	u32 test_page_number;
	
	test_page_number = socle_get_test_page_number();
	NDEBUG("current page =0x%x\n",test_page_number);
	//program page
	socle_nand_initial_buf(BUFFER_WRITE);
	//program last word of page
	if (socle_nfc_page_program1_memory(nand_info, test_page_number, flash_info->page_size-4, (u8 *)nand_info->dma_buffer+flash_info->page_size-4, 4))
		return -1;
	//program page from first word, except last word
	if (socle_nfc_random_data_input_memory(nand_info, 0, (u8 *)nand_info->dma_buffer, flash_info->page_size-4))
		return -1;
	// FSM go to IDLE STATE
	socle_nfc_write_command(nand_info);
	
	//page read 
	socle_nand_initial_buf(BUFFER_READ);
	//read last word of page
	if (socle_nfc_page_read1_memory(nand_info, test_page_number, flash_info->page_size-4, (u8 *)nand_info->dma_buffer+flash_info->page_size-4,4))
		return -1;
	//read page from first word, except last word
	if (socle_nfc_random_data_read_memory(nand_info, 0, (u8 *)nand_info->dma_buffer, flash_info->page_size-4))
		return -1;
	// FSM go to IDLE STATE
	socle_nfc_read_command(nand_info);

	//compare
	if(socle_nand_compare_pattern(TEST_PATTERN_RX_ADDR,TEST_PATTERN_TX_ADDR, flash_info->page_size))
		return -1;

	return ret;
}

extern int socle_block_erase_test(int autotest)
{
	int block_num;

	printf("Please Input which block you want to erase: ");
	scanf("%d", &block_num);

	socle_nfc_block_erase(nand_info, block_num);

	printf("Finish to erase %dth block\n", block_num);
	return 0;
}

extern struct test_item_container nfc_ctrl_transfer_dma_mode_test_container;

extern int 
dma_transfer_size_8bit(int autotest)
{
	int ret = 0;
	nand_info->dma_size= NF_DMA_SIZE_8;

	ret = test_item_ctrl(&nfc_ctrl_transfer_dma_mode_test_container, autotest);
	
	return ret;

}

extern int 
dma_transfer_size_16bit(int autotest)
{
	int ret = 0;
	nand_info->dma_size= NF_DMA_SIZE_16;

	ret = test_item_ctrl(&nfc_ctrl_transfer_dma_mode_test_container, autotest);
	return ret;

}

extern int 
dma_transfer_size_32bit(int autotest)
{
	int ret = 0;
	nand_info->dma_size= NF_DMA_SIZE_32;

	ret = test_item_ctrl(&nfc_ctrl_transfer_dma_mode_test_container, autotest);
	//ret = socle_nand_page_read_program_loopback_test();	
	return ret;

}

extern struct test_item_container nfc_ctrl_dma_transfer_size_test_container;

extern int 
dma_bust_single_decrement(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_SINGLE_ADRR_DEC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
data_bust_unspecifed_addr_decrement(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_BURST_ADRR_DEC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
dma_bust_single_increment(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_SINGLE_ADRR_INC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
data_bust_unspecifed_addr_increment(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_BURST_ADRR_INC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
dma_4bit_bust_addr_increment(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_BURST_4_ADDR_INC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
dma_8bit_bust_addr_increment(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_BURST_8_ADDR_INC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
dma_16bit_bust_addr_increment(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_BURST_16_ADDR_INC;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern int 
dma_stream_bust_addr_const(int autotest)
{
	int ret = 0;
	nand_info->dma_mode= NF_DMA_STREAM_BURST;

	ret = socle_nand_page_read_program_loopback_test();
	
	return ret;

}

extern struct test_item_container nfc_ctrl_transfer_dma_mode_test_container;

extern int 
instruction_with_dma_test(int autotest)
{
	int ret = 0;
	nand_info->dma_on= 1;
	socle_nfc_set_dma(nand_info);

	ret = test_item_ctrl(&nfc_ctrl_dma_transfer_size_test_container, autotest);
	
	return ret;

}

extern struct test_item_container nfc_ctrl_event_container;

extern int 
instruction_without_dma_test(int autotest)
{
	int ret = 0;
	nand_info->dma_on = 0;
	socle_nfc_set_dma(nand_info);
	ret = test_item_ctrl(&nfc_ctrl_event_container, autotest);

	return ret;
}

extern struct test_item_container nfc_ctrl_data_transfer_dma_test_container;

extern int 
bch4_ecc(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 0;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;
}

extern int 
bch8_ecc(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 1;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;
	
}

extern int 
bch16_ecc(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 2;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;

}

extern int 
hm_ecc(int autotest)
{
	int ret = 0;
	nand_info->ecc_mode = 3;
	ret = test_item_ctrl(&nfc_ctrl_ecc_correct_test_container, autotest);
	return ret;

}

//*********************************************************************************


extern struct test_item_container nfc_ctrl_data_transfer_dma_test_container;

extern int 
instruction_test(int autotest)
{
	int ret = 0;

	page_number = 0;
	ret = test_item_ctrl(&nfc_ctrl_data_transfer_dma_test_container, autotest);
	return ret;

}

extern struct test_item_container nfc_ctrl_feature_test_container;

extern struct test_item_container nfc_ctrl_select_chip_test_container;

int nfc_ctrl_test (int autotest)
{
	int ret = 0;
	int auto_boot_fail = 0;

	if (nfc_mode == fpga_axi) {
		nand_info->io_base = 0x30040000 + NF_SFR_ADDRESS_OFFSET;
		nand_info->buf_addr = 0x30040000;
	}else if (nfc_mode == fpga_ahb) {
		nand_info->io_base = 0x20040000 + NF_SFR_ADDRESS_OFFSET;
		nand_info->buf_addr = 0x20040000;
	}else {
		nand_info->io_base = NF_CONTROLLER_ADDRESS + NF_SFR_ADDRESS_OFFSET;
		nand_info->buf_addr = NF_CONTROLLER_ADDRESS;
	}
	mtd->priv = &nand_info;

	printf(	"Warning: After testing, Diagnostic will erase offset 0x20000 block.\n"
		"This block storage U-boot Environment information.\n"
		"You must re-enter `saveenv` in U-boot.\n");
#if defined(CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_NFC);
#endif

#ifndef SKIP_AUTO_BOOT_TEST
	if (autoboot_test () != 0) 
		printf("Auto Boot Up Test... [Fail]\n");
	else
		printf("Auto Boot Up Test... [Pass]\n");
#endif

	socle_nand_initial_buf(BUFFER_INITIAL);
	socle_nand_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, TEST_PATTERN_BUF_SIZE);

	socle_nfc_init();

	/* Select the device */
	socle_nfc_select_chip (mtd, 0);
	if(nand_detect_flash(&socle_info))
		return -1;

/*	if (auto_boot_fail) {
		printf("Auto Boot Up Test... [Fail]\n");
		printf("Re-write page 0 for auto boot up test!!!\n");
		socle_nfc_configure_timing(NULL, 0);

		if(socle_nfc_block_erase(nand_info, 0)) {
			printf("%s: Erase fail\n", __func__);
			return -1;
		}
		if (socle_nfc_read_status(nand_info) & NAND_STATUS_FAIL) {
			printf("%s: Read Status fail\n", __func__);
			return -1;
		}

		nand_info->ecc_on = 0;
		socle_nfc_set_ecc(nand_info);
		socle_nand_initial_buf(BUFFER_WRITE);
		if (!(socle_nfc_page_program_memory(nand_info, 0, nand_info->flash_info->page_size) & NAND_STATUS_READY)) {
			printf("%s: Program Page fail\n", __func__);
			return -1;
		}
		printf("Please power shut down and re-start diagnostic for auto boot test again!!!\n");		
		return -1;
	}*/
	
	socle_init_badblock_remap(nand_info);

	nand_info->irq_on = 0;

	ret = test_item_ctrl(&nfc_ctrl_feature_test_container, autotest);	

	return ret;

}

int nfc_axi_test(int autotest)
{
	int ret = 0;
	nfc_mode = fpga_axi;
	ret = nfc_ctrl_test(autotest);
	return ret;
}

int nfc_ahb_test(int autotest)
{
	int ret = 0;
	nfc_mode = fpga_ahb;
	ret = nfc_ctrl_test(autotest);
	return ret;
}

extern struct test_item_container nfc_fpga_test_container;

int nfc_ctrl_fpga_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&nfc_fpga_test_container, autotest);
	return ret;

}



