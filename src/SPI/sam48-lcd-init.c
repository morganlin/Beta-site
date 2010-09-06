#include "spi-master.h"

static char init_array[]={
	0x00,0x63,
	0x01,0x55,
	0x02,0x01,
	0x03,0x0c,
	0x04,0x00,
	0x05,0x11,
	0x06,0x00,
	0x07,0x00,
	0x08,0x44,
	0x09,0x01,
	0x0a,0x30,
	0x0b,0x1a,
	0x0c,0x29,
	0x0d,0x3f,
	0x0e,0x20,
	0x0f,0x20,
	0x10,0xfa,
	0x11,0xfa,
	0x12,0x0a,
	0x13,0x20,
	0x14,0x20,
	0x15,0x20,
	0x16,0x80,
	0x17,0x80,
	0x18,0x80,
	0x20,0x00,
	0x21,0x0c,
	0x22,0xd9,
	0x23,0x19,
	0x24,0x56,
	0x25,0x88,
	0x26,0xb9,
	0x27,0x4b,
	0x28,0xbe,
	0x29,0x21,
	0x2a,0x77,
	0x2b,0xff,
	0x2c,0x40,
	0x2d,0x95,
	0x2e,0xfe,
	0x50,0x00,
	0x51,0x0c,
	0x52,0xd9,
	0x53,0x19,
	0x54,0x56,
	0x55,0x88,
	0x56,0xb9,
	0x57,0x4b,
	0x58,0xbe,
	0x59,0x21,
	0x5a,0x77,
	0x5b,0xff,
	0x5c,0x40,
	0x5d,0x95,
	0x5e,0xfe,
	0x2f,0x21
};



extern int sam48_lcd_init(void)
{
	u8 divisor,i,count; 
	u8 tx_buf[2];
	
	sq_spi_master_init(SQ_SPI0,SQ_INTC_SPI0);
	
	/* Reset SPI controller */
	sq_spi_write(
			sq_spi_read(SQ_SPI_FWCR) | 
			SQ_SPI_SOFT_RST,
			SQ_SPI_FWCR);

	/* Configure SPI controller */
		sq_spi_write(
#if defined(CONFIG_PC9223)
			SQ_SPI_MASTER_SIGNAL_CTL_HW |
			SQ_SPI_MASTER_SIGNAL_ACT_NO |		
			SQ_SPI_MODE_MASTER |
#endif
			SQ_SPI_SOFT_N_RST |
			SQ_SPI_TXRX_N_RUN |
			SQ_SPI_CLK_IDLE_AST |
			SQ_SPI_TXRX_SIMULT_DIS |
			SQ_SPI_CPOL_1 |
			SQ_SPI_CPHA_1 |
			SQ_SPI_TX_MSB_FIRST |
			SQ_SPI_OP_NORMAL,
			SQ_SPI_FWCR);
			
	/* Enable SPI interrupt */
/*
	sq_spi_write(
			SQ_SPI_IER_RXFIFO_INT_EN |
			SQ_SPI_IER_RXFIFO_OVR_INT_EN |
			SQ_SPI_IER_RX_COMPLETE_INT_EN,
			SQ_SPI_IER);
*/
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
			SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_RXFIFO_CLR |
			SQ_SPI_TXFIFO_CLR,
			SQ_SPI_FCR);
     
	
	divisor = sq_spi_calculate_divisor(1000000); /* 1.0 MHz clock rate */
	/* Set the SPI slaves select and characteristic control register */
	sq_spi_write(
			SQ_SPI_CHAR_LEN_8 |
			SQ_SPI_SLAVE_SEL_0 |
			SQ_SPI_CLK_DIV(divisor),
			SQ_SPI_SSCR);

	/* Config SPI clock delay */
	sq_spi_write(
			SQ_SPI_PBTXRX_DELAY_NONE |
			SQ_SPI_PBCT_DELAY_NONE |
			SQ_SPI_PBCA_DELAY_1_2,
			SQ_SPI_DLYCR);

	/* Set per char length */
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);
	
	count=sizeof(init_array)/2;
	for(i=0;i<count;i++) {
			tx_buf[0]=init_array[2*i];
			tx_buf[1]=init_array[2*i+1];
			if(sq_spi_transfer(tx_buf, NULL, SET_TX_RX_LEN(2, 0))) {
				printf("EVB LCD init fail!\n");
				return -1;
			}
			if(i==0 || i==1)
				MSDELAY(100);
	}
	return 0;

}
