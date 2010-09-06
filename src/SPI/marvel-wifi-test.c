#include "spi-master.h"
#include "if_gspi.h"

#define MARVEL_MAX_NUM	64
#define MARVEL_CHAR_LEN	16
#define MARVEL_CMD_WR	0x8000

static u8 *sq_spi_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *sq_spi_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

extern int
sq_spi_marvel_wifi(int autotest)
{
	u8 divisor;
	u16 tx_buf[MARVEL_MAX_NUM + 1] = {0};
	u16 rx_buf[1] = {0};
	u32 result = 0, i;
	u32 apb_clk = 0;

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
			SQ_SPI_TX_MSB_FIRST |
			SQ_SPI_OP_NORMAL,
			SQ_SPI_FWCR);
			
	/* Enable SPI interrupt */
	sq_spi_write(
			SQ_SPI_IER_RXFIFO_INT_EN |
			SQ_SPI_IER_RXFIFO_OVR_INT_EN |
			SQ_SPI_IER_RX_COMPLETE_INT_EN,
			SQ_SPI_IER);

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
			SQ_SPI_CHAR_LEN_16 |
			SQ_SPI_SLAVE_SEL_0 |
			SQ_SPI_CLK_DIV(divisor),
			SQ_SPI_SSCR);

	apb_clk = sq_scu_apb_clock_get();

	// if apb_clk <= 25M, the RX will overflow
	if (apb_clk > 25000000) {
		/* Config SPI clock delay */
		sq_spi_write(
				SQ_SPI_PBTXRX_DELAY_NONE |
				SQ_SPI_PBCT_DELAY_64 |
				SQ_SPI_PBCA_DELAY_1_2,
				SQ_SPI_DLYCR);
	} else {
		/* Config SPI clock delay */
		sq_spi_write(
				SQ_SPI_PBTXRX_DELAY_NONE |
				SQ_SPI_PBCT_DELAY_16 |
				SQ_SPI_PBCA_DELAY_1_2,
				SQ_SPI_DLYCR);

	}
	/* Set per char length */
	sq_spi_set_current_mode(MODE_CHAR_LEN_16);

	/* Clear pattern buffer and compare buffer */
	memset((char *)sq_spi_pattern_buf, 0x0, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
	memset((char *)sq_spi_cmpr_buf, 0x0, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);

	/* Set as no delay mode */
	tx_buf[0] = MARVEL_CMD_WR | SPU_BUS_MODE_REG;
	tx_buf[1] = BUS_MODE_16_NO_DELAY;
	if(sq_spi_transfer(tx_buf, NULL, SET_TX_RX_LEN(2, 0)))
		return -1;

	/* Read Chip ID */
	tx_buf[0] = CHIPREV_REG;
	if(sq_spi_transfer(tx_buf, rx_buf, SET_TX_RX_LEN(1, 2)))
		return -1;

	if (0xb != rx_buf[0]) {
		printf("Chip ID [0x%02x] != 0x0b fail!!\n", rx_buf[0]);
		return -1;
	} else {
		printf("Read Chip ID = 0x%02x success\n", rx_buf[0]);
	}

	/* Bulk data R/W test */
	printf("Burn in loop: ");
	for (i = 0; i < 10; i++) {

		/* Write data */
		tx_buf[0] = MARVEL_CMD_WR | DATA_RDWRPORT_REG;

		sq_spi_make_test_pattern(sq_spi_pattern_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
		memcpy((char *)&tx_buf[1], (char *)sq_spi_pattern_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8);
		if(sq_spi_transfer(tx_buf, NULL, SET_TX_RX_LEN(MARVEL_MAX_NUM+1, 0)))
			return -1;

		/* Read data */
		tx_buf[0] = DATA_RDWRPORT_REG;

		if(sq_spi_transfer(tx_buf, sq_spi_cmpr_buf, SET_TX_RX_LEN(1, MARVEL_MAX_NUM)))
			return -1;

		result = sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, MARVEL_MAX_NUM * MARVEL_CHAR_LEN / 8,
						autotest);
		printf("%d ", i);

		if (result)
			break;
	}
	printf("\n");

	if (result)
		printf("Burn in fail[%d]!!\n", i);

	return result;
}
