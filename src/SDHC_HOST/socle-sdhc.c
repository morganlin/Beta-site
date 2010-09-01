#include <test_item.h>
#include <genlib.h>
#include <dma/dma.h>
#include "dependency.h"
#include "socle-sdhc-regs.h"
#include "sdhc.h"
#include "mmc.h"
#include "host.h"
#include "sd.h"
#include "card.h"
#include "core.h"
#include "sdio.h"

//#define SDHC_DEBUG 

#ifdef SDHC_DEBUG
#define PDEBUG(fmt, args...) printf(fmt, ## args)
#else
#define PDEBUG(fmt, args...)
#endif

struct sdhc_adma2 {
	u32	ctrl;
	u32	address;
};

static u32 socle_sdhc_adma1[TEST_MULTI_BLOCK_NUM];
static struct sdhc_adma2 socle_sdhc_adma2[TEST_MULTI_BLOCK_NUM];

extern u32 socle_sdhc_base;
	
static inline void
socle_sdhc_write(u32 val, u32 reg)
{
	iowrite32(val, socle_sdhc_base + reg);
}

static inline u32
socle_sdhc_read(u32 reg)
{
	u32 val;
	val = ioread32(socle_sdhc_base + reg);
	return val;
}

static u8 socle_sdhc_host_initialize(struct socle_sdhc_host *host);
static void socle_sdhc_isr(void *data);
static u8 socle_sdhc_send_command(struct socle_sdhc_host *host, struct mmc_command *cmd);
static u8 socle_sdhc_card_detect(struct socle_sdhc_host *host);
static u8 socle_sdhc_device_attach(struct socle_sdhc_host *host);
static void socle_sdhc_set_bus_width(struct socle_sdhc_host *host);
static void socle_sdhc_set_hs(struct socle_sdhc_host *host, u8 speed);
static u8 socle_sdhc_dma_prepare(struct socle_sdhc_host *host, struct mmc_data *data);
static u8 socle_sdhc_get_write_protect(struct socle_sdhc_host *host);
static u8 socle_sdhc_set_sd_clk(struct socle_sdhc_host *host,u32 FrequencyKHz);



struct socle_sdhci_ops socle_sdhc_host_ops = {
	.sdhci_host_initialize = socle_sdhc_host_initialize,
	.sdhci_host_isr = socle_sdhc_isr,
	.sdhci_send_command = socle_sdhc_send_command,
	.sdhci_card_detect = socle_sdhc_card_detect,
	.sdhci_host_set_bus_width = socle_sdhc_set_bus_width, 	
	.sdhci_host_set_hs = socle_sdhc_set_hs, 		
	.sdhci_host_get_write_protect = socle_sdhc_get_write_protect,
	.sdhci_host_set_clk = socle_sdhc_set_sd_clk,
};

//------------------------------------------------------------------------------------------
static u8 SDIOHost_SetTimeout (struct socle_sdhc_host *host, u32 TimeoutVal )
{
	if( !(TimeoutVal & SFR11_TIMEOUT_MASK) )
		return -1;

	socle_sdhc_write((socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) & SFR11_TIMEOUT_MASK) | TimeoutVal, SOCLE_SDHC_SRS11(host->slot));

	return 0;
}
//------------------------------------------------------------------------------------------

static u8 
socle_sdhc_host_initialize(struct socle_sdhc_host *host)
{
	u8 SlotsAvailability;
	u8 NumberOfSlots;
	u32 i;
	u32 DP;

    	printf("SDIOHost_HostInitialize : Start host initializing... \n" );

	// reset controller for sure
	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_HRS0) | HSFR0_SOFTWARE_RESET, SOCLE_SDHC_HRS0);
	// wait for clear 
	while(socle_sdhc_read(SOCLE_SDHC_HRS0) & HSFR0_SOFTWARE_RESET);

	//DP * Tclk = 20ms
	DP = sq_scu_ahb_clock_get() * 20 /1000; 
//	printf("DP = %x \n", DP);
	socle_sdhc_write(DEBOUNCING_TIME, SOCLE_SDHC_HRS1);

	SlotsAvailability = (socle_sdhc_read(SOCLE_SDHC_HRS0) & HSFR0_AVAILABLE_SLOT) >> 16;
	NumberOfSlots = 0;
	for (i = 0; i < 4; i++){
		if ((SlotsAvailability >> i) & 1){
			NumberOfSlots++;
		}
	}
	printf("SDHC : Found = %d slots \n",NumberOfSlots);


        // NumberOfSlots can't be 0
        if ( NumberOfSlots == 0 )
            return -1;

// IRQ ENABLE
	socle_sdhc_write(SFR13_AUTO_CMD12_ERR_STAT_EN
            | SFR13_CURRENT_LIMIT_ERR_STAT_EN   
            | SFR13_DATA_END_BIT_ERR_STAT_EN      
            | SFR13_DATA_CRC_ERR_STAT_EN          
            | SFR13_DATA_TIMEOUT_ERR_STAT_EN      
            | SFR13_COMMAND_INDEX_ERR_STAT_EN     
            | SFR13_COMMAND_END_BIT_ERR_STAT_EN   
            | SFR13_COMMAND_CRC_ERR_STAT_EN       
            | SFR13_COMMAND_TIMEOUT_ERR_STAT_EN   
#if ENABLE_CARD_INTERRUPT                                           
            | SFR13_CARD_INTERRUPT_SIG_EN
#endif
            | SFR13_CARD_REMOVAL_STAT_EN          
            | SFR13_CARD_INERTION_STAT_EN         
            | SFR13_BUFFER_READ_READY_STAT_EN     
            | SFR13_BUFFER_WRITE_READY_STAT_EN    
            | SFR13_DMA_INTERRUPT_STAT_EN         
            | SFR13_BLOCK_GAP_EVENT_STAT_EN       
            | SFR13_TRANSFER_COMPLETE_STAT_EN     
            | SFR13_COMMAND_COMPLETE_STAT_EN
		,SOCLE_SDHC_SRS13(host->slot));

#if SDIO_INTERRUPT_SUPPORT
	socle_sdhc_write(SFR14_AUTO_CMD12_ERR_SIG_EN
            | SFR14_CURRENT_LIMIT_ERR_SIG_EN
            | SFR14_DATA_END_BIT_ERR_SIG_EN   
            | SFR14_DATA_CRC_ERR_SIG_EN       
            | SFR14_DATA_TIMEOUT_ERR_SIG_EN   
            | SFR14_COMMAND_INDEX_ERR_SIG_EN  
            | SFR14_COMMAND_END_BIT_ERR_SIG_EN
            | SFR14_COMMAND_CRC_ERR_SIG_EN    
            | SFR14_COMMAND_TIMEOUT_ERR_SIG_EN
#if ENABLE_CARD_INTERRUPT                                           
            | SFR14_CARD_INTERRUPT_SIG_EN
#endif
            | SFR14_CARD_REMOVAL_SIG_EN       
            | SFR14_CARD_INERTION_SIG_EN      
            | SFR14_BUFFER_READ_READY_SIG_EN  
            | SFR14_BUFFER_WRITE_READY_SIG_EN 
            | SFR14_DMA_INTERRUPT_SIG_EN      
            | SFR14_BLOCK_GAP_EVENT_SIG_EN    
            | SFR14_TRANSFER_COMPLETE_SIG_EN  
            | SFR14_COMMAND_COMPLETE_SIG_EN
		,SOCLE_SDHC_SRS14(host->slot));
#endif

//      SlotsArray[i].CardInserted = 0;
//FIXME
//	SDIOHost_SetTimeout( SFR11_TIMEOUT_TMCLK_X_2_POWER_21);
	SDIOHost_SetTimeout(host, SFR11_TIMEOUT_TMCLK_X_2_POWER_27);

	if(host->alive_flag == 1) {
		socle_sdhc_device_attach(host);
	}
	
	return 0;

}

static u8 socle_sdhc_check_err(struct socle_sdhc_host *host)
{
    u32 SRS12, SRS15;
    u8 Error =0;
    // number of interrupt to clear
    volatile u32 IntToClear = 0;

//	printf("socle_sdhc_check_err \n");
	SRS12 = socle_sdhc_read(SOCLE_SDHC_SRS12(host->slot));
	SRS15 = socle_sdhc_read(SOCLE_SDHC_SRS15(host->slot));

	if(SRS12 & SFR12_AUTO_CMD12_ERROR) {
		printf("SFR12_AUTO_CMD12_ERROR \n");
		if ( SRS15 & SFR15_CMD_NOT_ISSUED_ERR )
			printf("SFR15_CMD_NOT_ISSUED_ERR \n");
		if ( SRS15 & SFR15_AUTO_CMD12_INDEX_ERR )
			printf("SFR15_AUTO_CMD12_INDEX_ERR \n");
		if ( SRS15 & SFR15_AUTO_CMD12_END_BIT_ERR )
			printf("SFR15_AUTO_CMD12_END_BIT_ERR \n");
		if ( SRS15 & SFR15_AUTO_CMD12_CRC_ERR ) {
			printf("SFR15_AUTO_CMD12_CRC_ERR \n");
//			if(SRS15 & SFR15_AUTO_CMD12_TIMEOUT_ERR)
//				printf("SFR15_AUTO_CMD12_TIMEOUT_ERR \n");
//			else
//				printf("!!! SFR15_AUTO_CMD12_TIMEOUT_ERR \n");
		}
		if ( SRS15 & SFR15_AUTO_CMD12_TIMEOUT_ERR ) 
			printf("SFR15_AUTO_CMD12_TIMEOUT_ERR \n");
		if ( SRS15 & SFR15_AUTO_CMD12_NOT_EXECUTED )
			printf("SFR15_AUTO_CMD12_NOT_EXECUTED \n");

		IntToClear = SFR12_AUTO_CMD12_ERROR | SFR12_ERROR_INTERRUPT;
	}

	if(SRS12 & SFR12_CURRENT_LIMIT_ERROR ) {
		printf("SFR12_CURRENT_LIMIT_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_CURRENT_LIMIT_ERROR;
	}
	if ( SRS12 & SFR12_DATA_END_BIT_ERROR ){
		printf("SFR12_DATA_END_BIT_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_DATA_END_BIT_ERROR;
	}	
	if ( SRS12 & SFR12_DATA_CRC_ERROR ){
		printf("SFR12_DATA_CRC_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_DATA_CRC_ERROR;
	}
	if ( SRS12 & SFR12_DATA_TIMEOUT_ERROR ){
		printf("SFR12_DATA_TIMEOUT_ERROR \n");
		host->err_flag = MMC_ERR_TIMEOUT;
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_DATA_TIMEOUT_ERROR;
	}
	if ( SRS12 & SFR12_COMMAND_INDEX_ERROR ){
		printf("SFR12_COMMAND_INDEX_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_COMMAND_INDEX_ERROR;
	}
	if ( SRS12 & SFR12_COMMAND_END_BIT_ERROR ){
		printf("SFR12_COMMAND_END_BIT_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_COMMAND_END_BIT_ERROR;
	}
	if ( SRS12 & SFR12_COMMAND_CRC_ERROR ){
		printf("SFR12_COMMAND_CRC_ERROR \n");
		if ( SRS12 & SFR12_COMMAND_TIMEOUT_ERROR ){
			IntToClear = SFR12_ERROR_INTERRUPT | SFR12_COMMAND_CRC_ERROR | SFR12_COMMAND_TIMEOUT_ERROR ;
		}
		else{
			IntToClear = SFR12_ERROR_INTERRUPT | SFR12_COMMAND_CRC_ERROR;
		}
	}
	if ( SRS12 & SFR12_COMMAND_TIMEOUT_ERROR ){
		PDEBUG("SFR12_COMMAND_TIMEOUT_ERROR \n");
		host->err_flag = MMC_ERR_TIMEOUT;
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_COMMAND_TIMEOUT_ERROR;
	}

	if ( SRS12 & SFR12_ADMA_ERROR ){
		printf("SFR12_ADMA_ERROR \n");
		IntToClear = SFR12_ERROR_INTERRUPT | SFR12_ADMA_ERROR;
	}
		
    // clear interrupt
    socle_sdhc_write(IntToClear, SOCLE_SDHC_SRS12(host->slot));

    return Error;
}

static void
socle_sdhc_get_resp(struct socle_sdhc_host *host)
{
	struct mmc_command *cmd;
	u32 tmp0,tmp1,tmp2,tmp3;

	cmd = host->cmd;

	// read response from response register 
	switch(cmd->flags) {
		case MMC_RSP_R2:	

			tmp3 = socle_sdhc_read(SOCLE_SDHC_SRS4(host->slot));
			tmp2 = socle_sdhc_read(SOCLE_SDHC_SRS5(host->slot));
			tmp1 = socle_sdhc_read(SOCLE_SDHC_SRS6(host->slot));
			tmp0 = socle_sdhc_read(SOCLE_SDHC_SRS7(host->slot));
			
			cmd->resp[3] = tmp3 <<8;
			cmd->resp[2] = tmp2 <<8 | ((tmp3 & 0xff000000) >> 24);
			cmd->resp[1] = tmp1 <<8 | ((tmp2 & 0xff000000) >> 24);
			cmd->resp[0] = tmp0 <<8 | ((tmp1 & 0xff000000) >> 24);

			PDEBUG("resp : %x, %x, %x, %x \n",cmd->resp[0],cmd->resp[1],cmd->resp[2],cmd->resp[3]);
			break;
		case MMC_RSP_R1:
		case MMC_RSP_R1B:
			cmd->resp[0] = socle_sdhc_read(SOCLE_SDHC_SRS4(host->slot));	
			PDEBUG("Current state = %d \n",(cmd->resp[0] & 0xf << 9) >> 9);
			break;
		default:
			cmd->resp[0] = socle_sdhc_read(SOCLE_SDHC_SRS4(host->slot));
//			printf("resp = %x \n",cmd->resp[0]);
			break;
			
	}
	
    
}

//------------------------------------------------------------------------------------------
static u8 
socle_sdhc_set_sd_clk(struct socle_sdhc_host *host,u32 FrequencyKHz )
{
	u32 BaseCLKkHz;
	u32 i;
	u32 Temp;


	// if requested frequency is more than 25MHz then return error
	if ( FrequencyKHz > 50000 )
	    return -1;

	// set SD clock off
	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) & ~SFR11_SD_CLOCK_ENABLE, SOCLE_SDHC_SRS11(host->slot));

    	//read base clock frequency for SD clock in kilo herz
    	BaseCLKkHz = SFR16_GET_BASE_CLK_FREQ_MHZ(socle_sdhc_read(SOCLE_SDHC_SRS16(host->slot))) * 1000;
//printf("BaseCLKkHz = %d\n", BaseCLKkHz);
	if ( !BaseCLKkHz ) {
#if !SYTEM_CLK_KHZ
		printf("SDIO_ERR_BASE_CLK_IS_ZERO \n");
		return -1;
#endif
		BaseCLKkHz = SYTEM_CLK_KHZ;
	}

	for ( i = 1; i < 512; i = 2 * i ){
		if ( ( BaseCLKkHz / i ) <= FrequencyKHz ){
			break; 
		}
	}

//	printf("i = %x \n", i);
	
	// read current value of SFR11 register    
	Temp = socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot));

	// clear old frequency base settings 
	Temp &= ~SFR11_SEL_FREQ_BASE_MASK;

	// Set SDCLK Frequency Select and Internal Clock Enable 
//printf("CLK FIXME \n");
	Temp |= ( ( i / 2 ) << 8 ) | SFR11_INT_CLOCK_ENABLE;
//	Temp |= ( 0x1 << 8 ) | SFR11_INT_CLOCK_ENABLE;
//printf("Temp = 0x%x\n", Temp);
	socle_sdhc_write(Temp, SOCLE_SDHC_SRS11(host->slot));

	// wait for clock stable is 1
	while(!(socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) &  SFR11_INT_CLOCK_STABLE));

	// set SD clock on
	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) |SFR11_SD_CLOCK_ENABLE , SOCLE_SDHC_SRS11(host->slot));

	// write to FrequencyKHz the real value of set frequecy  
	FrequencyKHz = BaseCLKkHz / i;

	return 0;
}

#if 0
static u8 
socle_sdhc_set_power(u32 Voltage)
{
	u32 Temp;

	Temp = socle_sdhc_read(SOCLE_SDHC_SRS10(0));
	Temp &= ~SFR10_BUS_VOLTAGE_MASK; 
	switch (Voltage){
		case SFR10_SET_3_3V_BUS_VOLTAGE:
			if (! (socle_sdhc_read(SOCLE_SDHC_SRS16(0)) & SFR16_VOLTAGE_3_3V_SUPPORT ) )
				return -1;

			// set new volatage value
			Temp |= SFR10_SET_3_3V_BUS_VOLTAGE | SFR10_SD_BUS_POWER;
			socle_sdhc_write(Temp, SOCLE_SDHC_SRS10(0));
		break;
		
		case SFR10_SET_3_0V_BUS_VOLTAGE:
			if (! ( socle_sdhc_read(SOCLE_SDHC_SRS16(0)) & SFR16_VOLTAGE_3_0V_SUPPORT ) )
				return -1;
			// set new volatage value
			Temp |= SFR10_SET_3_0V_BUS_VOLTAGE | SFR10_SD_BUS_POWER;
			socle_sdhc_write(Temp, SOCLE_SDHC_SRS10(0));

		break;
		
		case SFR10_SET_1_8V_BUS_VOLTAGE:
			if (! ( socle_sdhc_read(SOCLE_SDHC_SRS16(0)) & SFR16_VOLTAGE_1_8V_SUPPORT ) )
				return -1;
			// set new volatage value
			Temp |= SFR10_SET_1_8V_BUS_VOLTAGE | SFR10_SD_BUS_POWER;
			socle_sdhc_write(Temp, SOCLE_SDHC_SRS10(0));
		break;

		default: 
			return -1;
	}

    return 0;
}

static u32 
socle_sdhc_set_ocr(u32 OCR)
{
    u8 Status;
    /// host voltage capabilities
    u32 HostCapabilities;
    ///  current host volatge settings
    u32 HostSettings;
    /// new host voltage settings
    u32 NewControllerVoltage = 0;
    // card voltage capabilities
    u32 OCRVoltage = 255;


	HostCapabilities = socle_sdhc_read(SOCLE_SDHC_SRS16(0));
	HostSettings = socle_sdhc_read(SOCLE_SDHC_SRS10(0));

	// check the voltage capabilities of the SDIO host controller and a card
	// to set appriopriate voltage
	do{ 
		if ( HostCapabilities & SFR16_VOLTAGE_3_3V_SUPPORT ) {
			if ( OCR & SDCARD_REG_OCR_3_3_3_4 ) {
				OCRVoltage = SDCARD_REG_OCR_3_3_3_4;
				NewControllerVoltage = SFR10_SET_3_3V_BUS_VOLTAGE;
			break;
			}
			if ( OCR & SDCARD_REG_OCR_3_2_3_3 ){
				OCRVoltage = SDCARD_REG_OCR_3_2_3_3;
				NewControllerVoltage = SFR10_SET_3_3V_BUS_VOLTAGE;
			break;
			}
		}
		if ( HostCapabilities & SFR16_VOLTAGE_3_0V_SUPPORT ) {
			if ( OCR & SDCARD_REG_OCR_3_0_3_1 ){
				OCRVoltage = SDCARD_REG_OCR_3_0_3_1;
				NewControllerVoltage = SFR10_SET_3_0V_BUS_VOLTAGE;
				break;
			}
			if ( OCR & SDCARD_REG_OCR_2_9_3_0 ){
				OCRVoltage = SDCARD_REG_OCR_2_9_3_0;
				NewControllerVoltage = SFR10_SET_3_0V_BUS_VOLTAGE;
				break;
			}
		}
	}while(0);
	
    // if it is not 0 then a SD card can be supplied with voltage which the card supports
    if ( NewControllerVoltage ){
        if ( NewControllerVoltage != ( HostSettings & NewControllerVoltage ) ){
            Status = socle_sdhc_set_power(NewControllerVoltage );
            if ( Status )
                return 255; // if error return 255 ( can't supplay a volatage to card )
        }
    }
    return OCRVoltage;
}
#endif
static u8
socle_sdhc_get_write_protect(struct socle_sdhc_host *host)
{
	if(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot)) & SFR9_WP_SWITCH_LEVEL) {
		host->wp = 1;
		return 1;
	} else {
		host->wp = 0;
		return 0;
	}
}

static void
socle_sdhc_set_hs(struct socle_sdhc_host *host, u8 speed)
{

	if (speed){
		//printf("set HS \n");
		// enable highspeed in the SDIO host controller
		socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) | SFR10_HIGH_SPEED_ENABLE, SOCLE_SDHC_SRS10(host->slot));
	}
	else{
		//printf("disable HS \n");
		// dissable highspeed in the SDIO host controller
		socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & ~SFR10_HIGH_SPEED_ENABLE , SOCLE_SDHC_SRS10(host->slot));
		
	}
	
}

static void
socle_sdhc_set_bus_width(struct socle_sdhc_host *host)
{
	// set bus width in the host
	switch(host->bus_mode) {
		case MMC_BUS_WIDTH_1:
			PDEBUG("set MMC_BUS_WIDTH_1 \n");
			socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & ~SFR10_DATA_WIDTH_4BIT, SOCLE_SDHC_SRS10(host->slot));
			// dissable mmc8 mode
		    	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_HRS0) & ~HSFR0_MMC8_MASK , SOCLE_SDHC_HRS0);
		break;
		case MMC_BUS_WIDTH_4:
			PDEBUG("set MMC_BUS_WIDTH_4 \n");			
			socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) | SFR10_DATA_WIDTH_4BIT, SOCLE_SDHC_SRS10(host->slot));
			// dissable mmc8 mode
		    	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_HRS0) & ~HSFR0_MMC8_MASK , SOCLE_SDHC_HRS0);
		break;
		case MMC_BUS_WIDTH_8:
			PDEBUG("set MMC_BUS_WIDTH_8 \n");			
			// dissable mmc8 mode
		    	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_HRS0) | HSFR0_MMC8_MASK , SOCLE_SDHC_HRS0);
		break;
		default:
			printf("socle_sdhc_set_bus_width error !!\n");
		break;
	}
		
}

static u8 
socle_sdhc_host_trans_datablock(struct socle_sdhc_host *host,struct mmc_data *data)
{
	u32 index;
	int i;

	index = host->buf_idx;	

	if (data->flags == MMC_DATA_READ) {
		while(!(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot)) & SFR9_BUFF_READ_EN));
//		printf("read  SOCLE_SDHC_SRS8\n");
		for (i = 0; i < (data->blksz >> 2); i++) {
			host->buf[index++] = socle_sdhc_read(SOCLE_SDHC_SRS8(host->slot));
			while(!(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot) & SFR9_BUFF_READ_EN)));
		}

	} else {
		while(!(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot)) & SFR9_BUFF_WRITE_EN));

//		printf("write  SOCLE_SDHC_SRS8\n");
		for (i = 0; i < (data->blksz >> 2); i++) {
//			printf("DATA = %x \n",host->buf[index]);
			socle_sdhc_write(host->buf[index++], SOCLE_SDHC_SRS8(host->slot));
			while(!(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot) & SFR9_BUFF_WRITE_EN)));
		}
	}

//	printf("FIFO done \n");
	host->buf_idx = index;
	data->bytes_xfered += data->blksz;
	
	host->blk_idx++;
	return 0;

}

static u8 
socle_sdhc_device_deattach (struct socle_sdhc_host *host)
{
	printf("SDIOHost_DeviceDetach...\n");
	
	// reset CMD line
	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) | SFR11_SOFT_RESET_CMD_LINE | SFR11_SOFT_RESET_DAT_LINE, SOCLE_SDHC_SRS11(host->slot));
	while(socle_sdhc_read(SOCLE_SDHC_SRS11(host->slot)) & (SFR11_SOFT_RESET_DAT_LINE | SFR11_SOFT_RESET_CMD_LINE) );
	host->alive_flag = 0;

	
	return 0;
}
//------------------------------------------------------------------------------------------

static u8 
socle_sdhc_device_attach(struct socle_sdhc_host *host)
{

//	printf("socle_sdhc_device_attach \n");

	// set 1 bit bus mode
	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & ~SFR10_DATA_WIDTH_4BIT ,SOCLE_SDHC_SRS10(host->slot));
    	// dissable mmc8 mode
    	socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_HRS0) & ~HSFR0_MMC8_MASK , SOCLE_SDHC_HRS0);

	// SD clock supply before Issue a SD command
	// clock frequency is set to 400kHz 
	//Status = socle_sdhc_set_sd_clk (host, 400);
	
//Ryan Add for xDK 3 
        socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot))| SFR10_SET_3_3V_BUS_VOLTAGE | SFR10_SD_BUS_POWER,  SOCLE_SDHC_SRS10(host->slot));
	host->alive_flag = 1;
	return 0;
}

static u8 
socle_sdhc_dma_handle(struct socle_sdhc_host *host, u32 Status)
{
	u8 DMAMode = host->dma_mode;

	if(!host->cmd->data) {
//		printf("no data for dma handle \n");
		return -1;
	}

	if (Status & SFR12_DMA_INTERRUPT) {    
		if (DMAMode == SDMA_MODE) {    
			printf("DMA interrupt\n");
			if(host->blk_idx == host->cmd->data->blocks)
				printf("DMA_HandleInterrupt : pBufferPos pointer points to NULL\n");
				return -1; 
			}
			// clear DMA status
			socle_sdhc_write(SFR12_DMA_INTERRUPT, SOCLE_SDHC_SRS12(host->slot));
			printf("for next DMA tx \n");
			socle_sdhc_write((u32) host->buf + (512*host->blk_idx), SOCLE_SDHC_SRS0(host->slot));
			return 0; 


		if ((DMAMode == ADMA1_MODE) || (DMAMode == ADMA2_MODE)) {
			printf("SDIO_ERR_DMA_UNEXCEPTED_INTERRUPT \n");
			return -1;
		}
	}
    
	if ((Status & SFR12_TRANSFER_COMPLETE) || (Status & SFR12_ERROR_INTERRUPT)
		|| (Status & SFR12_CARD_REMOVAL) || (Status & SFR12_CARD_INSERTION)) {
		if ((DMAMode == ADMA1_MODE) || (DMAMode == ADMA2_MODE)){
			
//			printf("DMA mode = %x \n", DMAMode);
			
//			printf("DMA_FreeDecsriptors FIX ME\n");
//			DMA_FreeDecsriptors(pRequest); 
		}
	}

    	return 0;
} 
//------------------------------------------------------------------------------------------

static void 
socle_sdhc_isr(void *data)
{
	u32	Status;
	struct socle_sdhc_host *host = (struct socle_sdhc_host *)data;

	if((host->slot+1) != (socle_sdhc_read(SOCLE_SDHC_HRS63)&0xf)  )
		printf("slot interrupt error !!\n");

	
	Status = socle_sdhc_read(SOCLE_SDHC_SRS12(host->slot));

//	printf("STAT : %x \n", Status);
	
	// check the source of the interrupt
	if ( Status & SFR12_ERROR_INTERRUPT ){
//		printf("ISR : SFR12_ERROR_INTERRUPT \n");
		if(host->dma_mode != NONEDMA_MODE)
			socle_sdhc_dma_handle(host, Status);

		socle_sdhc_check_err(host);
		
	}

	if ( Status & SFR12_COMMAND_COMPLETE ){      
		PDEBUG("ISR : SFR12_COMMAND_COMPLETE \n");
		// clear command complete status
		socle_sdhc_write(SFR12_COMMAND_COMPLETE, SOCLE_SDHC_SRS12(host->slot));
		// get response data
		socle_sdhc_get_resp(host);
		host->cmd->cmd_done = 1;
		// if request doesn't need to transfer data
		// the transaction is finish


	}

	if ( Status & SFR12_BUFFER_READ_READY ){
		PDEBUG("ISR : Buffer read ready interrupt\n");
		// clear data read flag
		socle_sdhc_write(SFR12_BUFFER_READ_READY, SOCLE_SDHC_SRS12(host->slot));

		if ((host->cmd->data == NULL) || (host->cmd->data->blocks == host->blk_idx) ||(host->dma_mode != NONEDMA_MODE)) {
			printf("ISR : pBufferPos pointer points to NULL %x, %x ,data : %x \n", host->cmd->data->blocks, host->blk_idx, host->cmd->data);
			return;
		}

 		// read buffer is ready and transmission direction is read
		// transfer data from SDIO card
		if (host->cmd->data->flags == MMC_DATA_READ)
			socle_sdhc_host_trans_datablock(host, host->cmd->data);
		else
			printf("1 ERROR !!\n");

	}

	if ( Status & SFR12_BUFFER_WRITE_READY ) {
		PDEBUG("ISR : Buffer write ready interrupt\n");
		if ((host->cmd->data == NULL) || (host->cmd->data->blocks == host->blk_idx) ||(host->dma_mode != NONEDMA_MODE)) {
			printf("ISR : pBufferPos pointer points to NULL\n");
			return;
		}
		// clear data write flag
		socle_sdhc_write(SFR12_BUFFER_WRITE_READY, SOCLE_SDHC_SRS12(host->slot));


		// write buffer is ready and transmission direction is write
		// transfer data to SDIO card
		if (host->cmd->data->flags == MMC_DATA_WRITE)
			socle_sdhc_host_trans_datablock(host, host->cmd->data);
		else
			printf("2 ERROR !!\n");
		
	}

	if (Status & SFR12_TRANSFER_COMPLETE) {
		PDEBUG("ISR : Transfer complete interrupt\n");
		// clear transfer complete status
		socle_sdhc_write(SFR12_TRANSFER_COMPLETE, SOCLE_SDHC_SRS12(host->slot));
		
		if(!host->cmd->data) {
//			printf("No data transfer\n");
			return;
		}
		if(host->dma_mode != NONEDMA_MODE)
			socle_sdhc_dma_handle(host,Status);

		host->tc = 0; 
		
		if ( Status & SFR12_BLOCK_GAP_EVENT ) {
			printf("SFR12_BLOCK_GAP_EVENT ???? \n");
//			if ( pSlot->AbortRequest ){
//				pSlot->AbortRequest = 0;
//			}
			// continue request
			socle_sdhc_write(socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) |SFR10_CONTINUE_REQUEST, SOCLE_SDHC_SRS10(host->slot));
		}
				
		//pSlot->CurrentRequest = NULL;

	}

	if ( Status & SFR12_DMA_INTERRUPT ){         
		printf("ISR : SFR12_DMA_INTERRUPT =================\n");
		if(host->dma_mode != NONEDMA_MODE)
			socle_sdhc_dma_handle(host, Status);
		else
			printf("EERRRR !! \n");
	}

	if ( Status & SFR12_CARD_INTERRUPT ){      
		printf("ISR : SFR12_CARD_INTERRUPT FIXME \n");
		// clear card interrupt interrupt
		socle_sdhc_write(SFR12_CARD_INTERRUPT, SOCLE_SDHC_SRS12(host->slot));
	}

	if (Status & SFR12_CARD_REMOVAL) {
		printf("ISR : SFR12_CARD_REMOVAL \n");		
		socle_sdhc_write(SFR12_CARD_REMOVAL, SOCLE_SDHC_SRS12(host->slot));
		while(socle_sdhc_read(SOCLE_SDHC_SRS12(host->slot)) & SFR12_CARD_REMOVAL); 
		socle_sdhc_device_deattach(host);
	}
	
	if(Status & SFR12_CARD_INSERTION) {
		printf("ISR : SFR12_CARD_INSERTION \n");				
		socle_sdhc_write(SFR12_CARD_INSERTION,SOCLE_SDHC_SRS12(host->slot));
		while(socle_sdhc_read(SOCLE_SDHC_SRS12(host->slot)) & SFR12_CARD_INSERTION);
		socle_sdhc_device_attach(host);
	}

}

//------------------------------------------------------------------------------------------
static u32 
socle_sdhc_adma_descriptor(struct socle_sdhc_host *host, struct mmc_data *data)
{
	u8 i;

	if(host->dma_mode == ADMA1_MODE) {
		printf("ADMA1 Mode set desc \n");
		if (data->blksz % 4096) {
			printf("ERROR \n");
			return -1;
		}

		// first descriptor sets the page size
		socle_sdhc_adma1[0] = ADMA1_DESCRIPTOR_TYPE_SET 
					| ADMA1_DESCRIPTOR_ADDRESS_LENGHT(data->blksz)
					| ADMA1_DESCRIPTOR_VAL;

		// next descriptors send prepared data
		for (i = 0; i < data->blocks; i++) {
			socle_sdhc_adma1[i] = ADMA1_DESCRIPTOR_TYPE_TRAN
			| ADMA1_DESCRIPTOR_ADDRESS_LENGHT((u32) (data->sg->dma_address + (i*data->blksz)))
			| ADMA1_DESCRIPTOR_VAL;

		}
		// last descriptor finishes transmission 
		socle_sdhc_adma1[i] |= ADMA1_DESCRIPTOR_END;

	}  

	if(host->dma_mode == ADMA2_MODE){
//		printf("ADMA2 Mode set desc \n");
#if 0
		for (i = 0; i < data->blocks ; i ++){
			socle_sdhc_adma2[i].ctrl = ADMA2_DESCRIPTOR_TYPE_TRAN
			| ADMA2_DESCRIPTOR_LENGHT(data->blksz)
			| ADMA2_DESCRIPTOR_VAL;
//			printf("socle_sdhc_adma2[%d].ctrl = %x \n",i ,socle_sdhc_adma2[i].ctrl);
			socle_sdhc_adma2[i].address = data->sg->dma_address + (i*data->blksz);
//			printf("socle_sdhc_adma2[%d].address = %x \n",i ,socle_sdhc_adma2[i].address);
		}
		// last descriptor finishes transmission 
		socle_sdhc_adma2[i].ctrl = ADMA2_DESCRIPTOR_END | ADMA2_DESCRIPTOR_VAL;
//		printf("socle_sdhc_adma2[%d].ctrl = %x \n",i ,socle_sdhc_adma2[i].ctrl);
#else
		socle_sdhc_adma2[0].ctrl = ADMA2_DESCRIPTOR_TYPE_TRAN
			| ADMA2_DESCRIPTOR_LENGHT(data->blksz * data->blocks)
			| ADMA2_DESCRIPTOR_VAL;
		socle_sdhc_adma2[0].address = data->sg->dma_address;


		// last descriptor finishes transmission 
		socle_sdhc_adma2[1].ctrl = ADMA2_DESCRIPTOR_END | ADMA2_DESCRIPTOR_VAL;

#endif
	}
	
	return 0;
}
//------------------------------------------------------------------------------------------


static u8 
socle_sdhc_dma_prepare(struct socle_sdhc_host *host, struct mmc_data *data)
{

//	printf("set DMA address = %x \n",data->sg->dma_address);
	switch(host->dma_mode) {
		case SDMA_MODE:
			// set system address register
			socle_sdhc_write(data->sg->dma_address, SOCLE_SDHC_SRS0(host->slot));
			socle_sdhc_write((socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & ~SFR10_DMA_SELECT_MASK) 
				|SFR10_DMA_SELECT_SDMA, SOCLE_SDHC_SRS10(host->slot));
//			printf("DMA_PrepareTransfer : SDMA mode selected\n");        
		break;

		case ADMA1_MODE:
		case ADMA2_MODE:        
			// create descriptor table
			if(socle_sdhc_adma_descriptor(host, data))
				return -1;

			// set register with the descriptors table base address
			if(host->dma_mode == ADMA1_MODE) {
				socle_sdhc_write((u32) socle_sdhc_adma1,SOCLE_SDHC_SRS22(host->slot)); 
				socle_sdhc_write((socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & (~SFR10_DMA_SELECT_MASK)) |SFR10_DMA_SELECT_ADMA1
					,SOCLE_SDHC_SRS10(host->slot));
				
			} else {
				socle_sdhc_write((u32) socle_sdhc_adma2,SOCLE_SDHC_SRS22(host->slot));
				socle_sdhc_write((socle_sdhc_read(SOCLE_SDHC_SRS10(host->slot)) & (~SFR10_DMA_SELECT_MASK))| SFR10_DMA_SELECT_ADMA2
					,SOCLE_SDHC_SRS10(host->slot));
			}
		break;

		default:
		return -1;
	}

	return 0;
} 
//------------------------------------------------------------------------------------------

static u8
socle_sdhc_send_command(struct socle_sdhc_host *host, struct mmc_command *cmd)
{
	struct mmc_data *data;
	u32 	command_information;
	u8 	BusyCheck = 0;
	int no_resp = 0;
	u8 	Status = Status;
	u32 	timeout_cont = 0;
	int i;
//printf("@@--%s--cmd = %d\n", __func__, cmd->opcode);
	host->cmd = cmd;
	cmd->error = 0;
	cmd->cmd_done = 0;
	host->tc = 0;

	for (i=0;i<4;i++)
		cmd->resp[i] = 0;
	
	host->err_flag = 0;

	// check response type
	switch(cmd->flags) {
	case MMC_RSP_NONE:
		no_resp = 1;
		command_information = SFR3_NO_RESPONSE;
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R5:
	case MMC_RSP_R6:
	case MMC_RSP_R7:
		command_information = SFR3_RESP_LENGTH_48 | SFR3_CRC_CHECK_EN | SFR3_INDEX_CHECK_EN;
		break;
	case MMC_RSP_R1B:
//	case MMC_RSP_R5B:
		command_information = SFR3_RESP_LENGTH_48B | SFR3_CRC_CHECK_EN| SFR3_INDEX_CHECK_EN;
		BusyCheck = 1;
		break;
	case MMC_RSP_R2:
		command_information = SFR3_RESP_LENGTH_136 | SFR3_CRC_CHECK_EN;
		break;
	case MMC_RSP_R3:
	case MMC_RSP_R4:	
		command_information = SFR3_RESP_LENGTH_48;
		break;
	}

	// check if command line is not busy
	timeout_cont = COMMANDS_TIMEOUT;

//cyli fix
#if 1
        while((socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot)) & 0x307) && (!((socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot))>>20) & 0x1))) {
{
	u32 temp;
	temp = socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot));
printf("temp = 0x%x\n", temp);
       }
		timeout_cont--;
                if(timeout_cont == 0) {
                        cmd->error = MMC_ERR_INVALID;
                        printf("Command line is busy can't execute command !! \n ");
                        return -1;
                }

        }

#else

#if 0
                while(!((socle_sdhc_read(SOCLE_SDHC_SRS9(0))>>20) & 0x1)) {
                        timeout_cont--;
                        if(timeout_cont == 0) {
                                cmd->error = MMC_ERR_INVALID;
                                printf("DAT line is busy can't execute command debug : %x \n", (socle_sdhc_read(SOCLE_SDHC_SRS9(0))>>20) & 0xf);
                                        return -1;
                        }
                }
#else

	while(socle_sdhc_read(SOCLE_SDHC_SRS9(0)) & SFR9_CMD_INHIBIT_CMD) {
		timeout_cont--;
		if(timeout_cont == 0) {
			cmd->error = MMC_ERR_INVALID;
			printf("Command line is busy can't execute command !! \n ");
			return -1;
		}
			
	}
#endif
	// check if data line is not busy
	if ( ( cmd->data) || 
		( BusyCheck && ( cmd->opcode != MMC_STOP_TRANSMISSION ) && ( cmd->opcode != SD_IO_RW_DIRECT ) ) ){

		timeout_cont = COMMANDS_TIMEOUT;

#if 0
                while(!((socle_sdhc_read(SOCLE_SDHC_SRS9(0))>>20) & 0x1)) {
                        timeout_cont--;
                        if(timeout_cont == 0) {
                                cmd->error = MMC_ERR_INVALID;
                                printf("DAT line is busy can't execute command debug : %x \n", (socle_sdhc_read(SOCLE_SDHC_SRS9(0))>>20) & 0xf);
                                        return -1;
                        }
                }
#else

		while(socle_sdhc_read(SOCLE_SDHC_SRS9(0)) & ( SFR9_CMD_INHIBIT_CMD | SFR9_CMD_INHIBIT_DAT )) {
			timeout_cont--;
			if(timeout_cont == 0) {
				cmd->error = MMC_ERR_INVALID;
				printf("DAT line is busy can't execute command \n");
				return -1;
			}
		}
#endif
	}

#endif

	// clear all status interrupts except:
	// current limit error, card interrupt, card removal, card insertion
//	*pSlot->Registers.SFR12 = CPUToLE32( ~( SFR12_CURRENT_LIMIT_ERROR 
//	| SFR12_CARD_INTERRUPT 
//	| SFR12_CARD_REMOVAL 
//	| SFR12_CARD_INSERTION ) );
	data = cmd->data;
	if(data)
	{
		host->tc = 1;

		if(host->dma_mode!= NONEDMA_MODE) {
//			printf("DATA send with DMA \n");

			if(socle_sdhc_dma_prepare(host, cmd->data)) {
				printf("socle_sdhc_dma_prepare 3 ERROR !!\n");
				return -1;
			}

			command_information |= SFR3_DMA_ENABLE;
		}

		// set block size and block
//		printf("block count %d, blksz = %d \n", data->blocks, data->blksz);
//		printf("SRS 1 : %x \n ", (data->blocks <<16) |data->blksz | SFR1_DMA_BUFF_SIZE_512KB);
		socle_sdhc_write((data->blocks <<16) |data->blksz | SFR1_DMA_BUFF_SIZE_512KB,
					SOCLE_SDHC_SRS1(host->slot));

		// set data preset bit
		command_information |= SFR3_DATA_PRESENT;

		if ( data->blocks > 1 ) {
			command_information |= SFR3_MULTI_BLOCK_SEL | SFR3_BLOCK_COUNT_ENABLE;
				if (host->AutoCMD12Enable ) {
//					printf("AutoCMD12Enable ====\n");
					command_information |= SFR3_AUTOCMD12_ENABLE;   
				}

		}

		if (data->flags & MMC_DATA_READ) 
			command_information |= SFR3_TRANS_DIRECT_READ;


		host->cur_sg_idx = 0;
		host->blk_idx = 0;
		host->buf_idx = 0;
		host->buf = (u32 *) data->sg->dma_address;
//		printf("data address : %x \n",data->sg->dma_address);


	}


	//write argument
//	printf("SRS2 : CMD op = %d , CMD arg = %x \n",cmd->opcode, cmd->arg);
	socle_sdhc_write(cmd->arg, SOCLE_SDHC_SRS2(host->slot));

	if(cmd->opcode == MMC_STOP_TRANSMISSION)
		command_information |= SDIOHOST_CMD_TYPE_ABORT << 22 ;
	else
		command_information |= SDIOHOST_CMD_TYPE_OTHER << 22 ;

	command_information |= cmd->opcode <<24 ;
	
//	printf("SRS3 : %x \n",command_information);
	// execute command
	socle_sdhc_write(command_information, SOCLE_SDHC_SRS3(host->slot));

	while(!cmd->cmd_done);


	if(cmd->data)
		while(host->tc);
		
		
	if(host->err_flag) {
		//printf("host->err_flag = %x, op = %x \n ",host->err_flag, host->cmd->opcode);
		return -1;
	}
	if(cmd->error) {
		printf("cmd->error = %x \n ",host->err_flag);
		return -1;
	}
	return 0;



}

static u8
socle_sdhc_card_detect(struct socle_sdhc_host *host)
{
	if(socle_sdhc_read(SOCLE_SDHC_SRS9(host->slot)) & SFR9_CARD_INSERTED) {
		host->alive_flag = 1;
		return 1;
	} else {
		host->alive_flag = 0;
		return 0;
	}
}

