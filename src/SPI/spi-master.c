#include "spi-master.h"
#include <sq-scu.h>

static volatile int sq_spi_tx_complete_flag = 0;
static volatile int sq_spi_rx_complete_flag = 0;
static void *sq_spi_isr_tx_buf = 0;
static u32 sq_spi_isr_tx_buf_idx = 0;
static void *sq_spi_isr_rx_buf = 0;
static u32 sq_spi_isr_rx_buf_idx = 0;
static u32 sq_spi_isr_total_len = 0;
static int current_mode = MODE_CHAR_LEN_8;
static u32 sq_spi_base,sq_irq_num;
static void sq_spi_master_isr(void *pparam);
static u32 sq_spi_power(u32 base, u32 exp);

extern void
sq_spi_write(u32 val, u32 reg)
{
	iowrite32(val, sq_spi_base+reg);
}

extern u32
sq_spi_read(u32 reg)
{
	u32 val;
	val = ioread32(sq_spi_base+reg);
	return val;
}

extern void
sq_spi_master_init(u32 spi_base, u32 irq_num)
{
	sq_spi_base=spi_base;
	sq_irq_num=irq_num;
#if defined (CONFIG_PC9223)
	sq_spi_write(
		sq_spi_read(SQ_SPI_FWCR) |
		SQ_SPI_MODE_MASTER,
		SQ_SPI_FWCR);
#endif
	request_irq(sq_irq_num, sq_spi_master_isr, NULL);
}

extern void
sq_spi_master_free()
{
	free_irq(sq_irq_num);
}

extern void
sq_spi_set_current_mode(int mode)
{
	current_mode=mode;
}

extern int
sq_spi_get_current_mode(void)
{
	return current_mode;
}

extern int
sq_spi_transfer(void *tx_buf, void *rx_buf, u32 len)
{	
	u32 tx_cnt = 0, rx_cnt = 0;
	sq_spi_isr_tx_buf_idx = 0;
	sq_spi_isr_rx_buf_idx = 0;
	sq_spi_isr_total_len = GET_TX_LEN(len);

	if (tx_buf != NULL)
		tx_cnt = GET_TX_LEN(len);
	if (rx_buf != NULL)
		rx_cnt = GET_RX_LEN(len);

	sq_spi_isr_tx_buf = tx_buf;
	sq_spi_isr_rx_buf = rx_buf;

	/* Reset the flags */
	sq_spi_tx_complete_flag = 0;
	sq_spi_rx_complete_flag = 0;

	/* Check if SPI controller is running */
	while (SQ_SPI_TXRX_RUN == (sq_spi_read(SQ_SPI_FWCR) & SQ_SPI_TXRX_RUN))
		/* NOP */;

	/* Set transfer & receive data count */
	sq_spi_write(tx_cnt, SQ_SPI_TXCR);
	sq_spi_write(rx_cnt, SQ_SPI_RXCR);
	
	if (tx_cnt > 0) {
		while (SQ_SPI_TXFIFO_FULL != (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_TXFIFO_FULL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				sq_spi_write(((u8 *)sq_spi_isr_tx_buf)[sq_spi_isr_tx_buf_idx++], SQ_SPI_TXR); 
			else if (MODE_CHAR_LEN_16 == current_mode)
				sq_spi_write(((u16 *)sq_spi_isr_tx_buf)[sq_spi_isr_tx_buf_idx++], SQ_SPI_TXR);
			if (sq_spi_isr_tx_buf_idx == sq_spi_isr_total_len) {
				sq_spi_tx_complete_flag = 1;
				break;
			}
		}
	}
	if(sq_spi_tx_complete_flag!=1 && tx_cnt > 0)
		sq_spi_write(sq_spi_read(SQ_SPI_IER) | SQ_SPI_IER_TXFIFO_INT_EN, SQ_SPI_IER);

	/* Start SPI transfer */
	sq_spi_write(
			sq_spi_read(SQ_SPI_FWCR) |
			SQ_SPI_EN |
			SQ_SPI_TXRX_RUN,
			SQ_SPI_FWCR);

	/* Wait for transfer to be complete */
	if (rx_cnt > 0) {
		if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30)) {
			printf("Timeout\n");
			return -1;
		}
	} 
	else {
		if (sq_wait_for_int(&sq_spi_tx_complete_flag, 30)) {
			printf("Timeout\n");
			return -1;
		}
	}
	while (SQ_SPI_TXRX_RUN == (sq_spi_read(SQ_SPI_FWCR) & SQ_SPI_TXRX_RUN))
		/* NOP */;
	return 0;
}

static u32
sq_spi_power(u32 base, u32 exp)
{
	u32 i;
	u32 val = 1;
	
	if (0 == exp)
		return 1;
	else {
		for (i = 0; i < exp; i++)
			val *= base;
		return val;
	}
}

extern u32
sq_spi_calculate_divisor(u32 clk)
{
	u8 div_high_3 = 0, div_low_3 = 0, spi_cdvr;
	u32 sclk_divisor, sclk, pclk, power;

	/*
	 *  SCLK Divisor = (SPICDVR[5:3] + 1) * 2 power(SPICDVR[2:0] + 1)
	 *  SCLK = PCLK / SCLK Divisor
	 *  */
	pclk = sq_scu_apb_clock_get();
	while (1) {
		power = sq_spi_power(2, div_low_3+1);
		for (div_high_3 = 0; div_high_3 < 8; div_high_3++) {
			sclk_divisor = (div_high_3 + 1) * power;
			sclk = pclk / sclk_divisor;
			if (sclk < clk)
				goto out;
		}
		div_low_3++;
	}
out:
	spi_cdvr = (div_high_3 << 3) | div_low_3;
	printf("\nsclk is %d, divisor is 0x%08x\n", sclk, spi_cdvr);
	return spi_cdvr;
}

static void 
sq_spi_master_isr(void *pparam)
{
	/* Read & clear interrupt status */
	u32 isr_tmp = sq_spi_read(SQ_SPI_ISR);
	
	if (SQ_SPI_RXFIFO_OVR_INT == (isr_tmp & SQ_SPI_RXFIFO_OVR_INT)) {
		sq_spi_tx_complete_flag = -1;
		sq_spi_rx_complete_flag = -1;
		printf("\nReceive FIFO is full and another character has been received in the receiver shift register\n");
		return;
	}
	/* Check is receive complete */
	if (SQ_SPI_RX_COMPLETE_INT == (isr_tmp & SQ_SPI_RX_COMPLETE_INT)) {
		while (SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				((u8 *)sq_spi_isr_rx_buf)[sq_spi_isr_rx_buf_idx++] = sq_spi_read(SQ_SPI_RXR);
			else if (MODE_CHAR_LEN_16 == current_mode)
				((u16 *)sq_spi_isr_rx_buf)[sq_spi_isr_rx_buf_idx++] = sq_spi_read(SQ_SPI_RXR);		
		}
		sq_spi_rx_complete_flag = 1;
		return;
	}
	/* Check if any receive data is available */
	if (SQ_SPI_RXFIFO_INT == (isr_tmp & SQ_SPI_RXFIFO_INT)) {
//printf("@@SQ_SPI_RXFIFO_INT\n");
		while (SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				((u8 *)sq_spi_isr_rx_buf)[sq_spi_isr_rx_buf_idx++] = sq_spi_read(SQ_SPI_RXR);
			else if (MODE_CHAR_LEN_16 == current_mode) {
				((u16 *)sq_spi_isr_rx_buf)[sq_spi_isr_rx_buf_idx++] = sq_spi_read(SQ_SPI_RXR);
			}
		}
		return;
	}
	/* Check if the transmit FIFO is available */
	if (SQ_SPI_TXFIFO_INT == (isr_tmp & SQ_SPI_TXFIFO_INT)) {
		while (SQ_SPI_TXFIFO_FULL != (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_TXFIFO_FULL)) {
			if (MODE_CHAR_LEN_8 == current_mode)
				sq_spi_write(((u8 *)sq_spi_isr_tx_buf)[sq_spi_isr_tx_buf_idx++], SQ_SPI_TXR);
			else if (MODE_CHAR_LEN_16 == current_mode)
				sq_spi_write(((u16 *)sq_spi_isr_tx_buf)[sq_spi_isr_tx_buf_idx++], SQ_SPI_TXR);
			if (sq_spi_isr_tx_buf_idx == sq_spi_isr_total_len) {
				sq_spi_write(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_TXFIFO_INT_EN, SQ_SPI_IER);
				sq_spi_tx_complete_flag = 1;
				return;
			}
		}
		return;
	}
	
}

extern void
sq_spi_make_test_pattern(u8 *buf, u32 size)
{
	int i;    
	for (i = 0; i < size; i++)
		buf[i] = i;
}



extern int
sq_spi_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size, int skip_cmpr_result)
{
	int i;
	u32 *mem_32 = (u32 *)mem;
	u32 *cmpr_mem_32 = (u32 *)cmpr_mem;
	int err_flag = 0;

	for (i = 0; i < (size >> 2); i++) {
		if (mem_32[i] != cmpr_mem_32[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result)
				printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem_32[i], &mem_32[i], 
				       cmpr_mem_32[i], &cmpr_mem_32[i]);
		}
	}
	return err_flag;
}
