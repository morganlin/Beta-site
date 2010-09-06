#include <test_item.h>
#include <global.h>
#include <dma/dma.h>
#include "spi-regs.h"
#include "dependency.h"
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

/* Commands */

#define CMD_READ 0x01	/* read data from memory array */
#define CMD_WRITE 0x02	/* write data to memory array */
#define CMD_DMARD 0x03
#define CMD_DMAWR 0x04
#define CMD_EXIT 0x0F
#define REG_CMD_RESET_EN 0x0E
#define REG_CMD_RESET 0x0C

#define STA_READY              0x01
#define STA_BUSY               0x00
#define STA_RESET_EN          (0x01 << 1)
#define STA_CMD                0x00
#define STA_GET_ADDR_HI   		 0x10
#define STA_GET_ADDR_LOW       0x11
#define STA_GET_SIZE_HI        0x12
#define STA_GET_SIZE_LOW       0x13

#define PATTERN_BUF_SIZE  2048
#define PATTERN_BUF_SIZE_PURE 8
static void sq_spi_slave_protocol_isr(void *data);
static void sq_spi_slave_pure_isr(void *data);
static void sq_spi_slave_reg_isr(void *data);
static void sq_spi_slave_tx_dma_page_interrupt(void *data);
static void sq_spi_slave_rx_dma_page_interrupt(void *data);


static struct sq_dma_notifier sq_spi_slave_tx_dma_notifier = {
	.page_interrupt = sq_spi_slave_tx_dma_page_interrupt,
};

static struct sq_dma_notifier sq_spi_slave_rx_dma_notifier = {
	.page_interrupt = sq_spi_slave_rx_dma_page_interrupt,
};

static int sq_spi_slave_protocol_run(void);
static int sq_spi_slave_pure_hdma_run(u32 sq_spi_slave_dma_burst);
static void sq_spi_slave_write(void);
static void sq_spi_slave_read(void);
static void sq_spi_slave_dma_read(void);
static void sq_spi_slave_dma_write(void);
static void sq_spi_slave_run_state(void);
static void sq_spi_slave_protocol_reset(void);
static int sq_spi_reg_rw_run(void);
static int start,protocol,regflag;
static u32 sq_spi_base,sq_spi_irq,cpha,cpol,lsb,char_len;
static u8 state,command;
static u32 sq_spi_tx_dma_ch_num, sq_spi_rx_dma_ch_num;
static u32 sq_spi_tx_dma_ext_hdreq, sq_spi_rx_dma_ext_hdreq;
static u16 temp_addr,addr_temp,temp_size,count;
static u8 *storage = (u8 *)0x00a00000;
static u16 *storage16= (u16 *)0x00a01000;
static volatile int sq_spi_tx_complete_flag = 0;
static volatile int sq_spi_rx_complete_flag = 0;
static volatile int sq_spi_tx_finish_flag = 0;



static inline void
sq_spi_write(u32 val, u32 reg)
{
	iowrite32(val, sq_spi_base+reg);
}

static inline u32
sq_spi_read(u32 reg)
{
	return ioread32(sq_spi_base+reg);
}

extern struct test_item_container sq_spi_slave_test_container;

extern int
sq_spi_slave_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&sq_spi_slave_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_slave_model_test_container;

int sq_spi_slave0_test(int autotest)
{
	int ret = 0;
#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_SPI0);
	sq_scu_hdma_req45_spi0();
#endif
	
	sq_spi_base = SQ_SPI0;
	sq_spi_irq = SQ_INTC_SPI0;
	sq_spi_tx_dma_ext_hdreq = 5;
	sq_spi_rx_dma_ext_hdreq = 4;

	sq_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	sq_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	ret = test_item_ctrl(&sq_spi_slave_model_test_container, autotest);

#if defined(CONFIG_PC9223)
	sq_scu_dev_disable(SQ_DEVCON_SPI0);
#endif
	
	return ret;

}

int sq_spi_slave1_test(int autotest)
{
	int ret = 0;
#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_SPI1);
	sq_scu_hdma_req45_spi1();
#endif	
	sq_spi_base = SQ_SPI1;
	sq_spi_irq = SQ_INTC_SPI1;
	sq_spi_tx_dma_ext_hdreq = 5;
	sq_spi_rx_dma_ext_hdreq = 4;

	sq_spi_tx_dma_ch_num = PANTHER7_HDMA_CH_1;
	sq_spi_rx_dma_ch_num = PANTHER7_HDMA_CH_0;
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	ret = test_item_ctrl(&sq_spi_slave_model_test_container, autotest);
#if defined(CONFIG_PC9223)
	sq_scu_dev_disable(SQ_DEVCON_SPI1);
#endif
	
	return ret;

}

extern struct test_item_container sq_spi_slave_mode_test_container;

extern int
sq_spi_slave_protocol_test(int autotest)
{
	int ret = 0;
	protocol=1;
	ret = test_item_ctrl(&sq_spi_slave_mode_test_container, autotest);
	return ret;
}

extern int
sq_spi_slave_pure_test(int autotest)
{
	int ret = 0;
	protocol=0;
	regflag=0;
	ret = test_item_ctrl(&sq_spi_slave_mode_test_container, autotest);
	return ret;
}

extern int
sq_spi_reg_rw_test(int autotest)
{
	int ret = 0;
	regflag=1;
	protocol=0;
	ret = test_item_ctrl(&sq_spi_slave_mode_test_container, autotest);
	return ret;
	
}

extern struct test_item_container sq_spi_slave_sb_test_container;

extern int
sq_spi_slave_mode0_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_0;
	cpol=SQ_SPI_CPOL_0;
	if(protocol)
		ret = sq_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_sb_test_container, autotest);
	return ret;
}

extern int 
sq_spi_slave_mode1_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_0;
	cpol=SQ_SPI_CPOL_1;
	if(protocol)
		ret = sq_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_sb_test_container, autotest);
	return ret;
}


extern int 
sq_spi_slave_mode2_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_1;
	cpol=SQ_SPI_CPOL_0;
	if(protocol)
		ret = sq_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_sb_test_container, autotest);
	return ret;
}

extern int 
sq_spi_slave_mode3_test(int autotest)
{
	int ret = 0;
	cpha=SQ_SPI_CPHA_1;
	cpol=SQ_SPI_CPOL_1;
	if(protocol)
		ret = sq_spi_slave_protocol_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_sb_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_slave_ch_test_container;

extern int 
sq_spi_slave_lsb_test(int autotest)
{
	int ret = 0;
	lsb= SQ_SPI_TX_LSB_FIRST;
	ret = test_item_ctrl(&sq_spi_slave_ch_test_container, autotest);
	return ret;
}

extern int 
sq_spi_slave_msb_test(int autotest)
{
	int ret = 0;
	lsb = SQ_SPI_TX_MSB_FIRST;
	ret = test_item_ctrl(&sq_spi_slave_ch_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_slave_pure_type_test_container;

extern int
sq_spi_slave_ch8_test(int autotest)
{
	int ret = 0;
	char_len= SQ_SPI_CHAR_LEN_8;
	if(regflag)
		ret = sq_spi_reg_rw_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_pure_type_test_container, autotest);
	return ret;
}

extern int
sq_spi_slave_ch16_test(int autotest)
{
	int ret = 0;
	char_len= SQ_SPI_CHAR_LEN_16;
	if(regflag)
		ret = sq_spi_reg_rw_run();
	else
		ret = test_item_ctrl(&sq_spi_slave_pure_type_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_spi_slave_burst_type_test_container;

extern int
sq_spi_slave_pure_hdma_test(int autotest)
{
	int ret = 0;
	ret = test_item_ctrl(&sq_spi_slave_burst_type_test_container, autotest);
	return ret;
}

extern int
sq_spi_slave_hdma_burst_single(int autotest)
{
	return sq_spi_slave_pure_hdma_run(SQ_DMA_BURST_SINGLE);
}
extern int
sq_spi_slave_hdma_burst_incr4(int autotest)
{
	return sq_spi_slave_pure_hdma_run(SQ_DMA_BURST_INCR4);
}


static int
sq_spi_slave_protocol_run(void)
{
	sq_spi_write(STA_BUSY, SQ_SPI_TX_REG0);
	request_irq(sq_spi_irq, sq_spi_slave_protocol_isr, NULL);
	sq_request_dma(sq_spi_rx_dma_ch_num, &sq_spi_slave_rx_dma_notifier);
	sq_request_dma(sq_spi_tx_dma_ch_num, &sq_spi_slave_tx_dma_notifier);
	
	memset((char *)storage, 0, PATTERN_BUF_SIZE);
	start=1;
	sq_spi_slave_protocol_reset();
	sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
	printf("Slave Start\n");
	while(start) {
		if(state==CMD_WRITE)
			sq_spi_slave_write();
		else if(state==CMD_READ)
			sq_spi_slave_read();
		else if(state==CMD_DMAWR)
			sq_spi_slave_dma_write();
		else if(state==CMD_DMARD)
			sq_spi_slave_dma_read();
	}
	
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	sq_spi_write(STA_BUSY, SQ_SPI_TX_REG0);
	printf("program stop\n");	
	sq_free_dma(sq_spi_rx_dma_ch_num);
	sq_free_dma(sq_spi_tx_dma_ch_num);
	free_irq(sq_spi_irq);
	return 0;
}

static void
sq_spi_slave_protocol_isr(void *pparam)
{
	u32 tmp = sq_spi_read(SQ_SPI_ISR);
	
	if (SQ_SPI_RX_DATA_AVAIL_INT == (tmp & SQ_SPI_RX_DATA_AVAIL_INT)) {
		if(state!=CMD_WRITE)
			sq_spi_slave_run_state();
		else {			
			while (SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL)){
				storage[temp_addr+count] = sq_spi_read(SQ_SPI_RXR);
				count++;
				if(temp_size==count) {
					sq_spi_rx_complete_flag=1;
					break;
				}
			}
		}
		return;
	}
	 	
	if (SQ_SPI_TXFIFO_INT == (tmp & SQ_SPI_TXFIFO_INT)) {
		do {   
			sq_spi_write(storage[temp_addr+count], SQ_SPI_TXR);
			count++;
			if(temp_size==count) {
				sq_spi_write(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_TXFIFO_INT_EN, SQ_SPI_IER);
				sq_spi_tx_complete_flag=1;
				return;
			} 
		} while (SQ_SPI_TXFIFO_FULL != (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_TXFIFO_FULL));
		return;
	}
	
	if (SQ_SPI_TXFIFO_EMPTY_INT == (tmp & SQ_SPI_TXFIFO_EMPTY_INT)) {
			sq_spi_write(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_TXFIFO_EMPTY_INT_EN, SQ_SPI_IER);
			sq_spi_tx_finish_flag=1;
    	return; 
	}
	
	if (SQ_SPI_RX_REG0_INT == (tmp & SQ_SPI_RX_REG0_INT)) {
		sq_spi_write(SQ_SPI_RX_REG0_INT,SQ_SPI_ISR);
		if( REG_CMD_RESET_EN == sq_spi_read(SQ_SPI_RX_REG0))
			sq_spi_write(STA_RESET_EN, SQ_SPI_TX_REG0);
		else if( REG_CMD_RESET == sq_spi_read(SQ_SPI_RX_REG0)) {
			if(STA_RESET_EN != sq_spi_read(SQ_SPI_TX_REG0))
				return;
			sq_spi_slave_protocol_reset();
			if(state > CMD_DMAWR || state < CMD_READ) {
				state=STA_CMD;
				sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
			}
		}				
	}
	
	if(SQ_SPI_IER_CHAR_LEN_INT_EN == (tmp & SQ_SPI_CHAR_LEN_INT)) {
		sq_spi_write(SQ_SPI_CHAR_LEN_INT,SQ_SPI_ISR);
		printf("Wrong bits\n");
		start=0;
	}
	
}

static void 
sq_spi_slave_run_state(void)
{
	switch(state) {
  	case STA_CMD:
  		command=sq_spi_read(SQ_SPI_RXR);
    	if(command == CMD_READ || command == CMD_WRITE || command == CMD_DMAWR || command == CMD_DMARD) {
    		sq_spi_write(STA_BUSY, SQ_SPI_TX_REG0);
    		state=STA_GET_ADDR_HI;
    	}		
			else if(command == CMD_EXIT)
				start=0;
			break;
    case STA_GET_ADDR_HI:
     	temp_addr = (sq_spi_read(SQ_SPI_RXR) << 8);
     	state=STA_GET_ADDR_LOW;
     	break;     
    case STA_GET_ADDR_LOW:
    	temp_addr = (temp_addr | sq_spi_read(SQ_SPI_RXR));
    	state=STA_GET_SIZE_HI;               	
     	break;
    case STA_GET_SIZE_HI:
    	temp_size = (sq_spi_read(SQ_SPI_RXR) << 8);
    	state=STA_GET_SIZE_LOW;
     	break;
    case STA_GET_SIZE_LOW:
    	temp_size = (temp_size | sq_spi_read(SQ_SPI_RXR));
    	state=command;
    	sq_spi_write(sq_spi_read(SQ_SPI_IER)& ~SQ_SPI_IER_RXAVAIL_INT_EN, SQ_SPI_IER);    	
    	break;
 	}
} 

static void
sq_spi_slave_write(void)
{
	sq_spi_rx_complete_flag=0;
	count=0;
	sq_spi_write(sq_spi_read(SQ_SPI_IER) | SQ_SPI_IER_RXAVAIL_INT_EN , SQ_SPI_IER);
	if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30)) {
		printf("Rx Timeout\n");
	}
	if(SQ_SPI_RXFIFO_DATA_AVAIL == (ioread32(sq_spi_base+SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL))
		iowrite32(ioread32(sq_spi_base+SQ_SPI_FCR) | SQ_SPI_RXFIFO_CLR, sq_spi_base+SQ_SPI_FCR);
	
	state=STA_CMD;
	sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
}


static void
sq_spi_slave_dma_write(void)
{
	sq_spi_rx_complete_flag = 0;
	
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	sq_set_dma_mode(sq_spi_rx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_rx_dma_ch_num, sq_spi_rx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_rx_dma_ch_num, SQ_DMA_BURST_INCR4);
	sq_set_dma_source_address(sq_spi_rx_dma_ch_num, SQ_SPI_RXR+sq_spi_base);
	sq_set_dma_source_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_destination_address(sq_spi_rx_dma_ch_num, (u32)&storage[temp_addr]);
	sq_set_dma_destination_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_data_size(sq_spi_rx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_slice_count(sq_spi_rx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_rx_dma_ch_num, 1);	
	sq_set_dma_transfer_count(sq_spi_rx_dma_ch_num, temp_size);
	sq_enable_dma(sq_spi_rx_dma_ch_num);
	
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR) | SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30)) {
		printf("DMA Rx Timeout\n");
	}
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR) & ~SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	
	if(SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL))
		sq_spi_write(sq_spi_read(SQ_SPI_FCR) | SQ_SPI_RXFIFO_CLR, SQ_SPI_FCR);
	
	state=STA_CMD;
	sq_spi_write(sq_spi_read(SQ_SPI_IER) | SQ_SPI_IER_RXAVAIL_INT_EN , SQ_SPI_IER);	
	sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
	
}


static void
sq_spi_slave_read(void)
{
  sq_spi_tx_complete_flag=0;
  sq_spi_tx_finish_flag=0;
  count=0;
  
  
  while(sq_spi_tx_complete_flag!=1) {
  	while (SQ_SPI_TXFIFO_FULL != (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_TXFIFO_FULL)) {
  		sq_spi_write(storage[temp_addr+count], SQ_SPI_TXR);
			count++;
			if(temp_size==count) {
				sq_spi_tx_complete_flag=1;
				break;
			}
  	}
  }
  
  sq_spi_write(sq_spi_read(SQ_SPI_IER) | 
						SQ_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SQ_SPI_IER);
						
	if(sq_wait_for_int(&sq_spi_tx_finish_flag, 30)) {
		printf("Tx Timeout\n");
	}
	if(SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL))
		sq_spi_write(sq_spi_read(SQ_SPI_FCR) | SQ_SPI_RXFIFO_CLR, SQ_SPI_FCR);
	if(SQ_SPI_SLAVE_TxFF_EMPTY != (sq_spi_read(SQ_SPI_FIFO_STATUS) & SQ_SPI_SLAVE_TxFF_EMPTY))
		sq_spi_write(sq_spi_read(SQ_SPI_FCR) | SQ_SPI_TXFIFO_CLR, SQ_SPI_FCR);
	state=STA_CMD;
	sq_spi_write(sq_spi_read(SQ_SPI_IER) | SQ_SPI_IER_RXAVAIL_INT_EN , SQ_SPI_IER);
	sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
	
}

static void
sq_spi_slave_dma_read(void)
{	
	sq_spi_tx_complete_flag=0;
  sq_spi_tx_finish_flag=0;
  
	sq_disable_dma(sq_spi_tx_dma_ch_num);
	sq_set_dma_mode(sq_spi_tx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_tx_dma_ch_num, sq_spi_tx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_tx_dma_ch_num, SQ_DMA_BURST_INCR4);	
	sq_set_dma_source_address(sq_spi_tx_dma_ch_num, (u32)&storage[temp_addr]);
	sq_set_dma_source_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_destination_address(sq_spi_tx_dma_ch_num, SQ_SPI_TXR+sq_spi_base);
	sq_set_dma_destination_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_data_size(sq_spi_tx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_slice_count(sq_spi_tx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_tx_dma_ch_num, 1);
	sq_set_dma_transfer_count(sq_spi_tx_dma_ch_num, temp_size);
	sq_enable_dma(sq_spi_tx_dma_ch_num);
	
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR)| SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	if (sq_wait_for_int(&sq_spi_tx_complete_flag, 30)) {
		printf("DMA Tx Timeout\n");
	}
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR) & ~SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	sq_disable_dma(sq_spi_tx_dma_ch_num);
	
	sq_spi_write(sq_spi_read(SQ_SPI_IER) |
						SQ_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SQ_SPI_IER);
	
	if (sq_wait_for_int(&sq_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
	}
						
	if(SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL))
		sq_spi_write(sq_spi_read(SQ_SPI_FCR) | SQ_SPI_RXFIFO_CLR, SQ_SPI_FCR);
	if(SQ_SPI_SLAVE_TxFF_EMPTY != (sq_spi_read(SQ_SPI_FIFO_STATUS) & SQ_SPI_SLAVE_TxFF_EMPTY))
		sq_spi_write(sq_spi_read(SQ_SPI_FCR) | SQ_SPI_TXFIFO_CLR, SQ_SPI_FCR);
	
	state=STA_CMD;	
	sq_spi_write(sq_spi_read(SQ_SPI_IER) | SQ_SPI_IER_RXAVAIL_INT_EN , SQ_SPI_IER);
	sq_spi_write(STA_READY, SQ_SPI_TX_REG0);
	
}



static void
sq_spi_slave_protocol_reset(void)
{
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	sq_spi_write(SQ_SPI_CHAR_LEN_8, SQ_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
			SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_RXFIFO_CLR |
			SQ_SPI_TXFIFO_CLR,
			SQ_SPI_FCR);
     
	/* Enable SPI interrupt */
	sq_spi_write(
			//SQ_SPI_IER_WRONG_BIT_INT_EN |
			SQ_SPI_IER_RXFIFO_OVR_INT_EN |
			SQ_SPI_IER_RX_REG0_INT_EN |
			//SQ_SPI_IER_SS_INT_EN |
			SQ_SPI_IER_RXAVAIL_INT_EN,
			SQ_SPI_IER);
			
	sq_spi_write(
			SQ_SPI_MODE_SLAVE |
			SQ_SPI_EN |
			cpol |
			cpha |
			SQ_SPI_TX_MSB_FIRST,
			SQ_SPI_FWCR);
	
	count=0;
	sq_spi_tx_complete_flag=1;
	sq_spi_rx_complete_flag=1;
	sq_spi_tx_finish_flag=1;
	
}


static void
sq_spi_slave_tx_dma_page_interrupt(void *data)
{	
	sq_spi_tx_complete_flag = 1;
}

static void
sq_spi_slave_rx_dma_page_interrupt(void *data)
{	
	sq_spi_rx_complete_flag = 1;
}

extern int
sq_spi_slave_pure_normal_run(int autotest)
{
	
	sq_spi_rx_complete_flag=0;
	sq_spi_tx_complete_flag=0;
	sq_spi_tx_finish_flag=0;
	/* Reset SPI controller */
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	
	request_irq(sq_spi_irq, sq_spi_slave_pure_isr, NULL);
	
	memset((char *)storage, 0, PATTERN_BUF_SIZE);
	memset((char *)storage16, 0, PATTERN_BUF_SIZE);
	temp_addr=0;
	addr_temp=0;
	sq_spi_write(char_len, SQ_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
			SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_2 |
			SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_2 |
			SQ_SPI_RXFIFO_CLR |
			SQ_SPI_TXFIFO_CLR,
			SQ_SPI_FCR);
     
	/* Enable SPI interrupt */
	sq_spi_write(
			SQ_SPI_IER_RXFIFO_OVR_INT_EN |
			//SQ_SPI_IER_SS_INT_EN |
			SQ_SPI_IER_RXFIFO_INT_EN ,
			SQ_SPI_IER);
			
	sq_spi_write(
			SQ_SPI_MODE_SLAVE |
			SQ_SPI_EN |
			cpol |
			cpha |
			lsb,
			SQ_SPI_FWCR);
	printf("Wait data\n");
	
	if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30)) {
		printf("Rx Timeout\n");
	}
	printf("Rx Finish\n");

	sq_spi_write(
			(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_RXFIFO_INT_EN) |
			SQ_SPI_IER_TXFIFO_INT_EN,
			SQ_SPI_IER);
			
	if (sq_wait_for_int(&sq_spi_tx_complete_flag, 10)) {
		printf("Tx Timeout\n");
	}
	sq_spi_write(sq_spi_read(SQ_SPI_IER) |
						SQ_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SQ_SPI_IER);
	if (sq_wait_for_int(&sq_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
	}
	printf("Tx Finish\n");
	free_irq(sq_spi_irq);
	printf("Transfer End\n");
	
	return 0;

}

static int
sq_spi_slave_pure_hdma_run(u32 sq_spi_slave_dma_burst)
{
	
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	
	request_irq(sq_spi_irq, sq_spi_slave_pure_isr, NULL);
	sq_request_dma(sq_spi_rx_dma_ch_num, &sq_spi_slave_rx_dma_notifier);
	sq_request_dma(sq_spi_tx_dma_ch_num, &sq_spi_slave_tx_dma_notifier);
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	sq_disable_dma(sq_spi_tx_dma_ch_num);
	
	sq_spi_tx_complete_flag = 0;
	sq_spi_rx_complete_flag = 0;
	sq_spi_tx_finish_flag=0;
	memset((char *)storage, 0, PATTERN_BUF_SIZE);
	memset((char *)storage16, 0, PATTERN_BUF_SIZE);
	
	sq_spi_write(char_len, SQ_SPI_SSCR);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
			SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SQ_SPI_RXFIFO_CLR |
			SQ_SPI_TXFIFO_CLR,
			SQ_SPI_FCR);
     
	/* Enable SPI interrupt */
	sq_spi_write(
			SQ_SPI_IER_RXFIFO_OVR_INT_EN,
			//SQ_SPI_IER_SS_INT_EN,
			SQ_SPI_IER);
			
	sq_spi_write(
			SQ_SPI_MODE_SLAVE |
			SQ_SPI_EN |
			cpol |
			cpha |
			lsb,
			SQ_SPI_FWCR);
	
	sq_set_dma_mode(sq_spi_rx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_rx_dma_ch_num, sq_spi_rx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_rx_dma_ch_num, sq_spi_slave_dma_burst);
	sq_set_dma_source_address(sq_spi_rx_dma_ch_num, SQ_SPI_RXR+sq_spi_base);
	sq_set_dma_source_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_destination_direction(sq_spi_rx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_slice_count(sq_spi_rx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_rx_dma_ch_num, 1);
	if(char_len==SQ_SPI_CHAR_LEN_8) {
		sq_set_dma_transfer_count(sq_spi_rx_dma_ch_num, PATTERN_BUF_SIZE_PURE);
		sq_set_dma_destination_address(sq_spi_rx_dma_ch_num, (u32)&storage[0]);		
		sq_set_dma_data_size(sq_spi_rx_dma_ch_num, SQ_DMA_DATA_BYTE);
	}
	else if(char_len==SQ_SPI_CHAR_LEN_16) {
		sq_set_dma_transfer_count(sq_spi_rx_dma_ch_num, PATTERN_BUF_SIZE_PURE*2);
		sq_set_dma_destination_address(sq_spi_rx_dma_ch_num, (u32)&storage16[0]);
		sq_set_dma_data_size(sq_spi_rx_dma_ch_num, SQ_DMA_DATA_HALFWORD);
	}
	
	sq_set_dma_mode(sq_spi_tx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_spi_tx_dma_ch_num, sq_spi_tx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_spi_tx_dma_ch_num, sq_spi_slave_dma_burst);
	sq_set_dma_destination_address(sq_spi_tx_dma_ch_num, SQ_SPI_TXR+sq_spi_base);
	sq_set_dma_source_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_destination_direction(sq_spi_tx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_slice_count(sq_spi_tx_dma_ch_num, 4);
	sq_set_dma_page_number(sq_spi_tx_dma_ch_num, 1);
	if(char_len==SQ_SPI_CHAR_LEN_8) {
		sq_set_dma_transfer_count(sq_spi_tx_dma_ch_num, PATTERN_BUF_SIZE_PURE);
		sq_set_dma_source_address(sq_spi_tx_dma_ch_num, (u32)&storage[0]);
		sq_set_dma_data_size(sq_spi_tx_dma_ch_num, SQ_DMA_DATA_BYTE);
	}
	else if(char_len==SQ_SPI_CHAR_LEN_16) {
		sq_set_dma_transfer_count(sq_spi_tx_dma_ch_num, PATTERN_BUF_SIZE_PURE*2);
		sq_set_dma_source_address(sq_spi_tx_dma_ch_num, (u32)&storage16[0]);
		sq_set_dma_data_size(sq_spi_tx_dma_ch_num, SQ_DMA_DATA_HALFWORD);
	}
	
	
	sq_enable_dma(sq_spi_rx_dma_ch_num);
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR)| SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	
	printf("Wait data\n");
	if (sq_wait_for_int(&sq_spi_rx_complete_flag, 30)) {
		printf("DMA Rx Timeout\n");
		return -1;
	}
	sq_disable_dma(sq_spi_rx_dma_ch_num);
	
	printf("DMA Rx Finish\n");
	
	sq_enable_dma(sq_spi_tx_dma_ch_num);
	
	if (sq_wait_for_int(&sq_spi_tx_complete_flag, 10)) {
		printf("DMA Tx Timeout\n");
	}
	sq_disable_dma(sq_spi_tx_dma_ch_num);
	sq_spi_write(sq_spi_read(SQ_SPI_FWCR) & ~SQ_SPI_DMA_REQ, SQ_SPI_FWCR);
	
	sq_spi_write(sq_spi_read(SQ_SPI_IER) |
						SQ_SPI_IER_TXFIFO_EMPTY_INT_EN,
						SQ_SPI_IER);
	if (sq_wait_for_int(&sq_spi_tx_finish_flag, 10)) {
		printf("Tx Timeout\n");
		return -1;
	}
	
	printf("DMA Tx Finish\n");
	sq_free_dma(sq_spi_rx_dma_ch_num);
	sq_free_dma(sq_spi_tx_dma_ch_num);
	free_irq(sq_spi_irq);
	printf("Transfer End\n");

	return 0;
}

static void
sq_spi_slave_pure_isr(void *pparam)
{
	u32 tmp = sq_spi_read(SQ_SPI_ISR);
	
	if (SQ_SPI_RXFIFO_OVR_INT == (tmp & SQ_SPI_RXFIFO_OVR_INT)) {
		printf("\nReceive FIFO is full and another character has been received in the receiver shift register\n");
		return;
	}	

	if (SQ_SPI_RXFIFO_INT == (tmp & SQ_SPI_RXFIFO_INT)) {
		while (SQ_SPI_RXFIFO_DATA_AVAIL == (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_RXFIFO_DATA_AVAIL)) {
				if(char_len==SQ_SPI_CHAR_LEN_8)
					storage[addr_temp] = sq_spi_read(SQ_SPI_RXR);
				else if(char_len==SQ_SPI_CHAR_LEN_16)
					storage16[addr_temp] = sq_spi_read(SQ_SPI_RXR);
				addr_temp++;
				if(addr_temp==PATTERN_BUF_SIZE_PURE) {
					sq_spi_rx_complete_flag=1;
					break;
				}
		}		
		return;
	}
 	
	if (SQ_SPI_TXFIFO_INT == (tmp & SQ_SPI_TXFIFO_INT)) {
		while (SQ_SPI_TXFIFO_FULL != (sq_spi_read(SQ_SPI_FCR) & SQ_SPI_TXFIFO_FULL)) {
			if(char_len==SQ_SPI_CHAR_LEN_8)
				sq_spi_write(storage[temp_addr], SQ_SPI_TXR);
			else if(char_len==SQ_SPI_CHAR_LEN_16)
				sq_spi_write(storage16[temp_addr], SQ_SPI_TXR);
			temp_addr++;
			if(temp_addr==PATTERN_BUF_SIZE_PURE) {
				sq_spi_write(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_TXFIFO_INT_EN, SQ_SPI_IER);
				sq_spi_tx_complete_flag=1;
				break;
			}
		}
		return;
	}
	if (SQ_SPI_TXFIFO_EMPTY_INT == (tmp & SQ_SPI_TXFIFO_EMPTY_INT)) {
			sq_spi_write(sq_spi_read(SQ_SPI_IER) & ~SQ_SPI_IER_TXFIFO_EMPTY_INT_EN, SQ_SPI_IER);
			sq_spi_tx_finish_flag=1;
    	return; 
	}
}

static int
sq_spi_reg_rw_run(void)
{	
	request_irq(sq_spi_irq, sq_spi_slave_reg_isr, NULL);
	
	sq_spi_write(SQ_SPI_SOFT_RST, SQ_SPI_FWCR);
	sq_spi_write(char_len, SQ_SPI_SSCR);
	
	/* Enable SPI interrupt */
	sq_spi_write(
			SQ_SPI_IER_RX_REG0_INT_EN | 
			SQ_SPI_IER_RX_REG1_INT_EN |
			SQ_SPI_IER_RX_REG2_INT_EN | 
			SQ_SPI_IER_RX_REG3_INT_EN,
			SQ_SPI_IER);
			
	sq_spi_write(
			SQ_SPI_MODE_SLAVE |
			SQ_SPI_EN |
			cpol |
			cpha |
			lsb,
			SQ_SPI_FWCR);
	
	sq_spi_write(0x11, SQ_SPI_TX_REG0);
	sq_spi_write(0, SQ_SPI_TX_REG1);
	sq_spi_write(0, SQ_SPI_TX_REG2);
	sq_spi_write(0, SQ_SPI_TX_REG3);
	while(regflag);
	
	free_irq(sq_spi_irq);
	regflag=1;
	printf("Reg Test Finish\n");
	return 0;
}

static void
sq_spi_slave_reg_isr(void *pparam)
{
	u32 tmp = sq_spi_read(SQ_SPI_ISR);
	
	if (SQ_SPI_RX_REG0_INT == (tmp & SQ_SPI_RX_REG0_INT)) {
		sq_spi_write(SQ_SPI_RX_REG0_INT,SQ_SPI_ISR);
		u32 regvalue=sq_spi_read(SQ_SPI_RX_REG0);
		printf("RxReg0: %x\n", regvalue);
		if( 0xAA == regvalue)
			sq_spi_write(0x22, SQ_SPI_TX_REG1);				
		return;
	}	

	if (SQ_SPI_RX_REG1_INT == (tmp & SQ_SPI_RX_REG1_INT)) {
		sq_spi_write(SQ_SPI_RX_REG1_INT,SQ_SPI_ISR);
		u32 regvalue=sq_spi_read(SQ_SPI_RX_REG1);
		printf("RxReg1: %x\n", regvalue);
		if( 0xBB == regvalue)
			sq_spi_write(0x33, SQ_SPI_TX_REG2);				
		return;
	}
 	
	if (SQ_SPI_RX_REG2_INT == (tmp & SQ_SPI_RX_REG2_INT)) {
		sq_spi_write(SQ_SPI_RX_REG2_INT,SQ_SPI_ISR);
		u32 regvalue=sq_spi_read(SQ_SPI_RX_REG2);
		printf("RxReg2: %x\n", regvalue);
		if( 0xCC == regvalue)
			sq_spi_write(0x44, SQ_SPI_TX_REG3);				
		return;
	}
	if (SQ_SPI_RX_REG3_INT == (tmp & SQ_SPI_RX_REG3_INT)) {
		sq_spi_write(SQ_SPI_RX_REG3_INT,SQ_SPI_ISR);
		u32 regvalue=sq_spi_read(SQ_SPI_RX_REG3);
		printf("RxReg3: %x\n", regvalue);
		if( 0xDD == regvalue)
			regflag=0;				
		return;
	}
}
