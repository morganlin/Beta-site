#ifndef __SOCLE_SDHC_H_INCLUDED
#define __SOCLE_SDHC_H_INCLUDED

#include "type.h"

//*****************************************************************************************************
#define TO_THRESHOLD_CNT 			1024*1024
#define PAGE_SIZE					512
#define TEST_PATTERN_BUF_SIZE 		2048 //Max = 32768 
#define TEST_MULTI_BLOCK_NUM 		TEST_PATTERN_BUF_SIZE/PAGE_SIZE
#define TEST_SECT_NUM 				TEST_PATTERN_BUF_SIZE/PAGE_SIZE //4
#define TEST_MEM_PATTERN_ADDR 	0x00a00000
#define TEST_MEM_CMPR_ADDR 		(TEST_MEM_PATTERN_ADDR + (TEST_SECT_NUM * PAGE_SIZE))

#define TEST_PATTERN_TX_ADDR 		(SQ_MEMORY_ADDR_START + 0x00a00000)
#define TEST_PATTERN_RX_ADDR 		(SQ_MEMORY_ADDR_START + 0x00a10000)
#define SD_STATUS_ADDR 			(SQ_MEMORY_ADDR_START + 0x00a20000)

//*****************************************************************************************************


/*
 *  Interrupt Bit
 *  */
enum int_bit {
	CMD_RESP_INT = (1 << 2),
	DATA_TX_INT = (1 << 1),
	CARD_DET_INT = 1
};

/* Test pattern's width */
enum {
	PATTERN_WIDTH_BYTE = 0,
	PATTERN_WIDTH_HALFWORD,
	PATTERN_WIDTH_WORD
};

/*
 *  The internal representation of our device.
 *  */
struct socle_sdmmc_device {
	int size;		/* device size in sectors */
	u32 card_rca;		/* card's relative card address */
	u32 card_res_0;		/* card's response arguments -0 */
	u32 card_res_1;		/* card's response arguments -1 */
	u32 card_res_2;		/* card's response arguments -2 */
	u32 card_res_3;		/* card's response arguments -3 */
	u32 card_state;		/* card's state machine */
	u8 card_type;		/* card's type */
	volatile u8 card_int;	/* card's interrupt status */
	u8 alive_flag;		/* card's alive flag */
	u32 mmu_setting;	/* host controller's mmu setting */
	int bus_width;		/* card's transfer bus width */
	u32 pattern_width;	/* test pattern width */
	u8 media_changed;	/* flag a media change? */
};

/* err_flag Error indicator */
#define SDMMC_CMD_RESP_ERR 1
#define SDMMC_DATA_TX_ERR (1 << 1)

#define SDMA_MODE       0x00
#define ADMA1_MODE      0x01
#define ADMA2_MODE      0x02
#define NONEDMA_MODE    0xFF

#define MMC_SLOTS 1
/*
 *  Low level type for this driver
 *  */

struct socle_sdhc_host {
	struct mmc_host *mmc;
	struct mmc_command *cmd;
	struct mmc_request *request;

//	struct resource *io_area;
	int irq;
	u8 wp;		//write protect 
	u8 slot;
	/* Flag indicating when the command has been sent.
	 * This is used to work out whether or not to send the stop*/
	unsigned int flags;
	u8 AutoCMD12Enable;
	u8	dma_mode;
	u8 err_flag;
	u8	tc; 		//transfer complete
	u8 opmode;			// manual opmode
	u8 alive_flag;		/* card's alive flag */
	u32 bus_mode;		/* Flag for current bus setting */
	u32 *dma_buf;		/* DMA buffer used for transmitting */
	u32	hcs; 			/* host capacity support */
	u8 hs_mode;
	u8 mmc_bus_width;
	u32 *buf;
	u32 buf_idx;
	u32 cur_sg_idx;
	u32 blk_idx;
};


struct socle_sdhci_ops {
	u8 (*sdhci_host_initialize)(struct socle_sdhc_host *host);
	void (* sdhci_host_isr)(void *data);
	u8 (*sdhci_card_detect)(struct socle_sdhc_host *host);	
	u8 (*sdhci_send_command)(struct socle_sdhc_host *host, struct mmc_command *cmd);
//	u32 (*sdhci_send_command)(struct socle_sdmmc_device *dev, u16 cmd_abbr, u32 arg);
	u32 (*sdhci_get_response)(struct socle_sdmmc_device *dev, u32 *resp);
	u32 (*sdhci_host_release)(void);
	void (*sdhci_host_set_bus_width) (struct socle_sdhc_host *host);
	void (*sdhci_host_set_hs) (struct socle_sdhc_host *host, u8 speed);
	u8 (* sdhci_host_get_write_protect) (struct socle_sdhc_host *host);
	u8 (*sdhci_host_set_clk) (struct socle_sdhc_host *host, u32 clk);
};

#endif
