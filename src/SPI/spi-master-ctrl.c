#include <test_item.h>
#include <global.h>
#include <dma/dma.h>
#include "spi-master.h"
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif


/*function*/
static void sq_spi_tx_dma_page_interrupt(void *data);
static void sq_spi_rx_dma_page_interrupt(void *data);
static int sq_spi_master_internal_hdma_run(int autotest);

/*variable*/


static volatile int sq_spi_tx_complete_flag = 0;
static volatile int sq_spi_rx_complete_flag = 0;
static u32 sq_spi_tx_dma_ch_num, sq_spi_rx_dma_ch_num;
static u32 sq_spi_tx_dma_ext_hdreq, sq_spi_rx_dma_ext_hdreq;
static u32 sq_spi_base,sq_spi_dma_burst,char_len,lsb;
static u8 cpha,cpol,pure,regflag;
static u8 *sq_spi_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *sq_spi_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

static struct sq_dma_notifier sq_spi_tx_dma_notifier = {
	.page_interrupt = sq_spi_tx_dma_page_interrupt,
};

static struct sq_dma_notifier sq_spi_rx_dma_notifier = {
	.page_interrupt = sq_spi_rx_dma_page_interrupt,
};

extern struct test_item_container sq_spi_test_container;

extern int
spi_test(int autotest)
{
	
	int ret = 0;
	ret = test_item_ctrl(&sq_spi_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_master_test_container;

extern int
sq_spi_master_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&sq_spi_master_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_transfer_test_container;

extern int sq_spi_master0_test(int autotest)
{
	int ret = 0;
	
	{
		extern struct test_item sq_spi_transfer_test_items[];
		
		sq_spi_transfer_test_items[1].enable = SQ_SPI_EEPROM_0_TEST;
/* added by morganlin */
#ifndef GDR_SPI		
                sq_spi_transfer_test_items[2].enable = SQ_SPI_MARVEL_WIFI_0_TEST;
#endif
                sq_spi_transfer_test_items[3].enable = SQ_SPI_TSC2000_0_TEST;
/* added by morganlin */
#ifndef GDR_SPI 
                sq_spi_transfer_test_items[4].enable = SQ_SPI_ADS7846_0_TEST;
#endif		
                sq_spi_transfer_test_items[5].enable = SQ_SPI_MAX1110_0_TEST;
	}	
	sq_spi_base = SQ_SPI0;
#if defined(CONFIG_PC9223)
		sq_scu_dev_enable(SQ_DEVCON_SPI0);
		sq_scu_hdma_req45_spi0();
#endif
	sq_spi_master_init(sq_spi_base,SQ_INTC_SPI0);
		
	ret = test_item_ctrl(&sq_spi_transfer_test_container, autotest);
#if defined(CONFIG_PC9223)
	sq_scu_dev_disable(SQ_DEVCON_SPI0);
#endif
	sq_spi_master_free();
	return ret;
}

extern int sq_spi_master1_test(int autotest)
{
	int ret = 0;
	{
		extern struct test_item sq_spi_transfer_test_items[];
		
		sq_spi_transfer_test_items[1].enable = SQ_SPI_EEPROM_1_TEST;
		sq_spi_transfer_test_items[2].enable = SQ_SPI_MARVEL_WIFI_1_TEST;
		sq_spi_transfer_test_items[3].enable = SQ_SPI_TSC2000_1_TEST;
/* added by morganlin */
#ifndef GDR_SPI 
                sq_spi_transfer_test_items[4].enable = SQ_SPI_ADS7846_1_TEST;
#endif
                sq_spi_transfer_test_items[5].enable = SQ_SPI_MAX1110_1_TEST;
	}
	sq_spi_base = SQ_SPI1;
#if defined(CONFIG_PC9223)
		sq_scu_dev_enable(SQ_DEVCON_SPI1);
		sq_scu_hdma_req45_spi1();
#endif
	sq_spi_master_init(sq_spi_base,SQ_INTC_SPI1);

	ret = test_item_ctrl(&sq_spi_transfer_test_container, autotest);
#if defined(CONFIG_PC9223)
	sq_scu_dev_disable(SQ_DEVCON_SPI1);
#endif
	sq_spi_master_free();	
	return ret;
}

extern struct test_item_container sq_spi_internal_test_container;

extern int 
sq_spi_internal_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&sq_spi_internal_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_hdma_burst_type_test_container;

extern int
sq_spi_internal_hdma_test(int autotest)
{
	int ret = 0;	

	sq_spi_tx_dma_ext_hdreq = 5;
	sq_spi_rx_dma_ext_hdreq = 4;

	sq_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	sq_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	
	sq_request_dma(sq_spi_tx_dma_ch_num, &sq_spi_tx_dma_notifier);
	sq_request_dma(sq_spi_rx_dma_ch_num, &sq_spi_rx_dma_notifier);
	ret = test_item_ctrl(&sq_spi_hdma_burst_type_test_container, autotest);
	sq_disable_dma(sq_spi_tx_dma_ch_num);
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	sq_free_dma(sq_spi_tx_dma_ch_num);
	sq_free_dma(sq_spi_rx_dma_ch_num);
	return ret;
}

extern int
sq_spi_hdma_burst_single_test(int autotest)
{
	sq_spi_dma_burst = SQ_DMA_BURST_SINGLE;
	return sq_spi_master_internal_hdma_run(autotest);
}

extern int
sq_spi_hdma_burst_incr4_test(int autotest)
{
	sq_spi_dma_burst = SQ_DMA_BURST_INCR4;
	return sq_spi_master_internal_hdma_run(autotest);
}

extern struct test_item_container sq_spi_master_slave_model_test_container;

extern int
sq_spi_sq_slave_test(int autotest)
{
	int ret = 0;
  ret = test_item_ctrl(&sq_spi_master_slave_model_test_container, autotest);
	return ret;
}


extern struct test_item_container sq_spi_master_slave_mode_test_container;

extern int
sq_spi_master_slave_protocol_test(int autotest)
{
  int ret = 0;
  pure=0;
  ret = test_item_ctrl(&sq_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_slave_pure_test(int autotest)
{
  int ret = 0;
  pure=1;
  regflag=0;
  ret = test_item_ctrl(&sq_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_slave_reg_test(int autotest)
{
	int ret = 0;
	pure=1;
  regflag=1;
  ret = test_item_ctrl(&sq_spi_master_slave_mode_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_slave_type_test_container;
extern struct test_item_container sq_spi_master_sb_test_container;

extern int
sq_spi_master_slave_mode0_test(int autotest)
{
	int ret=0;
	cpha=SQ_SPI_CPHA_0;
	cpol=SQ_SPI_CPOL_0;
	if(pure)
		ret = test_item_ctrl(&sq_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&sq_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_slave_mode1_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_0;
	cpol=SQ_SPI_CPOL_1;
	if(pure)
		ret = test_item_ctrl(&sq_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&sq_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_slave_mode2_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_1;
	cpol=SQ_SPI_CPOL_0;
	if(pure)
		ret = test_item_ctrl(&sq_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&sq_spi_slave_type_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_slave_mode3_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_1;
	cpol=SQ_SPI_CPOL_1;
	if(pure)
		ret = test_item_ctrl(&sq_spi_master_sb_test_container, autotest);
	else
		ret = test_item_ctrl(&sq_spi_slave_type_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_master_ch_test_container;

extern int
sq_spi_master_lsb_test(int autotest)
{
	int ret = 0;
	lsb=SQ_SPI_TX_LSB_FIRST;
	ret = test_item_ctrl(&sq_spi_master_ch_test_container, autotest);
	return ret;
}

extern int
sq_spi_master_msb_test(int autotest)
{
	int ret = 0;
	lsb=SQ_SPI_TX_MSB_FIRST;
	ret = test_item_ctrl(&sq_spi_master_ch_test_container, autotest);
	return ret;
}

extern int sq_spi_slave_run(int type);
extern int sq_spi_slave_pure_run(void);
extern int sq_spi_slave_reg_run(void);

extern int
sq_spi_master_ch8_test(int autotest)
{
	int ret = 0;
	char_len=SQ_SPI_CHAR_LEN_8;
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);
	
		/* Configure SPI controller */
	sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |	
		SQ_SPI_MODE_MASTER |
#endif
    SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		lsb |
		SQ_SPI_OP_NORMAL,
		SQ_SPI_FWCR);
		
	sq_spi_write(
		char_len |
		SQ_SPI_SLAVE_SEL_0 |
		SQ_SPI_CLK_DIV(SLAVE_DIV),
		SQ_SPI_SSCR);
	
	if(regflag)
		ret = sq_spi_slave_reg_run();
	else
		ret = sq_spi_slave_pure_run();
	return ret;
}
extern int
sq_spi_master_ch16_test(int autotest)
{
	int ret = 0;
	char_len=SQ_SPI_CHAR_LEN_16;
	sq_spi_set_current_mode(MODE_CHAR_LEN_16);
	
		/* Configure SPI controller */
	sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |	
		SQ_SPI_MODE_MASTER |
#endif
    SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		lsb |
		SQ_SPI_OP_NORMAL,
		SQ_SPI_FWCR);
		
	sq_spi_write(
		char_len |
		SQ_SPI_SLAVE_SEL_0 |
		SQ_SPI_CLK_DIV(SLAVE_DIV),
		SQ_SPI_SSCR);
	
	if(regflag)
		ret = sq_spi_slave_reg_run();
	else
		ret = sq_spi_slave_pure_run();
	return ret;
}

extern int
sq_spi_slave_normal_test(int autotest)
{
	int type=SLAVE_NORMAL;
	
	sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |		
		SQ_SPI_MODE_MASTER |
#endif
		SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SQ_SPI_TX_MSB_FIRST |
		SQ_SPI_OP_NORMAL,
		SQ_SPI_FWCR);
		
	return sq_spi_slave_run(type);
}

extern int 
sq_spi_slave_hdma_test(int autotest)
{
	int type=SLAVE_HDMA;
	
		sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |		
		SQ_SPI_MODE_MASTER |
#endif
		SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SQ_SPI_TX_MSB_FIRST |
		SQ_SPI_OP_NORMAL,
		SQ_SPI_FWCR);
		
	return sq_spi_slave_run(type);
}

extern int sq_spi_slave_reset_test(int autotest)
{
	int type=SLAVE_RESET;
	
		sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |		
		SQ_SPI_MODE_MASTER |
#endif
		SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		cpol |
		cpha |
		SQ_SPI_TX_MSB_FIRST |
		SQ_SPI_OP_NORMAL,
		SQ_SPI_FWCR);
		
	return sq_spi_slave_run(type);
}



extern int
sq_spi_internal_normal_run(int autotest)
{
    /* Reset SPI controller */
  sq_spi_write(
  	sq_spi_read(SQ_SPI_FWCR) |
    SQ_SPI_SOFT_RST, 
    SQ_SPI_FWCR);
    
    /* Configure SPI controller */
	sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |		
		SQ_SPI_MODE_MASTER |
#endif
		SQ_SPI_SOFT_N_RST |
		SQ_SPI_TXRX_N_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_DIS |
		SQ_SPI_CPOL_0 |
		SQ_SPI_CPHA_0 |
		SQ_SPI_TX_LSB_FIRST |
		SQ_SPI_OP_LOOPBACK,
		SQ_SPI_FWCR);
			
	/* Enable SPI interrupt */
	sq_spi_write(
		//SQ_SPI_IER_RXFIFO_OVR_INT_EN |
    //SQ_SPI_IER_TXFIFO_INT_EN |
		SQ_SPI_IER_RXFIFO_INT_EN |
		SQ_SPI_IER_RX_COMPLETE_INT_EN,
		SQ_SPI_IER);

	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
		SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_RXFIFO_CLR |
		SQ_SPI_TXFIFO_CLR,
		SQ_SPI_FCR);
     
	/* Set the SPI slaves select and characteristic control register */
	sq_spi_write(
		SQ_SPI_CHAR_LEN_8 |
		SQ_SPI_SLAVE_SEL_2 |
		SQ_SPI_CLK_DIV(LOOKBACK_DIV),
		SQ_SPI_SSCR);

	/* Config SPI clock delay */
	sq_spi_write(
		SQ_SPI_PBTXRX_DELAY_NONE |
		SQ_SPI_PBCT_DELAY_NONE |
		SQ_SPI_PBCA_DELAY_1_2,
		SQ_SPI_DLYCR);


	/* Set per char length */
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);
	/* Clear pattern buffer and compare buffer */
	memset((char *)sq_spi_pattern_buf, 0x0, PATTERN_BUF_SIZE);
	memset((char *)sq_spi_cmpr_buf, 0x0, PATTERN_BUF_SIZE);

	/* Make test pattern */
	sq_spi_make_test_pattern(sq_spi_pattern_buf, PATTERN_BUF_SIZE);

	if (sq_spi_transfer(sq_spi_pattern_buf, sq_spi_cmpr_buf, SET_TX_RX_LEN(PATTERN_BUF_SIZE, PATTERN_BUF_SIZE)))
		return -1;
	return sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, PATTERN_BUF_SIZE, autotest);

}

static int
sq_spi_master_internal_hdma_run(int autotest)
{	
	sq_spi_tx_complete_flag = 0;
	sq_spi_rx_complete_flag = 0; 
	
	/* Set per char length */
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);
	
	memset((char *)sq_spi_pattern_buf, 0x0, PATTERN_BUF_SIZE);
	memset((char *)sq_spi_cmpr_buf, 0x0, PATTERN_BUF_SIZE);
	sq_spi_make_test_pattern(sq_spi_pattern_buf, PATTERN_BUF_SIZE);
	
	sq_spi_write(
    sq_spi_read(SQ_SPI_FWCR) |
		SQ_SPI_SOFT_RST, 
    SQ_SPI_FWCR);
		
	sq_spi_write(
		SQ_SPI_IER_TXFIFO_INT_DIS |
		SQ_SPI_IER_RXFIFO_INT_DIS |
		//SQ_SPI_IER_RXFIFO_OVR_INT_EN |
		SQ_SPI_IER_RX_COMPLETE_INT_DIS,
		SQ_SPI_IER);
	
	sq_spi_write(
		SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_RXFIFO_CLR |
		SQ_SPI_TXFIFO_CLR,
		SQ_SPI_FCR);
			
	sq_spi_write(
		SQ_SPI_PBTXRX_DELAY_NONE |
		SQ_SPI_PBCT_DELAY_NONE |
		SQ_SPI_PBCA_DELAY_256,
		SQ_SPI_DLYCR);
	
	sq_spi_write(
		SQ_SPI_CHAR_LEN_8 |
		SQ_SPI_SLAVE_SEL_2 |
		SQ_SPI_CLK_DIV(LOOKBACK_DIV),
		SQ_SPI_SSCR);

	sq_spi_write(PATTERN_BUF_SIZE, SQ_SPI_TXCR);
	sq_spi_write(PATTERN_BUF_SIZE, SQ_SPI_RXCR);

	sq_disable_dma(sq_spi_tx_dma_ch_num);
	sq_set_dma_mode(sq_spi_tx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_tx_dma_ch_num, sq_spi_tx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_tx_dma_ch_num, sq_spi_dma_burst);
	sq_set_dma_source_address(sq_spi_tx_dma_ch_num, (u32)sq_spi_pattern_buf);
	sq_set_dma_destination_address(sq_spi_tx_dma_ch_num, SQ_SPI_TXR+sq_spi_base);
	sq_set_dma_source_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_destination_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_data_size(sq_spi_tx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_transfer_count(sq_spi_tx_dma_ch_num, PATTERN_BUF_SIZE);
	sq_set_dma_slice_count(sq_spi_tx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_tx_dma_ch_num, 1);
	sq_set_dma_buffer_size(sq_spi_tx_dma_ch_num, PATTERN_BUF_SIZE);
		
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	sq_set_dma_mode(sq_spi_rx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_rx_dma_ch_num, sq_spi_rx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_rx_dma_ch_num, sq_spi_dma_burst);
	sq_set_dma_source_address(sq_spi_rx_dma_ch_num, SQ_SPI_RXR+sq_spi_base);
	sq_set_dma_destination_address(sq_spi_rx_dma_ch_num, (u32)sq_spi_cmpr_buf);
	sq_set_dma_source_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_destination_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_data_size(sq_spi_rx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_transfer_count(sq_spi_rx_dma_ch_num, PATTERN_BUF_SIZE);
	sq_set_dma_slice_count(sq_spi_rx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_rx_dma_ch_num, 1);
	sq_set_dma_buffer_size(sq_spi_rx_dma_ch_num, PATTERN_BUF_SIZE);

	sq_enable_dma(sq_spi_tx_dma_ch_num);
	sq_enable_dma(sq_spi_rx_dma_ch_num);
			
	sq_spi_write(
#if defined (CONFIG_PC9223)
		SQ_SPI_MASTER_SIGNAL_CTL_HW |
		SQ_SPI_MASTER_SIGNAL_ACT_NO |		
		SQ_SPI_MODE_MASTER |
#endif
		SQ_SPI_EN |
		SQ_SPI_TXRX_RUN |
		SQ_SPI_CLK_IDLE_AST |
		SQ_SPI_TXRX_SIMULT_EN |
		SQ_SPI_CPOL_0 |
		SQ_SPI_CPHA_0 |
		SQ_SPI_TX_MSB_FIRST |
		SQ_SPI_DMA_REQ |
		SQ_SPI_OP_LOOPBACK,			
		SQ_SPI_FWCR);

	if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30))
		printf("Timeout\n");
	
	return sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, PATTERN_BUF_SIZE, autotest);
}

static void 
sq_spi_tx_dma_page_interrupt(void *data)
{
	sq_spi_tx_complete_flag = 1;
}

static void 
sq_spi_rx_dma_page_interrupt(void *data)
{
	sq_spi_rx_complete_flag = 1;
}



