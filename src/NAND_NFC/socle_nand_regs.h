#ifndef _SOCLE_NAND_REGS_H
#define _SOCLE_NAND_REGS_H
/******************************************************************************/


#define NANOSec 1000000000


/******************************************************************************/
#define NF_8_BIT    8    /** controller organization - 8bit */
#define NF_16_BIT   16   /** controller organization - 16bit */
/******************************************************************************/

struct socle_nand_timing {
	u8 tWBns;    /** WE# HIGH to busy in nano second */
	u8 tWHRns;   /** WE# HIGH to RE# LOW in nanosecond */
	u8 tWPns;    /** WE# pulse width in nano second */
	u8 tRPns;    /** RE# pulse width in nano second */
	u8 tWHns;    /** WE# pulse width HIGH in nano second */
};

struct socle_nand_flash {
	struct socle_nand_timing *timing; /* NAND Flash timing */

	u32 flash_width;   /* Width of Flash memory (DWIDTH_M) */
	u32 page_per_block;/* Pages per block (PG_PER_BLK) */
	u32 page_size;	   /* Page size in bytes (PAGE_SZ) */
	u32 sp_size;       /* SP size in byte  0: basic , 1 :  extend */
	u32 num_blocks;	   /* Number of physical blocks in Flash */
	u8 column_addr_size;
	u8 addr_cycle;
	u8 bad_block_position;
	u32 chip_id;
};

struct socle_nand_info {
	struct nand_chip *nand_chip;
	struct socle_nand_flash *flash_info;

	u32 chip_en;
	u8 ecc_mode;	//0:BCH4, 1:BCH8, 2:BCH16, 3:HM
	u8 ecc_on;		// 0: off , 1 : on
	u8 dma_on;		// 0:off , 1:on
	u32 dma_size;		// 00:8, 01:6, 10:resv, 11: 32 bit
	u32 dma_mode;		// 000: single, 001 ,,,,,,,,,
	u32 dma_direct;		// 0: rx, 1 tx
	u32 dma_buffer;

	u32 io_base;
	u32 buf_addr;

	u8 irq_on;
	
	u16 TransferType;    /** type of DMA transfer */
	u16 TransferSize;    /** size of DMA transfer */

	u32 reg_data;//for read_byte use 
	u8 reg_star;
	u8 reg_size;

	u32 reg_flctr;

	

	u8 power_mode; 
	int chip_delay;
	u32 wait_flags;
	
};


/******************************************************************************/
/** @name Special Function Registers. */
/******************************************************************************/
/** @{ */
/** Address of NAND flash memory space begin.
 * Slave interface of NANDFLASHCTRL occupies some memory space. 
 * In this space are mapped internal buffer (BUFFER) 
 * and a Special Function Registers (SFR). 
 */


#define NAND_INT    SQ_INTC_NAND0

//#define NF_CONTROLLER_ADDRESS   0x1CB20000
#define NF_CONTROLLER_ADDRESS   SQ_NAND0


#define NF_INTERNAL_BUFF_ADDR   (NF_CONTROLLER_ADDRESS )    /** NAND flash buffer */


#define NF_SFR_ADDRESS_OFFSET   0x1300    /** SFR offset address */

#define NF_SFR_FLCONF           	0x00    /** Flash configuration register */
#define NF_SFR_FLCTRL           		0x04    /** Flash control register */
#define NF_SFR_FLCOMM           	0x08    /** Flash command register */
#define NF_SFR_FLADDR0L         	0x0C    /** Flash address 0 register */
#define NF_SFR_FLADDR1L         	0x10    /** Flash address 1 register */
#define NF_SFR_FLDATA           	0x14    /** Flash data register */
#define NF_SFR_FLPB0            		0x18    /** Protected block 0 in Flash memory */
#define NF_SFR_FLSTATE          	0x1C    /** Controller status register */
#if 0
#define NF_SFR_FLLOOKUP0        	0x20    /** Look Up Table register 0. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP1        	0x24    /** Look Up Table register 1. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP2        	0x28    /** Look Up Table register 2. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP3        	0x2C    /** Look Up Table register 3. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP4        	0x30    /** Look Up Table register 4. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP5        	0x34    /** Look Up Table register 5. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP6        	0x38    /** Look Up Table register 6. LUT is used to address remapping. */
#define NF_SFR_FLLOOKUP7        	0x3C    /** Look Up Table register 7. LUT is used to address remapping. */
#else
#define NF_SFR_FLLOOKUP(x) 		(0x20 + 0x4 *x)
#endif
#define NF_SFR_FLECCSTATUS      	0x40    /** ECC status register. */

#define NF_SFR_FLADDR0H         	0x44    /** Flash address 0 register higher word. */
#define NF_SFR_FLADDR1H         	0x48    /** Flash address 1 register higher word. */

#define NF_SFR_FLLOOKUPEN       	0x4C    /** Enable look up registers during Nand flash memory addressing. */


#define NF_SFR_FLPSM            		0x54      


#define NF_SFR_FLDMAADDR        	0x80    /** DMA address register */
#define NF_SFR_FLDMACTRL        	0x84    /** DMA control register */
#define NF_SFR_FLDMACNTR        	0x88    /** DMA configuration register */
/** @} */
/******************************************************************************/



/******************************************************************************/
/** @name The flconf register masks. */
/******************************************************************************/
/** @{ */
#define NF_CONF_POWER_SAVE_MODE     0x0000    /** Power save mode */
#define NF_CONF_NORMAL_OPERATION    0x0001    /** Normal operation */
#define NF_CONF_HOLD_TIME           0x000E    /** Hold time settings */
#define NF_CONF_READ_PULSE_TIME     0x00F0    /** Duration of read pulse */
#define NF_CONF_WRITE_PULSE_TIME    0x0F00    /** Duration of write pulse */
#define NF_CONF_WRITE_H_TO_READ_L   0xF000    /** Twhr time duration */

/******************************************************************************/

/******************************************************************************/
/**  The flctrl (0x04) register masks. */
/******************************************************************************/

#define NF_CTRL_WRITE_PROT          	0x00800000    /** Write Protect bits */

#define NF_CTRL_CHIP_EN             		0x00010000    /** Chip Enable bit */
#define NF_CTRL_CHIP_MASK           	0x007f0000    /** Chip Enable bit */
#define NF_CTRL_IO_WIDTH32            	0x00004000     /** IO  Witdth 32*/
#define NF_CTRL_IO_WIDTH            	0x00002000     /** IO Witdth */
#define NF_CTRL_BLOCK_SIZE          	0x00001000    /** Block Size */
#define NF_CTRL_PAGE_SIZE           	0x00000800    /** Page Size */
#define NF_CTRL_SP_SIZE             		0x00000400    /** SP Size */

#define NF_CTRL_ECC_EN              		0x00200    /** Enable hardware ECC */
#define NF_CTRL_INT_EN              		0x00100    /** Global interrupt enable */
#define NF_CTRL_ACC_ERR_EN          	0x00080    /** Incorrect fldata transfer interrupt enable. */

/** Interrupt enable for attempt Erase/Write protected area. 
 * (Bit presence depends on the controller configuration) 
 */
#define NF_CTRL_PROT_IE             0x00040


#define NF_CTRL_RNB_IE              0x00020    /** Interrupt enable on rising edge on ready/busy port detection. */

/** Interrupt enable for DMA interrupt request. 
 * (Bit presence depends on the controller configuration)
 */ 
#define NF_CTRL_DMA_IE              0x00010

/** The DMA triggering after completing automatic data transfer enable. 
 * (Bit presence depends on the controller configuration)
 */ 
#define NF_CTRL_DMA_TRIGGER         0x00008 
#define NF_CTRL_TRANS_COMPLETE      0x00004

#define NF_CTRL_COMMON_RNB_LINE     0x00002    /** Common RnB line for all devices */
#define NF_CTRL_ADDR_CYCLE          0x00001    /** Number of address bytes sent to NAND Flash device */
/** @} */
/******************************************************************************/

/******************************************************************************/
/** @name The flstate register masks. */
/******************************************************************************/
/** @{ */


#define NF_STATE_RNB_ST             0x0001ul    /** State of NFRnB (Nand Flash Read not Busy) pin */
#define NF_STATE_RNB_EDGE           0x0002ul    /** This bit is set when rising edge hes been detected. */
#define NF_STATE_ACC_ERR            0x0004ul    /** This bit is set when has been attempt to make illegal access. */
#define NF_STATE_PROT_ERR           0x0008ul    /** This bit is set when CPU wanted Write or Erase protected area */
#define NF_STATE_HINT               0x0010ul    /** Interrupt request */

/** FSM Busy. 
  *	If at least one of these two flag(0x20 and 0x40)
  * is set it means that FSM executes instruction 
  *	and can�t accept new one 
  */
#define NF_STATE_FSM_BUSY           0x0060ul

#define NF_STATE_DMA_BUSY           0x0080ul    /** DMA Busy. This flag is set when DMA transfers data. */
#define NF_STATE_DMA_ERR_INT            0x0100ul    /** DMA Interrupt flag. Flag is set when DMA set interrupt request */
#define NF_STATE_TRANS_INT          0x0200ul    /** Flag is set when transfer with using internall buffer is completed. */

#define NF_DMACTRL_DMA_BUSY         0x4ul       /** Flag is at DMA Control SFR **/
/** @} */
/******************************************************************************/

/******************************************************************************/
/** @name The flstate register masks. */
/******************************************************************************/

/******************************************************************************/
/** @name The fldmactrl (0x84) register masks. */
/******************************************************************************/
/** @{ */
/** DMA write error */
#define NF_DMA_AHB_ERROR            0x00000001
/** DMA read error */
#define NF_DMA_ERR_FLAG             0x00000002
/** DMA busy */
#define NF_DMA_BUSY                 0x00000004


#define NF_DMA_BUST_MASK				(0x7 << 11)

/** single transfer address decrement */
#define NF_DMA_SINGLE_ADRR_DEC      0x00000000
/** burst of unspecified length address decrement */
#define NF_DMA_BURST_ADRR_DEC       0x00000800
/** single transfer address increment */
#define NF_DMA_SINGLE_ADRR_INC      0x00001000
/** burst of unspecified length address increment */
#define NF_DMA_BURST_ADRR_INC       0x00001800
/** 4 beat burst address increment */
#define NF_DMA_BURST_4_ADDR_INC     0x00002000
/** 8 beat burst address increment */
#define NF_DMA_BURST_8_ADDR_INC     0x00002800
/** 16 beat burst address increment */
#define NF_DMA_BURST_16_ADDR_INC    0x00003000
/** stream burst (address const) */
#define NF_DMA_STREAM_BURST         0x00003800

#define NF_DMA_SIZE_MASK				(0x3 << 11)

/** AHB 8 bit transfer size */
#define NF_DMA_SIZE_8               0x00000000
/** AHB 16 bit transfer size */
#define NF_DMA_SIZE_16              0x00004000
/** AHB 32 bit transfer size */
#define NF_DMA_SIZE_32              0x0000C000
/** DMA direction - write data from AHB to internal data buffer */
#define NF_DMA_WRITE                0x00000000
/** DMA direction - read data from internal data buffer to AHB */
#define NF_DMA_READ                 0x00010000
/** start DMA transfer */
#define NF_DMA_START_FLAG           0x01000000
/** @} */
/******************************************************************************/

/******************************************************************************/
/** @name Data transfer direction parameters */
/******************************************************************************/
/** @{ */
#define NF_TRANSFER_READ            0x01    /** transfer data direction - read */
#define NF_TRANSFER_WRITE           0x00    /** transfer data direction - write */
/** @} */
/******************************************************************************/

/******************************************************************************/
#define NF_UNPROTECT_WRITE		0x0		/** could write */
#define NF_PROTECT_WRITE		0x1		/** couldn't write */
/******************************************************************************/


/******************************************************************************/
/** 
 * @name Genric memory commands.
 * These commands are supported by all of the three NANDFlash memories 
 * (Samsung, Micron and ST)
 */
/******************************************************************************/
/** @{ */
#define NFG_CMD_PAGE_READ           0x0030    /** Reads automatically full Page and transfers  it into the BUFFER */
#define NFG_CMD_PAGE_READ_1         0x0130    /** Sends instructions and address to the Flash memory device */
#define NFG_CMD_READ_INTER_DM       0x0035    /** Sends instructions and address (4 or 5 bytes) to Flash memory */
#define NFG_CMD_RANDOM_DATA_READ    0x0005    /** It sends instructions and 2 address bytes to Flash memory */
#define NFG_CMD_READ_STATUS         0x0070    /** Sends instructions to Flash memory */
#define NFG_CMD_PROGRAM_PAGE        0x0080    /** Automatically writes contents of the BUFFER to the Flash memory */
#define NFG_CMD_PROGRAM_PAGE_1      0x0180    /** Sends instruction and address to the Flash memory. */
#define NFG_CMD_PROGRAM_PAGE_CACHE  0x0580    /** Automatically writes contents of the BUFFER to the Flash memory */
#define NFG_CMD_WRITE_PAGE          0x0010    /** Sends instruction to the Flash memory */
#define NFG_CMD_WRITE_CACHE         0x0015    /** It sends only instruction to Flash memory */
#define NFG_CMD_PROGRAM_INTER_DM    0x0085    /** It writes instruction 0x85h, next address (4 or 5 bytes) and instruction 0x10h */
#define NFG_CMD_RANDOM_DI_PROG      0x0185    /** It writes instruction and 2 address bytes */
#define NFG_CMD_RANDOM_DI_PROG_1    0x0585    /** It writes instruction and 5 address bytes */
#define NFG_CMD_BLOCK_ERASE         0x0060    /** It writes instructions address to Flash memory */
#define NFG_CMD_RESET               0x00FF    /** It writes instruction to Flash memory */
#define NFG_CMD_READ_ID             0x0090    /** It writes instruction and address */
#define NFG_CMD_READ_PAGE          0x00E0
#define NFST_COM_READ_BL_LOCK_STAT   0x207A  /** read block lock status of ST memory */
#define NFST_COM_BL_UNLOCK           0x2023 /** unlock blocks of ST memory */
#define NFST_COM_BL_LOCK             0x202A /** lock blocks of ST memory */
#define NFST_COM_BL_LOCK_DOWN        0x202C /** lock-down blocks of ST memory */
/** @} */
/******************************************************************************/
#define BIT0    0x1ul
#define BIT1    0x2ul
#define BIT2    0x4ul
#define BIT3    0x8ul
#define BIT4    0x10ul
#define BIT5    0x20ul
#define BIT6    0x40ul
#define BIT7    0x80ul
#define BIT8    0x100ul
#define BIT9    0x200ul
#define BIT10   0x400ul
#define BIT11   0x800ul
#define BIT12   0x1000ul
#define BIT13   0x2000ul
#define BIT14   0x4000ul
#define BIT15   0x8000ul
#define BIT16   0x10000ul
#define BIT17   0x20000ul
#define BIT18   0x40000ul
#define BIT19   0x80000ul
#define BIT20   0x100000ul
#define BIT21   0x200000ul
#define BIT22   0x400000ul
#define BIT23   0x800000ul
#define BIT24   0x1000000ul
#define BIT25   0x2000000ul
#define BIT26   0x4000000ul
#define BIT27   0x8000000ul
#define BIT28   0x10000000ul
#define BIT29   0x20000000ul
#define BIT30   0x40000000ul
#define BIT31   0x80000000ul
#endif//_SOCLE_NAND_REGS_H

