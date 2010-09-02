#include <io.h>
#include "nand.h"
//#include "socle_nand.h"
#include "dependency.h"
#include "socle_nand_regs.h"


//#define NFC_DEBUG 
#ifdef NFC_DEBUG
#define NDEBUG(fmt, args...) printf(fmt, ## args)
#else
#define NDEBUG(fmt, args...)
#endif



extern u8 socle_nfc_finish_dma_transfer(void);

u8 NFCheckRegECC(void);

volatile int socle_nfc_int_flag = 0;
volatile u32 socle_nfc_int_state = 0;
volatile u32 socle_nfc_protect_err_flag = 0;
volatile u32 socle_nfc_fldata_access_err_flag = 0;


struct mtd_info socle_mtd_info;
struct socle_nand_info socle_info;
struct socle_nand_info *nand_info = &socle_info;

/******************************************************************************/
struct socle_nand_timing st_D08GW3C2C_timing = 
{

	.tWBns	= 100,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash D08GW3C2C = {
	.timing		= &st_D08GW3C2C_timing,

	.page_per_block	= 64,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 4096,
	.addr_cycle = 5,
	.bad_block_position = 1,
	.chip_id	= 0x9551d320,
};
/******************************************************************************/
struct socle_nand_timing st_D08GW3C2B_timing = 
{

	.tWBns	= 100,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash D08GW3C2B = {
	.timing		= &st_D08GW3C2B_timing,

	.page_per_block	= 128,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 8192,
	.addr_cycle = 5,
	.bad_block_position = 1,
	.chip_id	= 0xa514d320,
};

/******************************************************************************/
struct socle_nand_timing samsung_K9GAG08U0M_timing = 
{
	.tWBns	= 255,//100,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash K9GAG08U0M = {
	.timing		= &samsung_K9GAG08U0M_timing,

	.page_per_block	= 128,
	.page_size	= 4096,
	.sp_size = 128,
	.flash_width	= 8,
	.num_blocks	= 4096,
	.addr_cycle = 5,
	.bad_block_position = 0,
	.chip_id	= 0xb614d5ec,

};

/******************************************************************************/
struct socle_nand_timing samsung_K9K8G08U0B_timing = 
{
	.tWBns	= 255,//100,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash K9K8G08U0B = {
	.timing		= &samsung_K9K8G08U0B_timing,

	.page_per_block	= 64,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 8192,
	.addr_cycle = 5,
	.bad_block_position = 0,
	.chip_id	= 0x9551d3ec,

};

/******************************************************************************/
struct socle_nand_timing hynix_HY27UF081G2A_timing = 
{
	.tWBns	= 100,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash HY27UF081G2A = {
	.timing		= &hynix_HY27UF081G2A_timing,

	.page_per_block	= 64,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 1024,
	.addr_cycle = 4,
	.bad_block_position = 0,
	.chip_id	= 0x1d80f1ad,
};
/******************************************************************************/
struct socle_nand_timing hynix_HY27G088G5M_timing = 
{
	.tWBns	= 255,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,
};

static struct socle_nand_flash HY27G088G5M = {
	.timing		= &hynix_HY27G088G5M_timing,

	.page_per_block	= 64,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 8192,
	.addr_cycle = 5,
	.bad_block_position = 0,
	.chip_id	= 0x9580dcad,
};
/******************************************************************************/
#if 0//not verify yet
struct socle_nand_timing hynix_HY27G08AG5M_timing = 
{
	.tWBns	= 255,
	.tWHRns	= 60,
	.tWPns	= 12,
	.tRPns	= 12,
	.tWHns	= 10,};

static struct socle_nand_flash HY27G08AG5M = {
	.timing		= &hynix_HY27G08AG5M_timing,

	.page_per_block	= 64,
	.page_size	= 2048,
	.sp_size = 64,
	.flash_width	= 8,
	.num_blocks	= 16384,
	.addr_cycle = 5,
	.bad_block_position = 0,
	.chip_id	= 0x95c1d3ad,
};
#endif
/******************************************************************************/
//#define FLASH_TABLE_NUM 4
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static struct socle_nand_flash *builtin_flash_types[] = {
	&K9GAG08U0M,
	&K9K8G08U0B,	
	&D08GW3C2C,
	&D08GW3C2B,	
	&HY27UF081G2A,
	&HY27G088G5M,
};

/******************************************************************************/

inline void 
socle_nfc_write(u32 val, u32 reg)					//ok!
{
	iowrite32(val, reg +  nand_info->io_base);
}

inline u32 
socle_nfc_read(u32 reg)					//ok!
{
	return (ioread32(reg +  nand_info->io_base));
}

inline void 
socle_nfc_write_buf(u32 val, u32 reg)					//ok!
{
	iowrite32(val, reg +  nand_info->buf_addr);
}

inline u32 
socle_nfc_read_buf(u32 reg)					//ok!
{
	return (ioread32(reg + nand_info->buf_addr));
}
#ifdef FUNCTION_FOR_LINUX //-------------------------------------
static u8 
socle_read_byte(struct mtd_info *mtd)
{
	struct socle_nand_info *info = mtd->priv;
	u8 byte_value = 0xff; 
         	
	if (info->reg_star > info->reg_size) {
		info->reg_star = 0;
		info->reg_data = socle_nfc_read(NF_SFR_FLDATA);
	}   
	byte_value = (u8) (info->reg_data >> (8*info->reg_star));
	info->reg_star ++;        
 
	return byte_value;
}

static u16 
socle_read_word(struct mtd_info *mtd)
{
	struct socle_nand_info *info = mtd->priv;
	u16 word_value = 0xffff; 
         	
	if (info->reg_star > info->reg_size) {
		NDEBUG(" info->reg_star= 0x%x,info->reg_size= 0x%x\n",info->reg_star,info->reg_size);
		info->reg_star = 0;
		info->reg_data = socle_nfc_read(NF_SFR_FLDATA);
	}
	word_value = (u16) (info->reg_data >> (8*info->reg_star));
	info->reg_star = info->reg_star + 2 ;        
 
	return word_value;
}
#endif//-----------------------------------------------

extern void 
socle_nfc_select_chip(struct mtd_info *mtd, int chip)					//ok!
{
	struct socle_nand_info *info= mtd->priv;

	switch (chip) {
	case -1:
		/* Disable the NFC clock */
		socle_nfc_write(0, NF_SFR_FLPSM);
		break;
        default:
		/* Enable the NFC clock */
		info->chip_en = chip;
		socle_nfc_write(1, NF_SFR_FLPSM);
		socle_nfc_write((socle_nfc_read(NF_SFR_FLCTRL) & 0xFF00FFFF) | (info->chip_en<< 16), NF_SFR_FLCTRL);		  
		break;  
	}
}

extern u8 
socle_nfc_busy_check(void)					//ok!
{
	u16 timeout = 0xFFFF;

	/* wait until controller is busy*/
	while (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY) ;
	
	/* wait until memory is busy*/
	do {
		if(timeout == 0) {
			NDEBUG("..... NFC Busy Check Timeout\n");             
			return -1;
		}
		timeout--;
	}while (!(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_RNB_ST));

	return 0; 
}

extern u8 
socle_nfc_read_status(struct socle_nand_info *info)					//OK!
{
	u8 status[4];

	socle_nfc_write(NFG_CMD_READ_STATUS, NF_SFR_FLCOMM);
	*((u32*)status) = socle_nfc_read(NF_SFR_FLDATA);

	socle_nfc_write_buf((u32)status[3], 0);
	//NDEBUG("status = %x\n", status[3]);	
	return status[3];
}

void socle_protect_err(void)
{
	NDEBUG("Protect Area, access error!\n");
	socle_nfc_protect_err_flag = 1;
	//clear protect error state flag with useing a normal command
	socle_nfc_write(NFG_CMD_WRITE_PAGE, NF_SFR_FLCOMM);
}

void socle_fldatareg_access_err(void)
{
	/*
	  *	Access error occur, 
	  *	program1 command then read data register or
	  *	read1 command then write data register
	  */
	NDEBUG("Data register access mode error!\n");
	socle_nfc_fldata_access_err_flag = 1;
	//clear fldata access error state flag
	//socle_nfc_write(socle_nfc_read(NF_SFR_FLSTATE) &~ NF_STATE_ACC_ERR, NF_SFR_FLSTATE);
}

void socle_dma_err(void)
{
	/*
	  *	DMA access error
	  */
	  NDEBUG("DMA access error!\n");
}

void socle_buffer_access_success(void)
{
	/*
	  *	buffer access success
	  */
}
void socle_nfc_isr(void *data)					//OK!
{
	socle_nfc_int_flag = 1;
	socle_nfc_int_state = socle_nfc_read(NF_SFR_FLSTATE);
	NDEBUG("%s, state=0x%8x\n", __func__, socle_nfc_int_state );
	//clear all interrupt rise state flag
	//socle_nfc_write(socle_nfc_int_state &~ (NF_STATE_RNB_EDGE|NF_STATE_HINT), NF_SFR_FLSTATE);
	socle_nfc_write(0, NF_SFR_FLSTATE);
	if (socle_nfc_int_state & NF_STATE_PROT_ERR) {
		socle_protect_err();
	} else if(socle_nfc_int_state & NF_STATE_ACC_ERR) {
		socle_fldatareg_access_err();
	} else if(socle_nfc_int_state & NF_STATE_DMA_ERR_INT) {
		socle_dma_err();
	} else if(socle_nfc_int_state & NF_STATE_TRANS_INT) {
		socle_buffer_access_success();
	} else {
		/*
		  *	NF_STATE_RNB_EDGE
		  */
	}
	//NDEBUG("Exit nfc isr\n");
}

extern void
socle_init_nfc_int(void)
{
	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE) , NF_SFR_FLCTRL); 
	//clear all interrupt rise state flag, otherwise while enabling interrupt, this interrupt always rise
	socle_nfc_write(0, NF_SFR_FLSTATE);
	request_irq(NAND_INT, socle_nfc_isr, NULL);
}

extern void
socle_exit_nfc_int(void)
{
	free_irq(NAND_INT);
}
/******************************************************************************/

u32 
socle_nfc_select_timing_mask(u8 MaskLength)
{
	u32 Mask = 0;

	switch (MaskLength)
	{
		case 3:
			Mask = (BIT0 | BIT1 | BIT2);
			break;
		case 4:
			Mask = (BIT0 | BIT1 | BIT2 | BIT3);
			break;
		case 5:
			Mask = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4);
			break;
		case 6:
			Mask = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
			break;
		case 7:
			Mask = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6);
			break;
		default:
			break;
	}
	return Mask;
}

static void
socle_nfc_configure_timing(struct socle_nand_timing *Timings, u8 CheckDeviceDone)
{
	long int TWB, TWHR, TWP, TRP, TWH;
	u32 thclk =0; 

	u8 trp_shift = TIMER_BITS - 2;
	u8 twp_shift = trp_shift + TIMER_BITS - 1;
	u8 twhr_shift = twp_shift + TIMER_BITS - 1;
	u8 twb_shift = twhr_shift + TIMER_BITS - 1;

	u8 twh_mask =  TIMER_BITS - 2;
	u8 trp_mask = TIMER_BITS - 1;
	u8 twp_mask =  TIMER_BITS - 1;
	u8 twhr_mask = TIMER_BITS - 1;
	u8 twb_mask =  TIMER_BITS ;
	u32 MaskTemp = 0;

	if (CheckDeviceDone == 0) {
		socle_nfc_write(0xFFFFFFFF, NF_SFR_FLCONF);
		NDEBUG("Use Default Timing \n" );
		NDEBUG("cpu clock = %d\n",sq_scu_cpu_clock_get() );
		NDEBUG("ahp clock = %d\n",sq_scu_ahb_clock_get() );
		NDEBUG("NF_SFR_FLCONF=0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
		return;
	}
	NDEBUG("Calculate NAND Timing \n" );

	NDEBUG("cpu clock = %d\n",sq_scu_cpu_clock_get() );
	NDEBUG("ahp clock = %d\n",sq_scu_ahb_clock_get() );

	NDEBUG(" tWBns = %d\n",Timings->tWBns);

	thclk = NANOSec / sq_scu_ahb_clock_get();  //Need Measure from Board

	MaskTemp = socle_nfc_select_timing_mask(twb_mask);
	if ((TWB = Timings->tWBns / thclk - 1) < 0 )
		TWB = 0;
	if (Timings->tWBns > ((TWB + 1) * thclk))
		++TWB;

	TWB = (TWB &  MaskTemp);
	NDEBUG(" TWB =0x%x",TWB );

	MaskTemp = socle_nfc_select_timing_mask(twhr_mask);
	if((TWHR = Timings->tWHRns / thclk - 1) < 0)
		TWHR = 0;
	if (Timings->tWHRns > ((TWHR + 1) * thclk))
		++TWHR;

	TWHR = (TWHR &  MaskTemp);
	NDEBUG(" TWHR =0x%x\n",TWHR);

	MaskTemp = socle_nfc_select_timing_mask(twp_mask);
	if((TWP = Timings->tWPns / thclk - 2) < 0)
		TWP = 0;
	if (Timings->tWPns > ((TWP + 1) * thclk))
		++TWP;

	TWP = (TWP &  MaskTemp);
	NDEBUG(" TWP =0x%x\n",TWP );

	MaskTemp = socle_nfc_select_timing_mask(trp_mask);
	if ((TRP = Timings->tRPns / thclk - 1) < 0)
		TRP = 0;
	if (Timings->tRPns > ((TRP + 1) * thclk))
		++TRP;

	TRP = (TRP &  MaskTemp);
	NDEBUG(" TRP =0x%x\n",TRP );

	MaskTemp = socle_nfc_select_timing_mask(twh_mask);
	if ((TWH = Timings->tWHns / thclk - 1) < 0)
		TWH = 0;
	if (Timings->tWHns > ((TWH + 1) * thclk))
		++TWH;

	TWH = (TWH &  MaskTemp);
	NDEBUG(" TWH =0x%x\n",TWH );

	socle_nfc_write(((u32)TWB << twb_shift)
		| ((u32)TWHR << twhr_shift) | ((u32)TWP << twp_shift) | ((u32)TRP << trp_shift)
		| ((u32)TWH), NF_SFR_FLCONF);

	NDEBUG("NF_SFR_FLCONF= 0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
}



/******************************************************************************/

void socle_nfc_memory_protection(u8 Protect)					//ok!
{
	if (Protect==NF_UNPROTECT_WRITE)
		socle_nfc_write(socle_nfc_read(NF_SFR_FLCTRL) & ~(NF_CTRL_WRITE_PROT),NF_SFR_FLCTRL); // Set bit 23 = 0;
	else		//NF_PROTECT_WRITE
		socle_nfc_write(socle_nfc_read(NF_SFR_FLCTRL) | NF_CTRL_WRITE_PROT,NF_SFR_FLCTRL);    // Set bit 23 =1;
}
/******************************************************************************/

/******************************************************************************/
extern void 
socle_nfc_set_lookup(u16 log_block_number, u16 phy_block_number, u32 channel)					//ok!
{

	/*save related physical and logic address for Look up register*/
	socle_nfc_write(((u32)phy_block_number << 16) | log_block_number, NF_SFR_FLLOOKUP(channel));
	/*enable Lookup table*/
	socle_nfc_write((u32) ((0x1 << channel )& 0xFF)|socle_nfc_read(NF_SFR_FLLOOKUPEN), NF_SFR_FLLOOKUPEN);
}
/******************************************************************************/

/******************************************************************************/
extern u8 
socle_nfc_release_lookup(u16 log_block_number)					//ok!
{
	u8 i;
	u32 looken = socle_nfc_read(NF_SFR_FLLOOKUPEN);

	for(i = 0; i < 8; i++) {
		if ((looken & (1 << i)) && ((socle_nfc_read(NF_SFR_FLLOOKUP((i)) & 0xFFFF) == log_block_number))) {
			/* Clear table mapping data */
			socle_nfc_write(0, NF_SFR_FLLOOKUP(i));

			/*Disable Lookup register*/
			looken &= ~(1 << i);
			socle_nfc_write(looken, NF_SFR_FLLOOKUPEN);

			return 0;
		}
	}
	return 1;
}
/******************************************************************************/

/******************************************************************************/
void 
socle_nfc_release_all_lookup(void)					//ok!
{
	u8 i;

	for(i = 0; i < 8; i++) {
		socle_nfc_write(0, NF_SFR_FLLOOKUP(i));
	}
	socle_nfc_write(0, NF_SFR_FLLOOKUPEN);
}
/******************************************************************************/


/******************************************************************************/
u32 
socle_nfc_read_reg_ecc(void)					//ok!
{
	return socle_nfc_read(NF_SFR_FLECCSTATUS);
}
/******************************************************************************/
extern u8
socle_nfc_finish_dma_transfer(void)					//ok!
{
 	while(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_DMA_BUSY);

	if(socle_nfc_read(NF_SFR_FLDMACTRL) & NF_DMA_ERR_FLAG) {
		NDEBUG("DMA transfer error !!!! \n");
		return -1;
	}
	socle_nfc_write(0, NF_SFR_FLDMACTRL);	
	return 0;
}

extern void 					//ok!
socle_nfc_prepare_dma_transfer(struct socle_nand_info *info, u32 data_size,u32 data_offset)
{

	u32 dmactrl = 0;
	u32 dmacntr = 0;
	u32 dma_count = data_size;

	socle_nfc_write(0, NF_SFR_FLDMACTRL);     	
	while((socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_DMA_BUSY));
	
	if(info->dma_size == NF_DMA_SIZE_32)
		dma_count >>= 2;
	else if (info->dma_size == NF_DMA_SIZE_16)
		dma_count >>= 1;

	// set dma addr
	socle_nfc_write((u32) info->dma_buffer, NF_SFR_FLDMAADDR);	

	dmacntr = (((u32)(dma_count) << 16) & 0xFFFF0000) | ((u32)data_offset & 0xFFFF);
	socle_nfc_write(dmacntr, NF_SFR_FLDMACNTR); 
	socle_nfc_write(0, NF_SFR_FLSTATE);     
	
	// set dma direction
	dmactrl =  info->dma_direct | info->dma_mode | info->dma_size;
	socle_nfc_write(dmactrl, NF_SFR_FLDMACTRL);	

}

extern u8					//ok!, use in dma trigger off
socle_nfc_dma_transfer(struct socle_nand_info *info, u32 data_size, u32 data_offset)
{

	socle_nfc_prepare_dma_transfer(info, data_size, data_offset);
	//dma enable
	socle_nfc_write(socle_nfc_read( NF_SFR_FLDMACTRL) | NF_DMA_START_FLAG, NF_SFR_FLDMACTRL);

	 /* Wait for transfer finish */
	while((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_DMA_BUSY));

	while ((socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));

	return socle_nfc_finish_dma_transfer();
}

 extern void 			
socle_nfc_set_dma(struct socle_nand_info *info)
{
	//NDEBUG("SET dma= 0x%x\n",info->dma_on);
	if(info->dma_on) 
		socle_nfc_write(socle_nfc_read(NF_SFR_FLCTRL) | (NF_CTRL_DMA_IE | NF_CTRL_DMA_TRIGGER), NF_SFR_FLCTRL);
	else 
		socle_nfc_write(socle_nfc_read(NF_SFR_FLCTRL) & ~(NF_CTRL_DMA_IE | NF_CTRL_DMA_TRIGGER), NF_SFR_FLCTRL);

}
/*********************************************************************************************************/

/******************************************************************************/

/******************************************************************************/
#if 0	// unuseful
void socle_nfc_internal_buffer_transfer(u8 *Buffer, u16 BufferSize, u16 Offset, u8 Direction, u8 DMAEnable)
{
	u32 *DBuffer = (u32*)Buffer;
	u32 i = 0;

	if (DMAEnable == 0) {
		if(Direction == NF_TRANSFER_READ) {
			while(BufferSize) {
				*DBuffer = socle_nfc_read_buf(Offset + i);
				DBuffer ++;
				BufferSize -= 4;
				i += 4;
			}
		} else { // NF_TRANSFER_WRITE
			while(BufferSize) {
				socle_nfc_write_buf(*DBuffer, Offset + i);
				DBuffer++;
				BufferSize -= 4;
				i += 4;
			}
		}
	} else {
//        NFDMATransfer(Buffer, BufferSize, Offset, Direction);
	}
}
#endif
/******************************************************************************/
extern void 					//ok!
socle_nfc_set_protected_area(u16 begin_protect_area, u16 end_protect_area)
{
	socle_nfc_write(((u32)end_protect_area << 16) | ((u32)begin_protect_area), NF_SFR_FLPB0);
}

/******************************************************************************/

/******************************************************************************/
extern void 					//ok! why to design this function?
socle_nfc_get_protected_area(u16 *begin_protect_area, u16 *end_protect_area)
{
	u32 protect_area;

	protect_area = socle_nfc_read(NF_SFR_FLPB0);
	*begin_protect_area = (u16)(protect_area) & 0xFFFF;
	*end_protect_area = (u16)(protect_area>>16) & 0xFFFF;
}
/******************************************************************************/


/******************************************************************************/
#if 0 // move to ctrl.c
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
			//NDEBUG("read ( 0x%8x, 0x%8x )\n",(Controller->RegOffset + NF_SFR_FLDATA),*DBuffer );
			//NDEBUG("Read DBuffe[%d]=0x%x  ", BufferSize,*DBuffer );
			dbuf ++;
		}
	} else {// Copy bytes to data register
		while(buf_size--) {            
			socle_nfc_write( *dbuf, NF_SFR_FLDATA);
//			NDEBUG("write ( 0x%8x,0x%8x )\n",( NF_SFR_FLDATA), socle_nfc_read( NF_SFR_FLDATA));
//			NDEBUG("Write dbuffe[%d]=0x%x  ", buf_size,*dbuf );
			dbuf ++; /*address increase by 1*/
		}
	}
	return 0;
}
#endif
extern u8 					//ok
socle_nfc_page_read1(struct socle_nand_info *info,u32 page_num, u16 page_offset)
{
	struct socle_nand_flash *flash_info = info->flash_info;
    //modify to uniform, and driver maybe already set
	// if memory has 16 bit bus width then it is addressed by 2 bytes words
	if (flash_info->flash_width == 16)
		page_offset = page_offset >>1;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;

	// Address
	socle_nfc_write((page_num << (flash_info->column_addr_size)) + page_offset, NF_SFR_FLADDR0L);

	socle_nfc_write(page_num >> 16, NF_SFR_FLADDR0H);
	// Command
	socle_nfc_write(NFG_CMD_PAGE_READ_1, NF_SFR_FLCOMM);
/*
	// Wait for RnB signal
	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));
*/
	return 0;

}
/******************************************************************************/
/*//modify
extern int 
socle_nand_badblock_scan_test(struct socle_nand_info *info)
{
	struct socle_nand_flash *flash_info = info->flash_info;
	int ret = 0;    
	u8 data[4];
	u8 block_is_bad= 0;
	int i = 0;

	printf("=== Scan Bad Block from factory info\n",i);    
	for (i = 0; i < flash_info->num_blocks; i++) {
		block_is_bad = 0;
		switch (flash_info->bad_block_position){
			case 0:
				if (socle_nfc_page_read1(nand_info, i * flash_info->page_per_block,flash_info->page_size, data, 4))
					return  -1;

				if(data[0] != 0xFF){
					NDEBUG(" Bad page Number = %10d,",i * flash_info->page_per_block);
					block_is_bad = 1;
					break;
				}

		        	if (socle_nfc_page_read1(nand_info, i * flash_info->page_per_block + 1, flash_info->page_size, data, 4))
					return  -1;        
        
				if(data[0] != 0xFF) {
					NDEBUG(" Bad page Number = %10d,",i * flash_info->page_per_block);
					block_is_bad = 1;
				}
				break;
			case 1:
				if (socle_nfc_page_read1(nand_info, (i+1) * flash_info->page_per_block - 1,flash_info->page_size, data, 4))
					return  -1;

				if(data[0] != 0xFF) {
					NDEBUG(" Bad page Number = %10d,",(i+1) * flash_info->page_per_block - 1);
					block_is_bad = 1;
					break;
				}

		        	if (socle_nfc_page_read1(nand_info, (i+1) * flash_info->page_per_block - 2, flash_info->page_size, data, 4))
					return  -1;        
        
				if(data[0] != 0xFF){
					NDEBUG(" Bad page Number = %10d,",(i+1) * flash_info->page_per_block - 2);	
					block_is_bad = 1;
				}
				break;
		}
		 if (block_is_bad == 1)
		{
			printf("\nBad Block Number = %4d,",i);
			NDEBUG(" data= 0x%2x %2x %2x %2x\n",data[3],data[2],data[1],data[0]);
			ret = -1;
		}
	} 
       NDEBUG("\n");   
	return ret;

}*/

/******************************************************************************/
extern u8 
socle_nfcg_random_data_input2(struct socle_nand_info *info, u32 page_num,  u32 page_offset, u8 *buf, u32 buf_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	if (flash_info->flash_width == 16)
		page_offset /= 2;
	
	if (socle_nfc_busy_check())
		return -1;

	socle_nfc_write(0,NF_SFR_FLSTATE);
	// Address
	socle_nfc_write((page_num << (flash_info->column_addr_size)) + page_offset, NF_SFR_FLADDR1L);
	socle_nfc_write(page_num >> 16, NF_SFR_FLADDR1H);
	// Command
	socle_nfc_write(NFG_CMD_RANDOM_DI_PROG_1, NF_SFR_FLCOMM);

	// Copy bytes to data register//modify
	//if (socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_WRITE))
		//return -1;
	return 0;	
}

extern int 					//ok!
socle_nfc_write_command(struct socle_nand_info *info)
{
	socle_nfc_write(NFG_CMD_WRITE_PAGE, NF_SFR_FLCOMM);
	return 0;
}
extern int 					//ok!
socle_nfc_read_command(struct socle_nand_info *info)
{
	socle_nfc_write(NFG_CMD_READ_PAGE, NF_SFR_FLCOMM);
	return 0;
}

extern u8 	//wait to modify
socle_nfc_internal_data_move(struct socle_nand_info *info, u32 source_page_num, u32 target_page_num)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;
  
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write(source_page_num << (flash_info->column_addr_size), NF_SFR_FLADDR0L);
	socle_nfc_write( source_page_num >> 16, NF_SFR_FLADDR0H);
	// Command
	socle_nfc_write(NFG_CMD_READ_INTER_DM, NF_SFR_FLCOMM);

	// Wait for RnB signal
	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));

	if(socle_nfc_busy_check())
		return -1;
	socle_nfc_write(0,NF_SFR_FLSTATE);

	// Address
	socle_nfc_write( target_page_num << (flash_info->column_addr_size),NF_SFR_FLADDR1L);
	socle_nfc_write(  target_page_num >> 16,NF_SFR_FLADDR1H);

	// Command
	socle_nfc_write( NFG_CMD_PROGRAM_INTER_DM, NF_SFR_FLCOMM);

    
	if (socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;

	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));
	socle_nfc_write_command(info);	//why to do?

	if (!(socle_nfc_read_status(info) & NAND_STATUS_READY))
		return -1;

	return 0;
}
/******************************************************************************/
extern u8 
socle_nfc_internal_data_move1(struct socle_nand_info *info, u32 source_page_num, u32 target_page_num,  u32 page_offset, u8 *buf, u32 buf_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;
  
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write(source_page_num << (flash_info->column_addr_size), NF_SFR_FLADDR0L);
	socle_nfc_write( source_page_num >> 16, NF_SFR_FLADDR0H);
	// Command
	socle_nfc_write(NFG_CMD_READ_INTER_DM, NF_SFR_FLCOMM);

	// Wait for RnB signal
	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));

	if(socle_nfc_busy_check())
		return -1;
	socle_nfc_write(0,NF_SFR_FLSTATE);
//modify
	//if( socle_nfcg_random_data_input2(nand_info, target_page_num, page_offset, buf,buf_size))
		//return -1;
    
	if (socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;

	//while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));

	socle_nfc_write_command(nand_info);
	if (!(socle_nfc_read_status(nand_info) & NAND_STATUS_READY))
		return -1;



	return 0;
}

/******************************************************************************/
/******************************************************************************/
extern u8 
socle_nfcg_page_program_cache(struct socle_nand_info *info, u32 page_num, u16 buf_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;
	
	// Check if memory is not busy
	socle_nfc_busy_check();
    
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write(page_num << (flash_info->column_addr_size), NF_SFR_FLADDR1L);

	socle_nfc_write(page_num >> 16, NF_SFR_FLADDR1H);


	if(info->dma_on) {
		info->dma_direct = NF_DMA_WRITE;
		if ((info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (info->dma_mode== NF_DMA_BURST_ADRR_DEC))
			info->dma_buffer =	info->dma_buffer +buf_size-1;
		if (socle_nfc_dma_transfer(info, buf_size, 0))
			return -1;
	} else 
		memcpy((char *)info->buf_addr, (char *)info->dma_buffer, buf_size);

	// Command
	socle_nfc_write(NFG_CMD_PROGRAM_PAGE_CACHE, NF_SFR_FLCOMM);

	if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;

	return socle_nfc_read_status(nand_info);
}
/******************************************************************************/

/******************************************************************************/
extern u8 
socle_nfcg_page_program_cache1(struct socle_nand_info *info, u32 page_num, u8 *buf, u16 buf_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;


	// Check if memory is not busy
	socle_nfc_busy_check();
    
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write( page_num << (flash_info->column_addr_size), NF_SFR_FLADDR1L);
	socle_nfc_write(page_num >> 16, NF_SFR_FLADDR1H);

	// Command
	socle_nfc_write(NFG_CMD_PROGRAM_PAGE_1, NF_SFR_FLCOMM);

	// Copy bytes to data register//modify
	//socle_nfc_data_reg_transfer(buf, buf_size, NF_TRANSFER_WRITE);

	// Command
	socle_nfc_write(NFG_CMD_WRITE_CACHE, NF_SFR_FLCOMM);

	if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;

	// Check if memory is not busy
	socle_nfc_busy_check();
    
	return socle_nfc_read_status(nand_info);
}
/******************************************************************************/
extern u8				//OK!
socle_nfc_random_data_read(struct socle_nand_info *info, u16 page_offset)
{

	// if memory has 16 bit bus width then it is addressed by 2 bytes words
#if NF_ORGANIZATION == NF_16_BIT
	page_offset /= 2;
#endif

	// Check if memory is not busy
	socle_nfc_busy_check();
    
	// Address
	socle_nfc_write(page_offset, NF_SFR_FLADDR0L);

	// Command
	socle_nfc_write(NFG_CMD_RANDOM_DATA_READ, NF_SFR_FLCOMM);

	return 0;
}

/******************************************************************************/
extern u8 			//modify
socle_nfc_page_program1(struct socle_nand_info *info, u32 page_num, u32 page_offset)
{
	struct socle_nand_flash *flash_info = info->flash_info;
//modify to uniform    
	// if memory has 16 bit bus width then it is addressed by 2 bytes words
	if (flash_info->flash_width == 16)
		page_offset = page_offset >>1;
	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);
	// Address
	socle_nfc_write((page_num << (flash_info->column_addr_size)) + page_offset, NF_SFR_FLADDR1L);
	socle_nfc_write(page_num >> 16, NF_SFR_FLADDR1H);
	// Command
	socle_nfc_write( NFG_CMD_PROGRAM_PAGE_1,NF_SFR_FLCOMM);
	
	if (socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;
	
	return 0 ;
}
/******************************************************************************/

/******************************************************************************/
extern u8 		//ok!
socle_nfc_random_data_input(struct socle_nand_info *info, u32 page_offset)
{
	
//modify to uniform
#if NF_ORGANIZATION == NF_16_BIT
		page_offset /= 2;
#endif
	if (socle_nfc_busy_check())
		return -1;

	// Address
	socle_nfc_write(page_offset, NF_SFR_FLADDR1L);

	// Command
	socle_nfc_write(NFG_CMD_RANDOM_DI_PROG, NF_SFR_FLCOMM);

	return 0;	
}
/******************************************************************************/


/******************************************************************************/

extern void socle_power_save_on_off(u8 power_mode)
{
    socle_nfc_write(power_mode, NF_SFR_FLPSM);
}

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
extern u8 					//ok!
socle_nfc_check_ecc_register(void)
{

	u32 ecc_mask;
	u32 read_ecc;

	if (nand_info->flash_info->page_size == 2048) {
		ecc_mask = 0xF000F;
	} else {							//4096
		ecc_mask = 0xFF00FF;
	}
    	
	read_ecc = socle_nfc_read_reg_ecc();
	NDEBUG(" ECC Status = 0x%x\n", read_ecc );
	if (read_ecc & ecc_mask) {
		u8 error_flag = (u8)((read_ecc >> 16) & 0xFF);
		u8 error_correct_flag = (u8)(read_ecc & 0xFF);
        
		NDEBUG("\error_flag =0x%2x,ErrorCorrectFlag =0x%2x\n",error_flag,error_correct_flag);
		if (error_flag == error_correct_flag)
			return 1;

		return -1;
    }

    return 0;
}

/******************************************************************************/
extern u8 					//ok!
socle_nfc_reset(struct socle_nand_info *info)
{
	socle_nfc_write(NFG_CMD_RESET, NF_SFR_FLCOMM);
	return 0;
}

extern u8					//ok!
socle_nfc_page_read(struct socle_nand_info *info, u32 page_number)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR0L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR0H);
	// Command
	socle_nfc_write(NFG_CMD_PAGE_READ, NF_SFR_FLCOMM);

	//interrupt or pulling
	if(info->irq_on) {
		if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
			NDEBUG("Socle NAND Flash host: page read is timeout\n");
			return -1;
		}
	} else {
		while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	}
	//ecc on
	if(info->ecc_on) {
		u8 ecc_state = socle_nfc_check_ecc_register();
		if(ecc_state != 1 && ecc_state != 0)
			return -1;
	}
	
	return 0;
}
#if 0
extern u8
socle_nfc_interrupt_page_read(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR0L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR0H);


	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) &~ (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE) , NF_SFR_FLCTRL);        
	request_irq(NAND_INT, socle_nfc_isr, NULL);

	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE) , NF_SFR_FLCTRL);        
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_RNB_IE) , NF_SFR_FLCTRL);        	
	// Command
	socle_nfc_write(NFG_CMD_PAGE_READ, NF_SFR_FLCOMM);
	/* Wait for the transimission to be complete */
	if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
		NDEBUG("Socle NAND Flash host: page read is timeout\n");
		return -1;
	}
	//while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));

	if(info->dma_on) {

		info->dma_direct =  NF_DMA_READ;
		if ((info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (info->dma_mode== NF_DMA_BURST_ADRR_DEC))
			info->dma_buffer =	info->dma_buffer +data_size-1;
		if (socle_nfc_dma_transfer(info, data_size, 0))
			return -1;

	} else
		memcpy((char *)info->dma_buffer,(char *)info->buf_addr, data_size );

	return socle_nfc_read_status(info);

}

extern u8
socle_nfc_dma_interrupt_page_read(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;
	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) &~ (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);        
	request_irq(NAND_INT, socle_nfc_isr, NULL);
	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//Successfully
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//Successfully
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_DMA_IE|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//fail
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//fail

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR0L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR0H);

	info->dma_direct =  NF_DMA_READ;
	socle_nfc_prepare_dma_transfer(info, data_size, 0);


	
	socle_nfc_write(NFG_CMD_PAGE_READ, NF_SFR_FLCOMM);
	while(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_DMA_BUSY)
		NDEBUG("DMA Busy\n");

	/* Wait for the transimission to be complete */
	if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
		NDEBUG("Socle NAND Flash host: page read is timeout\n");
		return -1;
	}    

	while ((socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));
	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_TRANS_INT));

	socle_nfc_finish_dma_transfer();

	return socle_nfc_read_status(info);
}
#endif

extern u8 
socle_nfc_page_program(struct socle_nand_info *info, u32 page_number)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	if(socle_nfc_busy_check())
		return -1;

	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR1L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR1H);

	socle_nfc_write(NFG_CMD_PROGRAM_PAGE, NF_SFR_FLCOMM);

	//interrupt or pulling
	if(info->irq_on) {
		if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
			NDEBUG("Socle NAND Flash host: page read is timeout\n");
			return -1;
		}
	} else {
		while(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY);
		if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR) {
			socle_protect_err();
			return -1;
		}
		u32 read_status;
		while(!(read_status = socle_nfc_read_status(info)  & NAND_STATUS_READY));
		if(read_status & NAND_STATUS_FAIL)
			return -1;
	}
	return 0;
}
#if 0
extern u8 
socle_nfc_interrupt_page_program(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	if(socle_nfc_busy_check())
		return -1;


	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR1L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR1H);

	if(info->dma_on) {
		info->dma_direct = NF_DMA_WRITE;
		if ((info->dma_mode == NF_DMA_SINGLE_ADRR_DEC) | (info->dma_mode== NF_DMA_BURST_ADRR_DEC))
			info->dma_buffer =	info->dma_buffer +data_size-1;
		if (socle_nfc_dma_transfer(info, data_size, 0))
			return -1;
	} else
		memcpy((char *)info->buf_addr, (char *)info->dma_buffer, data_size);

	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) &~ (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE) , NF_SFR_FLCTRL);        
	request_irq(NAND_INT, socle_nfc_isr, NULL);

//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE) , NF_SFR_FLCTRL);        
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_RNB_IE) , NF_SFR_FLCTRL);        	
	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_PROT_IE) , NF_SFR_FLCTRL);        	
	socle_nfc_write(NFG_CMD_PROGRAM_PAGE, NF_SFR_FLCOMM);
	/* Wait for the transimission to be complete */
	if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
		NDEBUG("Socle NAND Flash host: page read is timeout\n");
		return -1;
	}
	if(socle_nfc_busy_check())
		return -1;
	if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		return -1;

	return socle_nfc_read_status(info);
	
}


extern u8
socle_nfc_dma_interrupt_page_program(struct socle_nand_info *info, u32 page_number, u32 data_size)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	// Check if memory is not busy
	if(socle_nfc_busy_check())
		return -1;
	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) &~ (NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN|NF_CTRL_PROT_IE|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);        
	request_irq(NAND_INT, socle_nfc_isr, NULL);

	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_RNB_IE|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//Successfully
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_DMA_IE|NF_CTRL_DMA_TRIGGER|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//Successfully
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_DMA_IE|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//fail
//	socle_nfc_write(socle_nfc_read( NF_SFR_FLCTRL) | (NF_CTRL_INT_EN|NF_CTRL_TRANS_COMPLETE) , NF_SFR_FLCTRL);//fail

	// Address
	socle_nfc_write(page_number << (flash_info->column_addr_size), NF_SFR_FLADDR1L);
	socle_nfc_write(page_number >>  16, NF_SFR_FLADDR1H);

	info->dma_direct =  NF_DMA_WRITE;
	socle_nfc_prepare_dma_transfer(info, data_size, 0);
	
	socle_nfc_write(NFG_CMD_PROGRAM_PAGE, NF_SFR_FLCOMM);
	while(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_DMA_BUSY)
		NDEBUG("DMA Busy\n");

	/* Wait for the transimission to be complete */
	if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
		NDEBUG("Socle NAND Flash host: page read is timeout\n");
		return -1;
	}    


//	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_RNB_EDGE));
	while ((socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY));
	while (!(socle_nfc_read( NF_SFR_FLSTATE) & NF_STATE_TRANS_INT));

//	socle_nfc_write(socle_nfc_read( NF_SFR_FLDMACTRL) | NF_DMA_START_FLAG, NF_SFR_FLDMACTRL);

	socle_nfc_finish_dma_transfer();

	return socle_nfc_read_status(info);
}
#endif

extern u8
socle_nfc_lock(struct socle_nand_info *info)
{
	u32 temp_data,temp_reg;
	u8 status[4];
	
	if(socle_nfc_busy_check())
		return -1;
	
	socle_nfc_write(NFG_CMD_READ_STATUS, NF_SFR_FLCOMM);
	*((u32*)status) = socle_nfc_read(NF_SFR_FLDATA);
	
	NDEBUG("status[0] =0x%x,status[1] =0x %x,status[2] =0x%x,status[3] =0x%x\n", status[0],status[1],status[2],status[3]);	
	
	socle_nfc_write(NFST_COM_BL_LOCK, NF_SFR_FLCOMM);
	socle_nfc_write(NFST_COM_READ_BL_LOCK_STAT, NF_SFR_FLCOMM);

	temp_data = socle_nfc_read(NF_SFR_FLDATA);
	temp_reg = socle_nfc_read(NF_SFR_FLSTATE);
	NDEBUG("NF_SFR_FLDATA=0x%x\n", temp_data);	
	NDEBUG("NF_SFR_FLSTATE=0x%x\n", temp_reg);

	temp_data =temp_data & 0xff;
	if (temp_data & 0x80)
		printf("un protect!!!\n");
	else
		printf(" protect!!!\n");

	socle_nfc_write(NFG_CMD_READ_STATUS, NF_SFR_FLCOMM);
	*((u32*)status) = socle_nfc_read(NF_SFR_FLDATA);
	
	NDEBUG("status[0] =0x%x,status[1] =0x %x,status[2] =0x%x,status[3] =0x%x\n", status[0],status[1],status[2],status[3]);	

		
	if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR)
		NDEBUG("Reg Status= 0x%x: Protect  happen \n", socle_nfc_read(NF_SFR_FLDATA));

	return 0;
}

extern u8 
socle_nfc_block_erase(struct socle_nand_info *info, u32 block_number)
{
	struct socle_nand_flash *flash_info = info->flash_info;
	
	if(socle_nfc_busy_check())
		return -1;
	// clear status register
	socle_nfc_write(0, NF_SFR_FLSTATE);
	if(flash_info->page_per_block== 128)
		socle_nfc_write(block_number << (flash_info->column_addr_size + 7), NF_SFR_FLADDR1L);
	else if (flash_info->page_per_block== 64)
		socle_nfc_write(block_number <<  (flash_info->column_addr_size + 6), NF_SFR_FLADDR1L);
	else
		return -1;
	socle_nfc_write(block_number >> 16, NF_SFR_FLADDR1H);

	// Command
	socle_nfc_write(NFG_CMD_BLOCK_ERASE, NF_SFR_FLCOMM);

	//interrupt or pulling
	if(info->irq_on) {
		if (sq_wait_for_int(&socle_nfc_int_flag, 10)) {
			NDEBUG("Socle NAND Flash host: page read is timeout\n");
			return -1;
		}
	}
	else {
		if (socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_PROT_ERR) {
			printf("%s: line %d, NF_STATE_PROT_ERR\n", __func__, __LINE__);
			return -1;
		}
#if 1
		if( socle_nfc_busy_check()) {
			printf("%s: Device Busy\n", __func__);
			return -1;
		}
#else
		while(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_FSM_BUSY);
		
		if (!(socle_nfc_read(NF_SFR_FLSTATE) & NF_STATE_RNB_ST)) {
			printf("%s: line %d, NF_STATE_RNB_ST\n", __func__, __LINE__);
			return -1;
		}
#endif
	}
	return 0;
}
/***********************************************************************************/

extern void 
socle_nfc_set_ecc(struct socle_nand_info *info)
{
	u32 flctr = socle_nfc_read(NF_SFR_FLCTRL);
	
	flctr &= ~(NF_CTRL_ECC_EN); 
	flctr |= (info->ecc_on == 1)? NF_CTRL_ECC_EN:0;
	info->reg_flctr = flctr;
	socle_nfc_write(flctr, NF_SFR_FLCTRL);
}

static int 
socle_nand_config_flash(struct socle_nand_info *info,
				     struct socle_nand_flash *f)
{

	u32 flctr = 0; 

	socle_nfc_write(0, NF_SFR_FLCTRL);

      	info->ecc_on = 0;
	socle_nfc_set_ecc(info);
       info->dma_on = 0;
	NDEBUG("===== Flash Information ======\n");
	NDEBUG("flash_width		=  %d\n",f->flash_width);
	NDEBUG("page_per_block	=  %d\n",f->page_per_block);
	NDEBUG("sp_size			=  %d\n",f->sp_size);
	NDEBUG("page_size		=  %d\n",f->page_size);
       f->column_addr_size 	= 12 - (f->flash_width / 16) + (f->page_size / 4096);
	NDEBUG("column_addr_size=  %d \n",f->column_addr_size);   	
	NDEBUG("addr_cycle=  %d\n",f->addr_cycle);   	
	
	socle_nfc_write(flctr &= ~NF_CTRL_INT_EN, NF_SFR_FLCTRL);/* disable all interrupts */
	if (f->page_size != 4096 && f->page_size != 2048)
		return 1;

	if (f->flash_width != 16 && f->flash_width != 8)
		return 1;

	flctr |= (f->page_per_block == 128) ? NF_CTRL_BLOCK_SIZE : 0;
	flctr |= (f->page_size == 2048) ? NF_CTRL_PAGE_SIZE : 0;
	flctr |= (f->flash_width == 16) ? NF_CTRL_IO_WIDTH : 0;
	
	flctr &= ~NF_CTRL_SP_SIZE; /* disable spare by default */
	flctr &= ~NF_CTRL_COMMON_RNB_LINE;          


	flctr |= (f->addr_cycle == 5) ? NF_CTRL_ADDR_CYCLE : 0;

	info->reg_flctr = flctr;
	socle_nfc_write(flctr, NF_SFR_FLCTRL);
	NDEBUG(" Afterflctr = 0x%x\n",flctr);

	info->flash_info = f;
	return 0;
}


extern u8  //ok
socle_nfc_readid(struct socle_nand_info *info, u32 *id)
{
	u32 tmp1, tmp2;
	socle_nfc_write(NFG_CMD_RESET, NF_SFR_FLCOMM);
	socle_nfc_busy_check();
	// Command
	socle_nfc_write(NFG_CMD_READ_ID, NF_SFR_FLCOMM);
	socle_nfc_busy_check();
	tmp1 = *id = socle_nfc_read(NF_SFR_FLDATA);
	socle_nfc_write_buf(tmp1,0);
	tmp2 = socle_nfc_read(NF_SFR_FLDATA);
	socle_nfc_write_buf(tmp2,4);
	NDEBUG(" readid 1: 0x%x\n" , tmp1);
	NDEBUG(" readid 2: 0x%x\n" , tmp2);
	return 0;
}

#define BAD_BLOCK_INFO_BLOCK_NUM(num_blocks) (num_blocks-1)
#define BAD_BLOCK_REMAPPED_FLAG 0x00000000

extern int
check_remapped(struct socle_nand_info *info)
{
	struct socle_nand_flash *flash_info = info->flash_info;
	int bad_block_info_page_number, flag;
	bad_block_info_page_number = BAD_BLOCK_INFO_BLOCK_NUM(flash_info->num_blocks)*flash_info->page_per_block;

	socle_nfc_page_read(info, bad_block_info_page_number);
	//Read OOB 0th word, then get bad block remaped flag
	flag = socle_nfc_read_buf(flash_info->page_size);
	if( flag == BAD_BLOCK_REMAPPED_FLAG )//-
		return 1;
	return 0;
}

extern int
factory_is_bad_block(int bad_block_num) 
{
	/*
	  *	FIXME
	  */
	return 0;
}

extern int
socle_get_bad_block_info_page_num(struct socle_nand_flash *flash_info, int block_num)
{
	int bad_block_info_block_num = BAD_BLOCK_INFO_BLOCK_NUM(flash_info->num_blocks);
	int num_word_per_page = flash_info->page_size/4;
	int bad_block_info_page_number = bad_block_info_block_num*flash_info->page_size;
	return bad_block_info_page_number + block_num/num_word_per_page;
}

extern void
socle_set_bad_block_info(struct socle_nand_info *info, u32 page_num, u32 page_offset)
{
	socle_nfc_page_program1(info, page_num, page_offset);
	socle_nfc_write(BAD_BLOCK_REMAPPED_FLAG , NF_SFR_FLDATA);
	socle_nfc_write_command(info);
}

extern int
socle_set_bad_block(struct socle_nand_info *info, int bad_block_num)
{
	struct socle_nand_flash *flash_info = info->flash_info;
	int num_word_per_page = flash_info->page_size/4;
	
	socle_set_bad_block_info(info, 
					socle_get_bad_block_info_page_num(flash_info, bad_block_num), 
					bad_block_num%num_word_per_page);
	return 0;
}

extern int
socle_init_badblock_remap(struct socle_nand_info *info)
{
	struct socle_nand_flash *flash_info = info->flash_info;

	/*
	  *	FIXME
	  */
	return 0;

	int bad_block_num;
	
	//check remaped
	if(check_remapped(info))
		return 0;

	info->ecc_on = 0;
	socle_nfc_set_ecc(info);

	int bad_block_info_block_num, page_number, bad_block_info_page_number;
	int num_word_per_page = flash_info->page_size/4;

	bad_block_info_block_num = BAD_BLOCK_INFO_BLOCK_NUM(flash_info->num_blocks);
	socle_nfc_block_erase(info, bad_block_info_block_num);

	bad_block_info_page_number = socle_get_bad_block_info_page_num(flash_info, 0);
	//set remapped flag
	socle_set_bad_block_info(info, bad_block_info_page_number, flash_info->page_size);
	
	
	for(bad_block_num=0; bad_block_num< flash_info->num_blocks; bad_block_num++) {
		if(!factory_is_bad_block(bad_block_num))
			continue;
		page_number = socle_get_bad_block_info_page_num(flash_info, bad_block_num);

		socle_set_bad_block_info(info, page_number, bad_block_num%num_word_per_page);
	}

	return 0;
}


extern u8  //modify
nand_detect_flash(struct socle_nand_info *info)
{
	struct socle_nand_flash *f =nand_info->flash_info ;
	u32 id;
	int i;

	socle_nfc_configure_timing(f->timing,0);
	socle_nfc_readid(info, &id);
	//id = 0xb614d5ec;
/*
	for (i = 0; i < ARRAY_SIZE(builtin_flash_types); i++) {

		f = builtin_flash_types[i];

		if (socle_nand_config_flash(info, f))
			continue;

		if (id == f->chip_id) {
			if (f->timing->tWBns != 255){
				socle_nfc_configure_timing(f->timing,1);
	              	NDEBUG(" Check ID Again after Timing Re-set======\n"); 
				NDEBUG("NF_SFR_FLCONF= 0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
				socle_nfc_readid(info, &id);
			}else
				NDEBUG("NF_SFR_FLCONF= 0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
			return 0;
		}
	}
*/

	for (i = 0; i < ARRAY_SIZE(builtin_flash_types); i++) {
		f = builtin_flash_types[i];
		if (id == f->chip_id)
			break;
	}

	if( i == ARRAY_SIZE(builtin_flash_types) ) {
		printf("====Get Wrong NAND ID info !!!====\n"); 
		return -1;
	}

	socle_nand_config_flash(info, f);
	
	if (f->timing->tWBns != 255){
		socle_nfc_configure_timing(f->timing,1);
		NDEBUG(" Check ID Again after Timing Re-set======\n"); 
		NDEBUG("NF_SFR_FLCONF= 0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
		socle_nfc_readid(info, &id);
	}else
		NDEBUG("NF_SFR_FLCONF= 0x%x\n",socle_nfc_read(NF_SFR_FLCONF));
	return 0;
}


extern void 
socle_nfc_init(void)
{
	socle_nfc_write(0xFFFF, NF_SFR_FLCONF);

	socle_nfc_int_flag = 0;
//	request_irq(SOCLE_INTC_NAND, socle_nfc_isr, NULL);

	//Enable
	socle_nfc_write( NF_CTRL_INT_EN|NF_CTRL_ACC_ERR_EN| NF_CTRL_PROT_IE |
				NF_CTRL_RNB_IE |  NF_CTRL_TRANS_COMPLETE, NF_SFR_FLCTRL);

}

	

