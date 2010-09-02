#include <test_item.h>
#include <genlib.h>
#include <dma/dma.h>
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif
#include "dependency.h"
#include "sd.h"
#include "mmc.h"
#include "card.h"
#include "sdio.h"
#include "sdhc.h"
#include "host.h"

//#define SDHC_DEBUG

#ifdef SDHC_DEBUG
#define PDEBUG(fmt, args...) printf(fmt, ## args)
#else
#define PDEBUG(fmt, args...)
#endif

static u8 socle_sdhci_card_ident(struct socle_sdhc_host *host,u8 only_ident);
static u8 socle_sdhc_compare_pattern(u32 data_a, u32 data_b, u32 size);
static void mmc_decode_cid(struct mmc_card *card);
static void mmc_decode_csd(struct mmc_card *card);
static int mmc_decode_scr(struct mmc_card *card);
static void socle_sdhc_prepare_pattern(u8 *buf, u32 size);

//****************************test function*******************************************************



//*****************************************************************************************************
static volatile u32 socle_sdmmc_sw_dma_complete_flag = 0;
static u32 socle_sdhci_irq;
u32 socle_sdhc_base;

u32 socle_sdmmc_to_cnt = 0;


static struct socle_sdhci_ops *socle_sdhci;

struct socle_sdhc_host socle_host;
struct mmc_host socle_mmc;
struct mmc_data send_date;
struct mmc_command cmd;
struct mmc_card socle_mmc_card;

//*****************************************************************************************************
//support SOCLE SDMMC
extern struct socle_sdhci_ops socle_sdmmc_host_ops;
//support SOCLE SDHC 2.0
extern struct socle_sdhci_ops socle_sdhc_host_ops;

//*****************************************************************************************************

extern struct test_item_container socle_sdhc_host_test_container;
extern struct test_item_container socle_sdmmc_sd_main_test_container;
extern struct test_item_container socle_sdmmc_mmc_main_test_container;

//*****************************************************************************************************
/// Macro gets function busy status from the switch funnction status data structure
/// Macro returns 1 if function is busy 0 otherwise
#define SDCARD_SWICH_FUNC_GET_BUSY_STAT(VAL, GROUP_NUM, FUNC_NUM)\
            ( ( ( ( (WORD) VAL[29 - (GROUP_NUM - 1) * 2 ] ) \
               | ( (WORD)( VAL[30 - (GROUP_NUM - 1) * 2 ] ) << 8 ) )\
               &  ( 1ul << FUNC_NUM ) ) != 0 )\

/// Macro gets function status code from the switch funnction status data structure
#define SDCARD_SWICH_FUNC_GET_STAT_CODE(VAL, GROUP_NUM)\
        ( ( VAL[16 - (GROUP_NUM - 1) / 2 ] >> ( ( ( GROUP_NUM - 1 ) % 2 ) * 4 ) ) & 0xF )

/// Macro returns 1 if given function is supported by the card
#define SDCARD_SWICH_FUNC_IS_FUNC_SUPPORTED(VAL, GROUP_NUM, FUNC_NUM)\
            ( ( ( ( (WORD) VAL[13 - (GROUP_NUM - 1) * 2 ] ) \
               | ( (WORD)( VAL[12 - (GROUP_NUM - 1) * 2 ] ) << 8 ) )\
               &  ( 1ul << FUNC_NUM ) ) != 0 )\
        
/// Macro gets version number of data structure
#define SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(VAL) (VAL[17]) 

/************************************************MISC Command *************************************/
extern int 
socle_sdhc_misc_command_9(int autotest)
{
	int err, i = 0;
	struct mmc_host *mmc = &socle_mmc;
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;

	if (0 == socle_host.alive_flag) {
		printf("\nSocle SDHC: card is not exist\n");
		return -1;
	}

	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}

	if (MMC_TYPE_SD != host->mmc->card->type) {
		printf("\nSocle SD/MMC host: this card is not a SD card\n");
		return -1;
	}


	PDEBUG("CMD : MMC_SEND_CSD \n");
	send_cmd->opcode = MMC_SEND_CSD;
	send_cmd->arg = mmc->card->rca << 16;
	send_cmd->flags = MMC_RSP_R2;
	err = socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	if (err)
		goto out;

	for(i=0;i<4;i++)
		mmc->card->raw_csd[i] = send_cmd->resp[i];
	
	mmc_decode_csd(host->mmc->card);

out:

	return err;
}

extern int 
socle_sdhc_misc_command_10(int autotest)
{
	int err, i = 0;
	struct mmc_host *mmc = &socle_mmc;
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;

	if (0 == socle_host.alive_flag) {
		printf("\nSocle SDHC: card is not exist\n");
		return -1;
	}

	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}

	if (MMC_TYPE_SD != host->mmc->card->type) {
		printf("\nSocle SD/MMC host: this card is not a SD card\n");
		return -1;
	}

	PDEBUG("CMD : MMC_SEND_CID \n");
	send_cmd->opcode = MMC_SEND_CID;
	send_cmd->arg = mmc->card->rca << 16;
	send_cmd->flags = MMC_RSP_R2;
	err = socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	if(err)
		goto out;

	for(i=0;i<4;i++)
		mmc->card->raw_cid[i] = send_cmd->resp[i];
	
	mmc_decode_cid(host->mmc->card);
out:

	return err;
}

extern int socle_sdhc_misc_command_13(int autotest)
{
	int err;
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;

	if (0 == socle_host.alive_flag) {
		printf("\nSocle SDHC: card is not exist\n");
		return -1;
	}

	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}

	if (MMC_TYPE_SD != host->mmc->card->type) {
		printf("\nSocle SD/MMC host: this card is not a SD card\n");
		return -1;
	}

	PDEBUG("CMD : MMC_SEND_STATUS\n");
	send_cmd->opcode = MMC_SEND_STATUS;
	send_cmd->arg = host->mmc->card->rca << 16;
	send_cmd->flags = MMC_RSP_R1;

	err = socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	if (err)
		goto out;

	PDEBUG("card current state : %x \n",R1_CURRENT_STATE(send_cmd->resp[0]));
out:

	return err;     
}

extern int socle_sdhc_misc_command_15(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;

	if (0 == socle_host.alive_flag) {
		printf("\nSocle SDHC: card is not exist\n");
		return -1;
	}

	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}

	if (MMC_TYPE_SD != host->mmc->card->type) {
		printf("\nSocle SD/MMC host: this card is not a SD card\n");
		return -1;
	}

	PDEBUG("CMD : MMC_GO_INACTIVE_STATE\n");
	send_cmd->opcode = MMC_GO_INACTIVE_STATE;
	send_cmd->arg = host->mmc->card->rca << 16;
	send_cmd->flags = MMC_RSP_NONE;

	if(socle_sdhci->sdhci_send_command(&socle_host, &cmd))
		return -1;

	printf("\nSocle SD/MMC host: card has gone to inactive state, please re-insert it again\n");
	return 0;     
}

extern int socle_sdhc_misc_app_command_51(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;

	if (0 == socle_host.alive_flag) {
		printf("\nSocle SDHC: card is not exist\n");
		return -1;
	}

	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}

	if (MMC_TYPE_SD != host->mmc->card->type) {
		printf("\nSocle SD/MMC host: this card is not a SD card\n");
		return -1;
	}

	PDEBUG("CMD : ACMD51\n");
	send_cmd->opcode = MMC_APP_CMD;
	send_cmd->arg = host->mmc->card->rca << 16;
	send_cmd->flags = MMC_RSP_R1;

	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

	send_cmd->opcode = SD_APP_SEND_SCR;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_NONE;

	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

//	for(i=0;i<4;i++)
//		printf("resp = %x, %x, %x, %x \n", send_cmd->resp[i]);

	mmc_decode_scr(host->mmc->card);


	return 0;
}

extern int 
socle_sdhc_manual_command(int autotest)
{
	printf("socle_sdhc_manual_command \n");
	printf("0.No\n");
	printf("1.Yes\n");

	
	return 0;
}

extern struct test_item_container socle_sdhc_misc_command_test_container;

extern int
socle_sdhc_misc_command_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&socle_sdhc_misc_command_test_container, autotest);
	return ret;
}

/************************************************ test Menu ****************************************/
extern struct test_item_container socle_sdmmc_sd_xfer_test_container;

extern int 
socle_sdhc_without_auto_cmd12_test(int autotest)
{
	int ret = 0;
 	socle_host.AutoCMD12Enable = 0;
	ret = test_item_ctrl(&socle_sdmmc_sd_xfer_test_container, autotest);
	return ret;
}

extern int 
socle_sdhc_with_auto_cmd12_test(int autotest)
{
	int ret = 0;
	
	socle_host.AutoCMD12Enable = 1;
	ret = test_item_ctrl(&socle_sdmmc_sd_xfer_test_container, autotest);
	return ret;	
}


/************************************************ test Menu ****************************************/
extern struct test_item_container socle_sdhc_auto_cmd12_test_container;

extern int 
socle_sdhc_without_dma_test(int autotest)
{
	int ret = 0;
	
	socle_host.dma_mode = NONEDMA_MODE;
	ret = test_item_ctrl(&socle_sdhc_auto_cmd12_test_container, autotest);
	return ret;
}

extern int 
socle_sdhc_sdma_transfer_test(int autotest)
{
	int ret = 0;
	
	socle_host.dma_mode = SDMA_MODE;
	ret = test_item_ctrl(&socle_sdhc_auto_cmd12_test_container, autotest);
	return ret;	
}

extern int 
socle_sdhc_adma1_transfer_test(int autotest)
{
	int ret = 0;
	
	socle_host.dma_mode = ADMA1_MODE;
	ret = test_item_ctrl(&socle_sdhc_auto_cmd12_test_container, autotest);
	return ret;	
}

extern int 
socle_sdhc_adma2_transfer_test(int autotest)
{
	int ret = 0;
	
	socle_host.dma_mode = ADMA2_MODE;
	ret = test_item_ctrl(&socle_sdhc_auto_cmd12_test_container, autotest);
	return ret;	
}

/************************************************SD test Menu ****************************************/
extern struct test_item_container socle_sdhc_speed_main_test_container;

extern int
socle_sdhc_bus_1_transfer_test(int autotest)
{
	int ret = 0;

	socle_host.bus_mode = MMC_BUS_WIDTH_1;
	socle_host.mmc_bus_width = EXT_CSD_BUS_WIDTH_1;
	ret = test_item_ctrl(&socle_sdhc_speed_main_test_container, autotest);
	return ret;
}

extern int
socle_sdhc_bus_4_transfer_test(int autotest)
{
	int ret = 0;

	socle_host.bus_mode = MMC_BUS_WIDTH_4;
	socle_host.mmc_bus_width = EXT_CSD_BUS_WIDTH_4;
	ret = test_item_ctrl(&socle_sdhc_speed_main_test_container, autotest);
	return ret;
}

/************************************************speed_test Menu ****************************************/

extern struct test_item_container socle_sdhc_dma_mode_test_container;
extern int 
socle_sdhc_hs_transfer_test(int autotest)
{
	int ret = 0;
	struct socle_sdhc_host *host = &socle_host;
	
	if (MMC_TYPE_SD == host->mmc->card->type) {
		if(!socle_host.hcs) {
			printf("It's not HC card");
			return 0;
		}
	}
	socle_host.hs_mode = 1;
	
	ret = test_item_ctrl(&socle_sdhc_dma_mode_test_container, autotest);
	return ret;
}

extern int
socle_sdhc_normal_transfer_test(int autotest)
{
	int ret = 0;

	socle_host.hs_mode = 0;
	ret = test_item_ctrl(&socle_sdhc_dma_mode_test_container, autotest);
	return ret;
}

/**********************SD test function *******************************************/
extern int 
socle_sdhc_single_block_transfer_test(int autotest)
{
	u32 cur_addr_w = 0, cur_addr_r = 0;
	u32 block_num_w = 0, block_num_r = 0;
	u32 nsect_w = TEST_SECT_NUM, nsect_r = TEST_SECT_NUM;
	u8 mode, group, value;
	u8 StatusBuffer[64];

	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;
	send_cmd->data = NULL;
	struct mmc_data *data = &send_date;
	
	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, (TEST_SECT_NUM * 512) );
	memset((char *) TEST_PATTERN_RX_ADDR, 0, TEST_PATTERN_BUF_SIZE);

	if (socle_sdhci->sdhci_host_get_write_protect(host)) {
		printf("get write protect return \n");
		return -1;
	}

	if (0 == host->alive_flag) {
		printf(" SDHC : card is not exist\n");
		return -1;
	}

	if (socle_sdhci_card_ident(host, 0)) {
		printf("sd/mmc host: tx mode is fail\n");
		return -1;
	}

	if (MMC_TYPE_SD == host->mmc->card->type) {
		/* Send CMD55 to indicate that the next command is an application specific command */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = host->mmc->card->rca << 16;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		/* Send ACMD6 to select given bus width */
		PDEBUG("CMD : ACMD6 \n");
		send_cmd->opcode = SD_APP_SET_BUS_WIDTH;
		send_cmd->arg = host->bus_mode;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		/* Send CMD6 to select given bus width */
		PDEBUG("CMD : CMD6 \n");
		send_cmd->opcode = MMC_SWITCH;
		send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (host->mmc_bus_width << 8) |EXT_CSD_CMD_SET_NORMAL;
		send_cmd->flags = MMC_RSP_R1B;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
	} else {
		printf("ERROR 1!!!!\n");
	}


	socle_sdhci->sdhci_host_set_bus_width(host);
	
#if 1
	if(host->hs_mode) {

		if (MMC_TYPE_SD == host->mmc->card->type) {		
			//get mode
			mode = 0; //check function
			group = 0;
			value = 1; //high speed

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  (u32)StatusBuffer;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;



			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);


			if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) == 0xF ) {
				printf("ERROR : GET ERROR \n");
				return -1;
			}

			send_cmd->data = NULL;

			//set mode
			mode = 1; //set function
			group = 0;
			host->hs_mode = 1;
			if (host->hs_mode)	
				value = 1; //high speed
			else
				value = 0;

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  SD_STATUS_ADDR;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;

			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);
		} else if (MMC_TYPE_MMC == host->mmc->card->type) {
			/* Send CMD6 to select given bus width */
			PDEBUG("MMC : CMD6 \n");
			send_cmd->opcode = MMC_SWITCH;
			send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_HS_TIMING << 16) | (host->hs_mode << 8) |EXT_CSD_CMD_SET_NORMAL;
			send_cmd->flags = MMC_RSP_R1B;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		
		} else {
		
			printf("ERROR SDIO !!!!\n");
		}

		if (MMC_TYPE_SD == host->mmc->card->type) {
		        if (host->hs_mode){
				// Status indicates the same function number as specified in the argument, 
				// which means Supported function successful function change.
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 1 ) {
					printf("switch HS Fail \n");
					return -1;
				}
		        }
		        else{
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 0 ) {
					printf("switch ERROR \n");
					return -1;	
				}
		        }
		}
		
		send_cmd->data = NULL;
		
		socle_sdhci->sdhci_host_set_hs(host, 1);
		
	}
#endif

	while (nsect_w > 0) {

		//prepare send data 
		data->blksz = 512;
		data->blocks = 1;
		data->sg->dma_address =  TEST_PATTERN_TX_ADDR + block_num_w*512;
		data->sg->length = 512;
		data->flags = MMC_DATA_WRITE;
		send_cmd->data = data;
		
		/* Send CMD24 to write single block */
		PDEBUG("CMD : MMC_WRITE_BLOCK \n");
		send_cmd->opcode = MMC_WRITE_BLOCK;
		send_cmd->arg = cur_addr_w; 			//write address from 0
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd)) {
			printf("CMD : ERROR \n");
			return -1;
		}
		host->mmc->card->state = RCV;

		host->mmc->card->state = PRG;
		
		
		/* start data transmission */
		host->mmc->card->state = TRAN;

		if(host->hcs)  // for high capacity
			cur_addr_w++;
		else
			cur_addr_w += 512;
		
		block_num_w++;
		nsect_w--;

	}

	send_cmd->data = NULL;
	
	while (nsect_r > 0) {
		//prepare send data 
		data->blksz = 512;
		data->blocks = 1;
		data->sg->dma_address =  TEST_PATTERN_RX_ADDR + block_num_r*512;
		data->sg->length = 512;
		data->flags = MMC_DATA_READ;
		send_cmd->data = data;

//		host->tc = 1;	
		/* Send CMD17 to read single block */
		PDEBUG("CMD : MMC_READ_SINGLE_BLOCK \n");
		send_cmd->opcode = MMC_READ_SINGLE_BLOCK;
		send_cmd->arg = cur_addr_r; 			//write address from 0
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd)) {
			printf("CMD : ERROR \n");
			return -1;
		}
		
		host->mmc->card->state = DATA;

		/* Wait for the reading transmission to be complete */
		/* start data transmission */
		host->mmc->card->state = TRAN;

		if(host->hcs)  // for high capacity
			cur_addr_r++;
		else
			cur_addr_r += 512;

		block_num_r++;
		nsect_r--;
	}

	send_cmd->data = NULL;

	if(socle_sdhc_compare_pattern(TEST_PATTERN_TX_ADDR, TEST_PATTERN_RX_ADDR, 512*TEST_SECT_NUM))
		return -1;
	
	return 0;
}

extern int 
socle_sdhc_multiple_block_transfer_test(int autotest)
{

	u32 cur_addr_w = 0, cur_addr_r = 0;

	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;
	send_cmd->data = NULL;
	struct mmc_data *data = &send_date;
	u8 mode, group, value;
	u8 StatusBuffer[64];

	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, TEST_MULTI_BLOCK_NUM * 512);
	memset((char *) TEST_PATTERN_RX_ADDR, 0, TEST_PATTERN_BUF_SIZE);

	if (socle_sdhci->sdhci_host_get_write_protect(host)) {
		printf("get write protect return \n");
		return -1;
	}

	if (0 == host->alive_flag) {
		printf(" SDHC : card is not exist\n");
		return -1;
	}

	if (socle_sdhci_card_ident(host, 0)) {
		printf("sd/mmc host: tx mode is fail\n");
		return -1;
	}

	if (MMC_TYPE_SD == host->mmc->card->type) {
		/* Send CMD55 to indicate that the next command is an application specific command */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = host->mmc->card->rca << 16;
		send_cmd->flags = MMC_RSP_R1;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		/* Send ACMD6 to select given bus width */
		PDEBUG("CMD : ACMD6 \n");
		send_cmd->opcode = SD_APP_SET_BUS_WIDTH;
		send_cmd->arg = host->bus_mode;
		send_cmd->flags = MMC_RSP_R1;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		/* Send CMD6 to select given bus width */
		PDEBUG("CMD : CMD6 \n");
		send_cmd->opcode = MMC_SWITCH;
		send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (host->mmc_bus_width << 8) |EXT_CSD_CMD_SET_NORMAL;
		send_cmd->flags = MMC_RSP_R1B;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
	} else {
		printf("ERROR 1!!!!\n");
	}

	socle_sdhci->sdhci_host_set_bus_width(host);
	
#if 1
	if(host->hs_mode) {
		
		if (MMC_TYPE_SD == host->mmc->card->type) {				
			
			//get mode
			mode = 0; //check function
			group = 0;
			value = 1; //high speed

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  (u32)StatusBuffer;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;



			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);


			if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) == 0xF ) {
				printf("ERROR : GET ERROR \n");
				return -1;
			}

			send_cmd->data = NULL;

			//set mode
			mode = 1; //set function
			group = 0;
			host->hs_mode = 1;
			if (host->hs_mode)	
				value = 1; //high speed
			else
				value = 0;

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  SD_STATUS_ADDR;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;

			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);

		} else if (MMC_TYPE_MMC == host->mmc->card->type) {
			/* Send CMD6 to select given bus width */
			PDEBUG("MMC : CMD6 \n");
			send_cmd->opcode = MMC_SWITCH;
			send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_HS_TIMING << 16) | (host->hs_mode << 8) |EXT_CSD_CMD_SET_NORMAL;
			send_cmd->flags = MMC_RSP_R1B;
			send_cmd->data = NULL;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		
		} else {
		
			printf("ERROR SDIO !!!!\n");
		}

		if (MMC_TYPE_SD == host->mmc->card->type) {
		        if (host->hs_mode){
				// Status indicates the same function number as specified in the argument, 
				// which means Supported function successful function change.
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 1 ) {
					printf("switch HS Fail \n");
					return -1;
				}
		        }
		        else{
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 0 ) {
					printf("switch ERROR \n");
					return -1;	
				}
		        }
		}

		send_cmd->data = NULL;
		
		socle_sdhci->sdhci_host_set_hs(host, 1);
		
	}
#endif

	//prepare send write data 
	data->blksz = 512;
	data->blocks = TEST_MULTI_BLOCK_NUM;
	data->sg->dma_address =  TEST_PATTERN_TX_ADDR;
	data->sg->length = 512 * TEST_MULTI_BLOCK_NUM;
	data->flags = MMC_DATA_WRITE;
	send_cmd->data = data;
	
	/* Send CMD25 to write multiple block */
	PDEBUG("CMD : MMC_WRITE_MULTIPLE_BLOCK \n");
	send_cmd->opcode = MMC_WRITE_MULTIPLE_BLOCK;
	send_cmd->arg = cur_addr_w;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

	host->mmc->card->state = RCV;

	/* Send CMD12 to stop */
	if (host->AutoCMD12Enable == 0) {
		PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");
		send_cmd->opcode = MMC_STOP_TRANSMISSION;
		send_cmd->arg = 0;
		send_cmd->flags = MMC_RSP_R1B;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	}

	host->mmc->card->state = TRAN;

	//prepare send read data 
	data->blksz = 512;
	data->blocks = TEST_MULTI_BLOCK_NUM;
	data->sg->dma_address =  TEST_PATTERN_RX_ADDR;
	data->sg->length = 512 * TEST_MULTI_BLOCK_NUM;
	data->flags = MMC_DATA_READ;
	send_cmd->data = data;
	
	/* Send CMD18 to read multiple block */
	PDEBUG("CMD : MMC_READ_MULTIPLE_BLOCK \n");	
	send_cmd->opcode = MMC_READ_MULTIPLE_BLOCK;
	send_cmd->arg = cur_addr_r;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;
	
	host->mmc->card->state = DATA;

	
	
	if (host->AutoCMD12Enable == 0) {
		/* Send CMD12 to stop */
		PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");	
		send_cmd->opcode = MMC_STOP_TRANSMISSION;
		send_cmd->arg = 0;
		send_cmd->flags = MMC_RSP_R1B;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	}
	
	if(socle_sdhc_compare_pattern(TEST_PATTERN_TX_ADDR, TEST_PATTERN_RX_ADDR, 512*TEST_MULTI_BLOCK_NUM))
		return -1;

	return 0;
}

//
/**********************Pattern Prepare *******************************************/
static void 
socle_sdhc_prepare_pattern(u8 *buf, u32 size)
{
	int i;

	for (i = 0; i < size; i++) {
		if(i>512)
			break;
		buf[i] = i + 1;
	}

	for (i=512 ; i < size; i++) {
		if(i>1024)
			break;

		buf[i] = 0x5a;
	}

	for (i=1024 ; i < size; i++) {
		if(i>1536)
			break;

		buf[i] = 0xa5;
	}

	for (i=1536 ; i < size; i++) {
		buf[i] = i + 1;
	}
}

static u8 
socle_sdhc_compare_pattern(u32 data_a, u32 data_b, u32 size)
{
	u32 i =0;

	while(i*4<size) {
		if(*(u32*) (data_a + (i*4)) != *(u32*) (data_b + (i*4)))
		{
			printf("\nSocle SD/MMC host: cause a compare error at addr : %x \n", (i*4));
			return -1;
		}
		i++;
	}
	return 0;
}
/************************************************************************************************/


static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static void 
mmc_decode_cid(struct mmc_card *card)
{
	u32 *resp = card->raw_cid;

	memset((char *)&card->cid, 0, sizeof(struct mmc_cid));

	/*
	 * SD doesn't currently have a version field so we will
	 * have to assume we can parse this.
	 */
	card->cid.manfid				= UNSTUFF_BITS(resp, 120, 8);
	card->cid.oemid				= UNSTUFF_BITS(resp, 104, 16);
	card->cid.prod_name[0]		= UNSTUFF_BITS(resp, 96, 8);
	card->cid.prod_name[1]		= UNSTUFF_BITS(resp, 88, 8);
	card->cid.prod_name[2]		= UNSTUFF_BITS(resp, 80, 8);
	card->cid.prod_name[3]		= UNSTUFF_BITS(resp, 72, 8);
	card->cid.prod_name[4]		= UNSTUFF_BITS(resp, 64, 8);
	card->cid.hwrev				= UNSTUFF_BITS(resp, 60, 4);
	card->cid.fwrev				= UNSTUFF_BITS(resp, 56, 4);
	card->cid.serial				= UNSTUFF_BITS(resp, 24, 32);
	card->cid.year				= UNSTUFF_BITS(resp, 12, 8);
	card->cid.month				= UNSTUFF_BITS(resp, 8, 4);

	card->cid.year += 2000; /* SD cards year offset */

	printf("menufactor id : %x \n", card->cid.manfid);
	printf("product name : %s \n", card->cid.prod_name);
	printf("year : %d \n", card->cid.year);
	
}

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static void 
mmc_decode_csd(struct mmc_card *card)
{
	struct mmc_csd *csd = &card->csd;
	unsigned int e, m, csd_struct;
	u32 *resp = card->raw_csd;

	csd_struct = UNSTUFF_BITS(resp, 126, 2);

	switch (csd_struct) {
	case 0:
		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity	  = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
		break;
	case 1:
		/*
		 * This is a block-addressed SDHC card. Most
		 * interesting fields are unused and have fixed
		 * values. To avoid getting tripped by buggy cards,
		 * we assume those fixed values ourselves.
		 */
//		mmc_card_set_blockaddr(card);

		csd->tacc_ns	 = 0; /* Unused */
		csd->tacc_clks	 = 0; /* Unused */

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		m = UNSTUFF_BITS(resp, 48, 22);
		csd->capacity     = (1 + m) << 10;
		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
		break;
	default:
		printf("ERROR : unrecognised CSD structure version %d\n",csd_struct);
		break;
	}
#if 0
		printf("csd->cmdclass : %x \n",csd->cmdclass);
		printf("csd->cmdclass : %x \n",csd->read_blkbits);
		printf("csd->cmdclass : %x \n",csd->r2w_factor);
		printf("csd->cmdclass : %x \n",csd->write_blkbits);
#endif
		
}

/*
 * Given a 64-bit response, decode to our card SCR structure.
 */
static int mmc_decode_scr(struct mmc_card *card)
{
	struct sd_scr *scr = &card->scr;
	unsigned int scr_struct;
	u32 resp[4];

	resp[3] = card->raw_scr[1];
	resp[2] = card->raw_scr[0];

	scr_struct = UNSTUFF_BITS(resp, 60, 4);
	if (scr_struct != 0) {
		printf("ERROR : unrecognised SCR structure version %d\n",scr_struct);
		return 0;
	}

	scr->sda_vsn = UNSTUFF_BITS(resp, 56, 4);
	scr->bus_widths = UNSTUFF_BITS(resp, 48, 4);

	return 0;
}


static u8
socle_sdhci_card_ident(struct socle_sdhc_host *host,u8 only_ident)
{
	u32 rdata;
	static const u8 test_pattern = 0xAA;
	struct mmc_command *send_cmd = host->cmd;
	struct mmc_host *mmc = host->mmc;
	
	socle_sdhci->sdhci_host_set_clk(host, 400);
	
	//go idle 
	PDEBUG("CMD : CMD0 \n");
	send_cmd->opcode = MMC_GO_IDLE_STATE;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_NONE ;
	socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	
	mmc->card->card_state = IDLE;

	//Send CMD8 
	PDEBUG("CMD : CMD8 \n");
	send_cmd->opcode = SD_SEND_IF_COND;
	send_cmd->arg = ((mmc->ocr_avail & 0xFF8000) != 0) << 8 | test_pattern;
	send_cmd->flags = MMC_RSP_R7 ;
	
	socle_sdhci->sdhci_send_command(host, send_cmd);

	PDEBUG("Card with Compatible Voltage Range = %x \n",(send_cmd->resp[0] & (0xf<<8)) >> 8);
	
	if(host->err_flag == MMC_ERR_TIMEOUT) {
		PDEBUG("Inserted is non-SD2.0 Card \n");
		host->hcs = 0;
		goto SDIO;
	} else {
		host->hcs = 1;
		if((send_cmd->resp[0] & 0xff) != 0xAA) {
			printf("check pattern error : Unsupport \n;");
			return -1;
		}	
	}


SDIO:

	/*
	 * First we search for SDIO...
	 */

	//Send CMD5 
	PDEBUG("CMD : CMD5 \n");
	send_cmd->opcode = SD_IO_SEND_OP_COND;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_R4;

	socle_sdhci->sdhci_send_command(host, send_cmd);
	if(host->err_flag == MMC_ERR_TIMEOUT) {
		PDEBUG("Inserted is non-SDIO Card \n");
		goto SDMEM;
	}
	else {
		printf("SDIO Card identifiy \n");
		mmc->card->type = MMC_TYPE_SDIO;
		printf("Number of IO %d \n",send_cmd->resp[0]>>28);
	}

	/*
	 * Can we support the voltage(s) of the card(s)?
	 */

	//sdio select voltage
	send_cmd->opcode = SD_IO_SEND_OP_COND;
	send_cmd->arg = mmc->ocr_avail; //select voltage
	send_cmd->flags = MMC_RSP_R4;

	socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	if(host->err_flag == MMC_ERR_TIMEOUT) {
		printf("Inserted non-SDIO Card Timeout !!\n");
		return -1;
	} else {
//		goto CMD3;
	}
	

SDMEM:

/////////////////////////////////////////////////	
// cyli add
#if 0
printf("cyli fix\n");
                /* Send CMD55 */
                PDEBUG("CMD : CMD55 \n");
                send_cmd->opcode = MMC_APP_CMD;
                send_cmd->arg = 0x00000000;
                send_cmd->flags = MMC_RSP_R1;

                socle_sdhci->sdhci_send_command(&socle_host, &cmd);
//              if (send_cmd->error)
                if (host->err_flag)
                        goto mmc_identification;


                /* Send ACMD41, host support VDD voltage is 0x00ff8000 */
                PDEBUG("CMD : ACMD41 \n");
                send_cmd->opcode = SD_APP_OP_COND;
                send_cmd->arg = 0x0;
//		send_cmd->arg = 0x00ff8000;
                send_cmd->flags = MMC_RSP_R3;
                socle_sdhci->sdhci_send_command(&socle_host, &cmd);
#endif

#if 0
        //go idle
        PDEBUG("CMD : CMD0 \n");
        send_cmd->opcode = MMC_GO_IDLE_STATE;
        send_cmd->arg = 0;
        send_cmd->flags = MMC_RSP_NONE ;
        socle_sdhci->sdhci_send_command(&socle_host, &cmd);

        mmc->card->card_state = IDLE;

      //Send CMD8
        PDEBUG("CMD : CMD8 \n");
        send_cmd->opcode = SD_SEND_IF_COND;
        send_cmd->arg = ((mmc->ocr_avail & 0xFF8000) != 0) << 8 | test_pattern;
        send_cmd->flags = MMC_RSP_R7 ;

        socle_sdhci->sdhci_send_command(host, send_cmd);

        PDEBUG("Card with Compatible Voltage Range = %x \n",(send_cmd->resp[0] & (0xf<<8)) >> 8);

        if(host->err_flag == MMC_ERR_TIMEOUT) {
                PDEBUG("Inserted is non-SD2.0 Card \n");
                host->hcs = 0;
                goto SDIO;
        } else {
                host->hcs = 1;
                if((send_cmd->resp[0] & 0xff) != 0xAA) {
                        printf("check pattern error : Unsupport \n;");
                        return -1;
                }
        }
#endif
///////////////////////////////////////////////

	//card identification flow

	socle_sdmmc_to_cnt = 0;
	do {
		if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
			printf("\nSocle SD/MMC host: sd identification is timeout\n");
			return -1;
		}

		/* Send CMD55 */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = 0x00000000;
		send_cmd->flags = MMC_RSP_R1;
		
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);
//		if (send_cmd->error)
		if (host->err_flag)
			goto mmc_identification;


		/* Send ACMD41, host support VDD voltage is 0x00ff8000 */
		PDEBUG("CMD : ACMD41 \n");
		send_cmd->opcode = SD_APP_OP_COND;
		send_cmd->arg = 0x00ff8000 | (host->hcs << 30);
		send_cmd->flags = MMC_RSP_R3;
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);
//		printf("send_cmd->resp[0] = %x   \n",send_cmd->resp[0]); 
		socle_sdmmc_to_cnt++;

		if(host->err_flag == MMC_ERR_TIMEOUT) {
			printf("Timeout !!\n");
//			return -1;
		} else {
	//		goto CMD3;
		}

	}while ((send_cmd->resp[0] & 0x80000000) == 0);

	if(send_cmd->resp[0] & 0x40000000) {
		printf("High Capacity Card \n");
		host->hcs = 1;
	} else {
		printf("Standard Capacity Card \n");
		host->hcs = 0;
	}
	
	mmc->card->type |= MMC_TYPE_SD;
	PDEBUG("\n Socle SD/MMC host: SD card has been identified\n");
	goto identity_end;


mmc_identification:
	PDEBUG("CMD : MMC CMD1 \n");

	socle_sdmmc_to_cnt = 0;
	
	do {
		if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
			printf("\nSocle SD/MMC host: sd identification is timeout\n");
			return -1;
		}
	
		send_cmd->opcode = MMC_SEND_OP_COND;
		send_cmd->arg = 0x00ff8000;
		send_cmd->flags = MMC_RSP_R3;
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);
		rdata = send_cmd->resp[0];
//		printf("send_cmd->resp[0] = %x   \n",send_cmd->resp[0]); 
		socle_sdmmc_to_cnt++;

		if(host->err_flag == MMC_ERR_TIMEOUT) {
			goto CMD3;
		}
	} while (0x00000000 == (send_cmd->resp[0] & 0x80000000));
	mmc->card->type = MMC_TYPE_MMC;
	PDEBUG("\nSocle SD/MMC host: MMC has been identified\n");

identity_end:
	mmc->card->card_state = READY;
	/* Send CMD2 */
	PDEBUG("CMD :  CMD2 \n");
	
	send_cmd->opcode = MMC_ALL_SEND_CID;
	send_cmd->arg = 0x00000000;
	send_cmd->flags = MMC_RSP_R2;
	socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	
	mmc->card->card_state = IDENT;

CMD3:

	/* Send CMD3 */
	// 20090108 cyli fix
	PDEBUG("CMD :  CMD3 \n");
	if (MMC_TYPE_SD == mmc->card->type) {
		send_cmd->opcode = MMC_SET_RELATIVE_ADDR;
		send_cmd->arg = 0x00000000;
		send_cmd->flags = MMC_RSP_R6;
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);

		/* Get relative card address (RCA) */
//		printf("RCA send_cmd->resp[0] = %x   \n",send_cmd->resp[0]); 
	} else if (MMC_TYPE_MMC == mmc->card->type) {
		send_cmd->opcode = MMC_SET_RELATIVE_ADDR;
		send_cmd->arg = 0x00010000; /* Set relative card address (RCA) */
		send_cmd->flags = MMC_RSP_R6;
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);
	} else if (MMC_TYPE_SDIO == mmc->card->type) {		
		send_cmd->opcode = MMC_SET_RELATIVE_ADDR;
		send_cmd->arg = 0x00000000;
		send_cmd->flags = MMC_RSP_R6;
		socle_sdhci->sdhci_send_command(&socle_host, &cmd);
		/* Get relative card address (RCA) */
//		printf("RCA send_cmd->resp[0] = %x   \n",send_cmd->resp[0]); 
	} else {
		printf("\nSocle SDHC : unknown card type\n");
		return -1;
	}
	mmc->card->card_state = STDY;
	
	

	/* Get card's relative address (RCA) */
//	mmc->card->rca = send_cmd->resp[0];
	mmc->card->rca = send_cmd->resp[0] >> 16;
	PDEBUG("SDHC  RCA : %x \n",mmc->card->rca);

	if (only_ident)
		return 0;

	/* Send CMD7 */
	// 20080408 cyli fix for mmc cmd7 response
	PDEBUG("CMD : MMC_SELECT_CARD \n");
	send_cmd->opcode = MMC_SELECT_CARD;
	if (MMC_TYPE_SD == mmc->card->type) {	
		send_cmd->arg = mmc->card->rca << 16; 	/* Set relative card address (RCA) */
	} else if (MMC_TYPE_MMC == mmc->card->type) {
		send_cmd->arg = 0x00010000; 				/* Set relative card address (RCA) */
	} else if (MMC_TYPE_SDIO == mmc->card->type) {
		send_cmd->arg = mmc->card->rca << 16; 	/* Set relative card address (RCA) */
	} else {
		printf("unknow type \n");
		return -1;
	}
	
	send_cmd->flags = MMC_RSP_R1B;
	if(socle_sdhci->sdhci_send_command(&socle_host, &cmd)) {
		printf("CMD Error !! \n");
		return -1;
	}

	mmc->card->card_state = TRAN;

	/* Send CMD16 to set the block length */
	PDEBUG("CMD : MMC_SET_BLOCKLEN : 512 len \n");
	send_cmd->opcode = MMC_SET_BLOCKLEN;
	send_cmd->arg = 512;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(&socle_host, &cmd)) {
		printf("CMD Error !! \n");
		return -1;
	}

	if(host->hs_mode)
		socle_sdhci->sdhci_host_set_clk(host, 50000);
	else
		socle_sdhci->sdhci_host_set_clk(host, 25000);
		
//	mmc->card->media_changed = 0;
	return 0;

}

extern int
socle_sdhc_sd_erase(int autotest)
{
	u32 rdata;
	int i;

	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;
	send_cmd->data = NULL;
	struct mmc_data *data = &send_date;
	
	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, TEST_MULTI_BLOCK_NUM * 512);

	if (socle_sdhci->sdhci_host_get_write_protect(host)) {
		printf("get write protect return \n");
		return -1;
	}

	if (0 == host->alive_flag) {
		printf(" SDHC : card is not exist\n");
		return -1;
	}

	if (socle_sdhci_card_ident(host, 0)) {
		printf("sd/mmc host: tx mode is fail\n");
		return -1;
	}

	if (MMC_TYPE_SD == host->mmc->card->type) {
		/* Send CMD55 to indicate that the next command is an application specific command */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = host->mmc->card->rca << 16;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		/* Send ACMD6 to select given bus width */
		PDEBUG("CMD : ACMD6 \n");
		send_cmd->opcode = SD_APP_SET_BUS_WIDTH;
		send_cmd->arg = host->bus_mode;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	}else if (MMC_TYPE_MMC == host->mmc->card->type) {
		/* Send CMD6 to select given bus width */
		PDEBUG("CMD : CMD6 \n");
		send_cmd->opcode = MMC_SWITCH;
//printf("##--%s--mmc_bus_width = 0x%x\n", __func__, host->mmc_bus_width);
		send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (host->mmc_bus_width << 8) |EXT_CSD_CMD_SET_NORMAL;
		send_cmd->flags = MMC_RSP_R1B;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
	} else {
		printf("ERROR 1!!!!\n");
	}

	socle_sdhci->sdhci_host_set_bus_width(host);
	
	//prepare send write data 
	data->blksz = 512;
	data->blocks = TEST_MULTI_BLOCK_NUM;
	data->sg->dma_address =  TEST_PATTERN_TX_ADDR;
	data->sg->length = 512 * TEST_MULTI_BLOCK_NUM;
	data->flags = MMC_DATA_WRITE;
	send_cmd->data = data;
	

	/* Send CMD24 to write single block */
	PDEBUG("CMD : MMC_WRITE_BLOCK \n");
	send_cmd->opcode = MMC_WRITE_MULTIPLE_BLOCK;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

	send_cmd->data = NULL;

	if (host->AutoCMD12Enable == 0) {	
		/* Send CMD12 to stop */
		PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");	
		send_cmd->opcode = MMC_STOP_TRANSMISSION;
		send_cmd->arg = 0;
		send_cmd->flags = MMC_RSP_R1B;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	}
	
 	MSDELAY(100);

	/* Set the transfer setting and start a writing transmission */

	host->mmc->card->card_state  = TRAN;

	/* Send CMD32 to set the address of the first write block to be erased */
	PDEBUG("CMD : SD_ERASE_WR_BLK_START \n");
	if (MMC_TYPE_SD == host->mmc->card->type) {	
		send_cmd->opcode = SD_ERASE_WR_BLK_START;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		send_cmd->opcode = MMC_ERASE_GROUP_START;
	} else {
		printf("COMMAND ERROR \n");
		return -1;
	}
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;


	/* Send CMD 33 to set the address of the last write block of the continuous range to be erased */
	PDEBUG("CMD : SD_ERASE_WR_BLK_END \n");
	if (MMC_TYPE_SD == host->mmc->card->type) {		
		send_cmd->opcode = SD_ERASE_WR_BLK_END;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		send_cmd->opcode = MMC_ERASE_GROUP_END;
	} else {
		printf("COMMAND ERROR \n");
		return -1;
	}

	if(host->hcs == 1)
		send_cmd->arg = TEST_MULTI_BLOCK_NUM;
	else
		send_cmd->arg = TEST_PATTERN_BUF_SIZE;
	
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

	/* Send CMD38 to erase all previously selected write blocks */
	PDEBUG("CMD : MMC_ERASE \n");
	send_cmd->opcode = MMC_ERASE;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_R1B;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

// 	MSDELAY(100);
	
	host->mmc->card->card_state = TRAN;

	//prepare send data 
	data->blksz = 512;
	data->blocks = 4;
	data->sg->dma_address =  TEST_PATTERN_RX_ADDR;
	data->sg->length = 512;
	data->flags = MMC_DATA_READ;
	send_cmd->data = data;


	/* Send CMD17 to read a single block */
	PDEBUG("CMD : MMC_READ_MULTIPLE_BLOCK \n");
	send_cmd->opcode = MMC_READ_MULTIPLE_BLOCK;
	send_cmd->arg = 0;
	send_cmd->flags = MMC_RSP_R1;
	if(socle_sdhci->sdhci_send_command(host, send_cmd))
		return -1;

	send_cmd->data = NULL;
	if (host->AutoCMD12Enable == 0) { 
		/* Send CMD12 to stop */
		PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");	
		send_cmd->opcode = MMC_STOP_TRANSMISSION;
		send_cmd->arg = 0;
		send_cmd->flags = MMC_RSP_R1B;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	}
	
	host->mmc->card->card_state = TRAN;


	/* Compare data buffer has been erased to 0x00 or 0xFF  */
	//////////////////////////////////////////
	for (i = 0; i < 512/4 ; i++) {
		rdata = ioread32(TEST_PATTERN_RX_ADDR);
		if ((rdata != 0x00000000) && (rdata != 0xFFFFFFFF)) {
			printf("\n Socle SD/MMC host: data compare failed!\n");
			return -1;
		}
	}

	return 0;
}

extern int 
socle_sdhc_manual_single_block_test(int autotest)
{
	u32 cur_addr_w,cur_addr_r;
	u32 nsect_w, nsect_r;	
	u32 block_num_w = 0, block_num_r = 0;
	u8 mode, group, value;
	u8 StatusBuffer[64];
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;
	send_cmd->data = NULL;
	struct mmc_data *data = &send_date;
	
	if(socle_host.opmode == 0x1) {
		printf("please enter the read block number : ");
		scanf("%d\n",&cur_addr_r);
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_r);
		printf("read form block %d, count %d \n", cur_addr_r, nsect_r);
	}

	if(socle_host.opmode == 0x2) {
		printf("please enter the write block number : ");
		scanf("%d\n",&cur_addr_w);
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_w);
		printf("write form block %d, count %d \n", cur_addr_w,nsect_w);
	}

	if(socle_host.opmode == 0x10) {
		printf("please enter the loop block number : ");
		scanf("%d\n",&cur_addr_w);
		cur_addr_r = cur_addr_w;
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_w);
		nsect_r = nsect_w;
		printf("loop form block %d, count %d \n", cur_addr_w,nsect_w);
	}

	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, (nsect_w * 512) );
	memset((char *) TEST_PATTERN_RX_ADDR, 0, nsect_r*512);

	if (socle_sdhci->sdhci_host_get_write_protect(host)) {
		printf("get write protect return \n");
		return -1;
	}

	if (0 == host->alive_flag) {
		printf(" SDHC : card is not exist\n");
		return -1;
	}

	if (socle_sdhci_card_ident(host, 0)) {
		printf("sd/mmc host: tx mode is fail\n");
		return -1;
	}

	if (MMC_TYPE_SD == host->mmc->card->type) {
		/* Send CMD55 to indicate that the next command is an application specific command */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = host->mmc->card->rca << 16;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		/* Send ACMD6 to select given bus width */
		PDEBUG("CMD : ACMD6 \n");
		send_cmd->opcode = SD_APP_SET_BUS_WIDTH;
		send_cmd->arg = host->bus_mode;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		/* Send CMD6 to select given bus width */
		PDEBUG("CMD : CMD6 \n");
		send_cmd->opcode = MMC_SWITCH;
		send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (host->mmc_bus_width << 8) |EXT_CSD_CMD_SET_NORMAL;
		send_cmd->flags = MMC_RSP_R1B;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
	} else {
		printf("ERROR 1!!!!\n");
	}


	socle_sdhci->sdhci_host_set_bus_width(host);
	
#if 1
	if(host->hs_mode) {

		if (MMC_TYPE_SD == host->mmc->card->type) {		
			//get mode
			mode = 0; //check function
			group = 0;
			value = 1; //high speed

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  (u32)StatusBuffer;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;



			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);


			if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) == 0xF ) {
				printf("ERROR : GET ERROR \n");
				return -1;
			}

			send_cmd->data = NULL;

			//set mode
			mode = 1; //set function
			group = 0;
			host->hs_mode = 1;
			if (host->hs_mode)	
				value = 1; //high speed
			else
				value = 0;

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  SD_STATUS_ADDR;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;

			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);
		} else if (MMC_TYPE_MMC == host->mmc->card->type) {
			/* Send CMD6 to select given bus width */
			PDEBUG("MMC : CMD6 \n");
			send_cmd->opcode = MMC_SWITCH;
			send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_HS_TIMING << 16) | (host->hs_mode << 8) |EXT_CSD_CMD_SET_NORMAL;
			send_cmd->flags = MMC_RSP_R1B;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		
		} else {
		
			printf("ERROR SDIO !!!!\n");
		}

		if (MMC_TYPE_SD == host->mmc->card->type) {
		        if (host->hs_mode){
				// Status indicates the same function number as specified in the argument, 
				// which means Supported function successful function change.
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 1 ) {
					printf("switch HS Fail \n");
					return -1;
				}
		        }
		        else{
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 0 ) {
					printf("switch ERROR \n");
					return -1;	
				}
		        }
		}
		
		send_cmd->data = NULL;
		
		socle_sdhci->sdhci_host_set_hs(host, 1);
		
	}
#endif

	if((socle_host.opmode == 0x10) || (socle_host.opmode == 0x2)) {
		while (nsect_w > 0) {

			//prepare send data 
			data->blksz = 512;
			data->blocks = 1;
			data->sg->dma_address =  TEST_PATTERN_TX_ADDR + block_num_w*512;
			data->sg->length = 512;
			data->flags = MMC_DATA_WRITE;
			send_cmd->data = data;
			
			/* Send CMD24 to write single block */
			PDEBUG("CMD : MMC_WRITE_BLOCK \n");
			send_cmd->opcode = MMC_WRITE_BLOCK;
			send_cmd->arg = cur_addr_w; 			//write block address 
			send_cmd->flags = MMC_RSP_R1;
			if(socle_sdhci->sdhci_send_command(host, send_cmd)) {
				printf("CMD : ERROR \n");
				return -1;
			}
			host->mmc->card->state = RCV;

			host->mmc->card->state = PRG;
			
			
			/* start data transmission */
			host->mmc->card->state = TRAN;

			if(host->hcs)  // for high capacity
				cur_addr_w++;
			else
				cur_addr_w += 512;
			
			block_num_w++;
			nsect_w--;

		}

	}
	
	send_cmd->data = NULL;

	if((socle_host.opmode == 0x10) || (socle_host.opmode == 0x1)) {
		while (nsect_r > 0) {
			//prepare send data 
			data->blksz = 512;
			data->blocks = 1;
			data->sg->dma_address =  TEST_PATTERN_RX_ADDR + (block_num_r*512);
			data->sg->length = 512;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;

	//		host->tc = 1;	
			/* Send CMD17 to read single block */
			PDEBUG("CMD : MMC_READ_SINGLE_BLOCK \n");
			send_cmd->opcode = MMC_READ_SINGLE_BLOCK;
			send_cmd->arg = cur_addr_r; 			//write address from 0
			send_cmd->flags = MMC_RSP_R1;
			if(socle_sdhci->sdhci_send_command(host, send_cmd)) {
				printf("CMD : ERROR \n");
				return -1;
			}
			
			host->mmc->card->state = DATA;

			/* Wait for the reading transmission to be complete */
			/* start data transmission */
			host->mmc->card->state = TRAN;

			if(host->hcs)  // for high capacity
				cur_addr_r++;
			else
				cur_addr_r += 512;

			block_num_r++;
			nsect_r--;
		}
	}
	send_cmd->data = NULL;
	
	if(socle_host.opmode == 0x10) {
		if(socle_sdhc_compare_pattern(TEST_PATTERN_TX_ADDR, TEST_PATTERN_RX_ADDR, 512*cur_addr_w))
			return -1;
	}

	
	return 0;

}

extern int 
socle_sdhc_manual_multiple_block_test(int autotest)
{
	u32 cur_addr_w = 0, cur_addr_r = 0;
	u32 nsect_w, nsect_r;	
	struct socle_sdhc_host *host = &socle_host;
	struct mmc_command *send_cmd = host->cmd;
	send_cmd->data = NULL;
	struct mmc_data *data = &send_date;
	u8 mode, group, value;
	u8 StatusBuffer[64];

	if(socle_host.opmode == 0x1) {
		printf("please enter the read block number : ");
		scanf("%d\n",&cur_addr_r);
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_r);
		printf("read form block %d, count %d \n", cur_addr_r, nsect_r);
	}

	if(socle_host.opmode == 0x2) {
		printf("please enter the write block number : ");
		scanf("%d\n",&cur_addr_w);
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_w);
		printf("write form block %d, count %d \n", cur_addr_w,nsect_w);
	}

	if(socle_host.opmode == 0x10) {
		printf("please enter the loop block number : ");
		scanf("%d\n",&cur_addr_w);
		cur_addr_r = cur_addr_w;
		printf("please enter the block number : ");
		scanf("%d\n",&nsect_w);
		nsect_r = nsect_w;
		printf("loop form block %d, count %d \n", cur_addr_w,nsect_w);
	}


	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, (nsect_w * 512) );
	memset((char *) TEST_PATTERN_RX_ADDR, 0, nsect_r*512);

	if (socle_sdhci->sdhci_host_get_write_protect(host)) {
		printf("get write protect return \n");
		return -1;
	}

	if (0 == host->alive_flag) {
		printf(" SDHC : card is not exist\n");
		return -1;
	}

	if (socle_sdhci_card_ident(host, 0)) {
		printf("sd/mmc host: tx mode is fail\n");
		return -1;
	}

	if (MMC_TYPE_SD == host->mmc->card->type) {
		/* Send CMD55 to indicate that the next command is an application specific command */
		PDEBUG("CMD : CMD55 \n");
		send_cmd->opcode = MMC_APP_CMD;
		send_cmd->arg = host->mmc->card->rca << 16;
		send_cmd->flags = MMC_RSP_R1;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		/* Send ACMD6 to select given bus width */
		PDEBUG("CMD : ACMD6 \n");
		send_cmd->opcode = SD_APP_SET_BUS_WIDTH;
		send_cmd->arg = host->bus_mode;
		send_cmd->flags = MMC_RSP_R1;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
	} else if (MMC_TYPE_MMC == host->mmc->card->type) {
		/* Send CMD6 to select given bus width */
		PDEBUG("CMD : CMD6 \n");
		send_cmd->opcode = MMC_SWITCH;
		send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16) | (host->mmc_bus_width << 8) |EXT_CSD_CMD_SET_NORMAL;
		send_cmd->flags = MMC_RSP_R1B;
		send_cmd->data = NULL;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
	} else {
		printf("ERROR 1!!!!\n");
	}

	socle_sdhci->sdhci_host_set_bus_width(host);
	
#if 1
	if(host->hs_mode) {
		
		if (MMC_TYPE_SD == host->mmc->card->type) {				
			
			//get mode
			mode = 0; //check function
			group = 0;
			value = 1; //high speed

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  (u32)StatusBuffer;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;



			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);


			if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) == 0xF ) {
				printf("ERROR : GET ERROR \n");
				return -1;
			}

			send_cmd->data = NULL;

			//set mode
			mode = 1; //set function
			group = 0;
			host->hs_mode = 1;
			if (host->hs_mode)	
				value = 1; //high speed
			else
				value = 0;

			data->blksz = 64;
			data->blocks = 1;
			data->sg->dma_address =  SD_STATUS_ADDR;
			data->sg->length = 1;
			data->flags = MMC_DATA_READ;
			send_cmd->data = data;

			/* Send CMD6 to select given bus speed */
			PDEBUG("CMD : CMD6 \n");
			send_cmd->opcode = SD_SWITCH;
			send_cmd->arg = mode << 31 | 0x00FFFFFF;
			send_cmd->arg &= ~(0xF << (group * 4));
			send_cmd->arg |= value << (group * 4);
			send_cmd->flags = MMC_RSP_R1;

			socle_sdmmc_to_cnt = 0;
			do {
				if (socle_sdmmc_to_cnt > TO_THRESHOLD_CNT) {
					printf("\nSocle SD/MMC host: sd status is timeout\n");
					return -1;
				}
				if(socle_sdhci->sdhci_send_command(host, send_cmd))
					return -1;

				// check data structure version
				// if it is 1 then checking the busy status is possible
				if ( SDCARD_SWITCH_FUNC_GET_DATA_STRUCT_VER(StatusBuffer) == 1 ){
					printf("VESRION = 1 \n");
					// if function is busy continue loop
					// wait untill function will be ready
						if ( !SDCARD_SWICH_FUNC_GET_BUSY_STAT(StatusBuffer,1,1) ) {
							printf("! BUSY \n");
							break;
						}
				} else {
					break;
				}
				socle_sdmmc_to_cnt++;

			}while (1);

		} else if (MMC_TYPE_MMC == host->mmc->card->type) {
			/* Send CMD6 to select given bus width */
			PDEBUG("MMC : CMD6 \n");
			send_cmd->opcode = MMC_SWITCH;
			send_cmd->arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_HS_TIMING << 16) | (host->hs_mode << 8) |EXT_CSD_CMD_SET_NORMAL;
			send_cmd->flags = MMC_RSP_R1B;
			send_cmd->data = NULL;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		
		} else {
		
			printf("ERROR SDIO !!!!\n");
		}

		if (MMC_TYPE_SD == host->mmc->card->type) {
		        if (host->hs_mode){
				// Status indicates the same function number as specified in the argument, 
				// which means Supported function successful function change.
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 1 ) {
					printf("switch HS Fail \n");
					return -1;
				}
		        }
		        else{
				if ( SDCARD_SWICH_FUNC_GET_STAT_CODE(StatusBuffer,1) != 0 ) {
					printf("switch ERROR \n");
					return -1;	
				}
		        }
		}
		
		send_cmd->data = NULL;
		
		socle_sdhci->sdhci_host_set_hs(host, 1);
		
	}
#endif

	if((socle_host.opmode == 0x10) || (socle_host.opmode == 0x2)) {
		//prepare send write data 
		data->blksz = 512;
		data->blocks = nsect_w;
		data->sg->dma_address =  TEST_PATTERN_TX_ADDR;
		data->sg->length = 512 * nsect_w;
		data->flags = MMC_DATA_WRITE;
		send_cmd->data = data;
		
		/* Send CMD25 to write multiple block */
		PDEBUG("CMD : MMC_WRITE_MULTIPLE_BLOCK \n");
		send_cmd->opcode = MMC_WRITE_MULTIPLE_BLOCK;
		send_cmd->arg = cur_addr_w;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;

		host->mmc->card->state = RCV;

		/* Send CMD12 to stop */
		if (host->AutoCMD12Enable == 0) {
			PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");
			send_cmd->opcode = MMC_STOP_TRANSMISSION;
			send_cmd->arg = 0;
			send_cmd->flags = MMC_RSP_R1B;
			send_cmd->data = NULL;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		}
	}
	
	host->mmc->card->state = TRAN;
	
	if((socle_host.opmode == 0x10) || (socle_host.opmode == 0x1)) {

		//prepare send read data 
		data->blksz = 512;
		data->blocks = nsect_r;
		data->sg->dma_address =  TEST_PATTERN_RX_ADDR;
		data->sg->length = 512 * nsect_r;
		data->flags = MMC_DATA_READ;
		send_cmd->data = data;
		
		/* Send CMD18 to read multiple block */
		PDEBUG("CMD : MMC_READ_MULTIPLE_BLOCK \n");	
		send_cmd->opcode = MMC_READ_MULTIPLE_BLOCK;
		send_cmd->arg = cur_addr_r;
		send_cmd->flags = MMC_RSP_R1;
		if(socle_sdhci->sdhci_send_command(host, send_cmd))
			return -1;
		
		host->mmc->card->state = DATA;

		
		
		if (host->AutoCMD12Enable == 0) {
			/* Send CMD12 to stop */
			PDEBUG("CMD : MMC_STOP_TRANSMISSION \n");	
			send_cmd->opcode = MMC_STOP_TRANSMISSION;
			send_cmd->arg = 0;
			send_cmd->flags = MMC_RSP_R1B;
			send_cmd->data = NULL;
			if(socle_sdhci->sdhci_send_command(host, send_cmd))
				return -1;
		}

	}

	if(socle_host.opmode == 0x10) {	
		if(socle_sdhc_compare_pattern(TEST_PATTERN_TX_ADDR, TEST_PATTERN_RX_ADDR, 512*cur_addr_w))
			return -1;
	}
	
	return 0;
//////////////////////////////////////////////////////////
}

extern struct test_item_container socle_sdhc_manual_test_container;

extern int 
socle_sdhc_manual_read_block_test(int autotest)
{
	int ret = 0;
	socle_host.opmode = 0x1;
	ret = test_item_ctrl(&socle_sdhc_manual_test_container, autotest);
	return ret;
}

extern int 
socle_sdhc_manual_write_block_test(int autotest)
{
	int ret = 0;
	socle_host.opmode = 0x2;	
	ret = test_item_ctrl(&socle_sdhc_manual_test_container, autotest);
	return ret;
}

extern int 
socle_sdhc_manual_rw_loop_test(int autotest)
{
	int ret = 0;
	socle_host.opmode = 0x10;
	ret = test_item_ctrl(&socle_sdhc_manual_test_container, autotest);
	return ret;
}

extern struct test_item_container socle_sdhc_manual_opmode_test_container;

extern int
socle_sdhc_manual(int autotest)
{
	int ret = 0;
	
	ret = test_item_ctrl(&socle_sdhc_manual_opmode_test_container, autotest);
	return ret;

}

extern int 
socle_sdhc_sdio_wifi_test(int autotest)
{
	return 0;
}

extern struct test_item socle_sdmmc_sd_main_test_items[];
extern struct test_item socle_sdhc_speed_main_test_items[];
extern struct test_item socle_sdmmc_sd_xfer_test_items[];
extern struct test_item socle_sdhc_auto_cmd12_test_items[];

static int
sdhc_sub_host_test(int autotest)
{
	u32 ret = 0;
	struct mmc_host *mmc = &socle_mmc;
	struct socle_sdhc_host *host = &socle_host;

	mmc->card = &socle_mmc_card;

	mmc->f_min = 375000;
	mmc->f_max = 25000000;
//	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_29_30|MMC_VDD_30_31 | MMC_VDD_165_195;
	mmc->caps = MMC_CAP_4_BIT_DATA; //FIXME
	host->cmd = &cmd;	
	host->mmc = &socle_mmc;
	host->buf = NULL;
	host->bus_mode = 0;

	if(!socle_sdhci->sdhci_card_detect(host)) {
		printf("\nSocle SD/MMC host: card is not exist!! \n");
		printf("\nSocle SD/MMC host: Please insert card !! \n");
		return -1;
	}

	if(socle_sdhci->sdhci_host_initialize(host)) {
		printf("SOCLE SD Host Initial fail \n");
		return -1;
	}
	
	request_irq(socle_sdhci_irq, socle_sdhci->sdhci_host_isr, &socle_host);


	if(socle_sdhci_card_ident(host,1)) {
		return -1;
	}
	
	if (MMC_TYPE_SD == host->mmc->card->type) {
		if(MMC_TYPE_SDIO & host->mmc->card->type)
			printf("Combo Card !!\n");
		printf("SD Card !!\n");
		ret = test_item_ctrl(&socle_sdmmc_sd_main_test_container, autotest);
	}
	else if (MMC_TYPE_MMC == host->mmc->card->type) {
		if(MMC_TYPE_SDIO & host->mmc->card->type)
			printf("Combo Card !!\n");
		printf("MMC Card !!\n");

		socle_sdhc_speed_main_test_items[0].enable = 0;
		socle_sdmmc_sd_main_test_items[2].enable = 0;
		socle_sdmmc_sd_xfer_test_items[2].allow_autotest = 0;
		socle_sdhc_auto_cmd12_test_items[1].allow_autotest = 0;
			
		ret = test_item_ctrl(&socle_sdmmc_sd_main_test_container, autotest);

		socle_sdhc_speed_main_test_items[0].enable = 1;
		socle_sdmmc_sd_main_test_items[2].enable = 1;
		socle_sdmmc_sd_xfer_test_items[2].allow_autotest = 1;
		socle_sdhc_auto_cmd12_test_items[1].allow_autotest = 1;
	}
	else if (MMC_TYPE_SDIO == host->mmc->card->type) {
		printf("\nSocle SD/MMC host: SDIO CARD \n");
		return 0;
	}
	else {
		printf("\nSocle SD/MMC host: unknown card type %d \n",host->mmc->card->type);
		return -1;
	}

	/* Send CMD0 to go to idle state */
	//go idle 
	PDEBUG("MMC_GO_IDLE_STATE \n");
	cmd.opcode = MMC_GO_IDLE_STATE;
	cmd.arg = 0;
	cmd.flags = MMC_RSP_NONE ;
	cmd.data = NULL;
	if(socle_sdhci->sdhci_send_command(&socle_host, &cmd)) {
		printf("\nSocle SD/MMC host: cannot go to \"Idle State\", please re-insert the card\n");
		return -1;
	}
	
	free_irq(socle_sdhci_irq);

	return ret;
}

extern int 
socle_sdhc_host_slot0_test(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	host->slot = 0;
	return sdhc_sub_host_test(autotest);
}

extern int 
socle_sdhc_host_slot1_test(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	host->slot = 1;

	return sdhc_sub_host_test(autotest);
}

extern int 
socle_sdhc_host_slot2_test(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	host->slot = 2;

	return sdhc_sub_host_test(autotest);
}

extern int 
socle_sdhc_host_slot3_test(int autotest)
{
	struct socle_sdhc_host *host = &socle_host;
	host->slot = 3;

	return sdhc_sub_host_test(autotest);
}

extern struct test_item_container socle_sdhc_host_slot_test_container;

extern int
socle_sdhc_host_0_test(int autotest)
{
	int ret;
	
#if defined (CONFIG_SDMMC)
	socle_sdhci = &socle_sdmmc_host_ops; 
	socle_sdhc_base = SOCLE_SDMMC0;
	socle_sdhci_irq = SOCLE_INTC_SDMMC0;
#elif defined (CONFIG_SDHC)
	socle_sdhci = &socle_sdhc_host_ops;
	socle_sdhc_base = SQ_SDHC0;
	socle_sdhci_irq = SQ_INTC_SDHC0;
#else
	#error not define SDHC Host IP
#endif
	printf("base : %x , irq %d \n",socle_sdhc_base,socle_sdhci_irq);
	ret = test_item_ctrl(&socle_sdhc_host_slot_test_container, autotest);

	return ret;

}

extern int
socle_sdhc_host_1_test(int autotest)
{
	int ret;
	
#if defined (CONFIG_SDMMC)
	socle_sdhci = &socle_sdmmc_host_ops; 
	socle_sdhc_base = SOCLE_SDMMC1;
	socle_sdhci_irq = SOCLE_INTC_SDMMC1;
#elif defined (CONFIG_SDHC)
	socle_sdhci = &socle_sdhc_host_ops;
	socle_sdhc_base = SOCLE_SDHC1;
	socle_sdhci_irq = SOCLE_INTC_SDHC1;
#else
	#error not define SDHC Host IP
#endif

	ret = test_item_ctrl(&socle_sdhc_host_slot_test_container, autotest);

	return ret;

}

extern int
sdhc_host_test(int autotest)
{
	int ret;

#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_SDMMC);	
#endif

	memset((char *) TEST_PATTERN_TX_ADDR, 0, TEST_PATTERN_BUF_SIZE);
	memset((char *) TEST_PATTERN_RX_ADDR, 0, TEST_PATTERN_BUF_SIZE);

	socle_sdhc_prepare_pattern((u8 *) TEST_PATTERN_TX_ADDR, TEST_SECT_NUM * 512);


	ret = test_item_ctrl(&socle_sdhc_host_test_container, autotest);

	return ret;
}

