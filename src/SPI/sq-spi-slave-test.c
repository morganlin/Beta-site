#include "spi-master.h"
/*Sq slave protocol command*/
#define SQ_CMD_READ 0x01	/* read data from memory array */
#define SQ_CMD_WRITE 0x02	/* write data to memory array */
#define SQ_CMD_DMARD 0x03
#define SQ_CMD_DMAWR 0x04
#define SQ_CMD_EXIT 0x0F
#define REG_CMD_RESET_EN 0x0E
#define REG_CMD_RESET 0x0C

#define STA_READY 0x01
#define STA_BUSY 0x00
#define STA_RESET_EN (0x01 << 1)

#define SQ_HW_CMD_READ 0x01
#define SQ_HW_CMD_RDFS 0x02
#define SQ_HW_CMD_RDREG_0 0x03
#define SQ_HW_CMD_RDREG_1 0x04
#define SQ_HW_CMD_RDREG_2 0x05
#define SQ_HW_CMD_RDREG_3 0x06
#define SQ_HW_CMD_WRITE 0x08
#define SQ_HW_CMD_WRREG_0 0x09
#define SQ_HW_CMD_WRREG_1 0x0A
#define SQ_HW_CMD_WRREG_2 0x0B
#define SQ_HW_CMD_WRREG_3 0x0C

#define PATTERN_BUF_SIZE_PURE 8

static u8 *sq_spi_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u8 *sq_spi_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);

static int sq_spi_wait_slave_reset_en(void);
static int sq_spi_slave_reset(void);
static int sq_spi_wait_slave_ready(void);
static int sq_spi_wait_slave_fifo(u8 fs);
static int sq_spi_wait_slave_fifo_16(u8 fs);
static int sq_spi_slave_write_data(u16 addr, u8 *buf, u16 len, int type);
static int sq_spi_slave_read_data(u16 addr, u8 *buf, u16 len, int type);
static int sq_spi_slave_exit(void);
static int sq_spi_slave_wait_reg_value(u8 regcmd, u8 wait_value);
static int sq_spi_slave_wait_reg_value_16(u16 regcmd, u16 wait_value);
static int sq_spi_slave_reset_en(void);


extern int
sq_spi_slave_run(int type)
{
   int ret; 
    /* Reset SPI controller */
  sq_spi_write(
  	sq_spi_read(SQ_SPI_FWCR) |
    SQ_SPI_SOFT_RST, 
    SQ_SPI_FWCR);

	/* Enable SPI interrupt */
	sq_spi_write(
		SQ_SPI_IER_RXFIFO_INT_EN |
		//SQ_SPI_IER_RXFIFO_OVR_INT_EN |
		SQ_SPI_IER_RX_COMPLETE_INT_EN,
		SQ_SPI_IER);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
		SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_RXFIFO_CLR |
		SQ_SPI_TXFIFO_CLR,
		SQ_SPI_FCR);
  
	sq_spi_write(
		SQ_SPI_CHAR_LEN_8 |
		SQ_SPI_SLAVE_SEL_0 |
		SQ_SPI_CLK_DIV(SLAVE_DIV),
		SQ_SPI_SSCR);

	/* Config SPI clock delay */
	sq_spi_write(
		SQ_SPI_PBTXRX_DELAY_NONE |
		SQ_SPI_PBCT_DELAY_NONE |
		SQ_SPI_PBCA_DELAY_1_2,
		SQ_SPI_DLYCR);
	
	/* Set per char length */
	sq_spi_set_current_mode(MODE_CHAR_LEN_8);
	
	if(type==SLAVE_RESET) {
		printf("Reset test\n");
		if(sq_spi_slave_reset_en())
			return -1;
		if(sq_spi_wait_slave_reset_en())
			return -1;
		if(sq_spi_slave_reset())
			return -1;
		if(sq_spi_wait_slave_ready())
			return -1;
		ret=0;
	}
	else {
		/* Clear pattern buffer and compare buffer */
		memset((char *)sq_spi_pattern_buf, 0,PATTERN_BUF_SIZE );
		memset((char *)sq_spi_cmpr_buf, 0, PATTERN_BUF_SIZE);
		/* Make test pattern */
		sq_spi_make_test_pattern(sq_spi_pattern_buf, PATTERN_BUF_SIZE);
	
		printf("wait slave ready...\n");
		if(sq_spi_wait_slave_ready())
			return -1;
		printf("write data to slave...\n");
		if (sq_spi_slave_write_data(0,sq_spi_pattern_buf, PATTERN_BUF_SIZE, type))
	  	return -1;
	  printf("wait slave ready...\n");
		if(sq_spi_wait_slave_ready())
			return -1;
	  printf("read data from slave...\n");
	  if(sq_spi_slave_read_data(0,sq_spi_cmpr_buf, PATTERN_BUF_SIZE, type))
			return -1;
    printf("disconnect...\n");
    if(sq_spi_wait_slave_ready())
			return -1;
		if(sq_spi_slave_exit())
			printf("exit error\n");
		printf("compare data...\n");
	  ret=sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, PATTERN_BUF_SIZE, 0);
	}
	return ret;
}

extern int
sq_spi_slave_pure_run(void)
{
  int ret;
  u8 pdata8[PATTERN_BUF_SIZE_PURE+1];
  u16 pdata16[PATTERN_BUF_SIZE_PURE+1];
  /* Reset SPI controller */
  sq_spi_write(
  	sq_spi_read(SQ_SPI_FWCR) |
    SQ_SPI_SOFT_RST, 
    SQ_SPI_FWCR);
            
	/* Enable SPI interrupt */
	sq_spi_write(
		//SQ_SPI_IER_TXFIFO_INT_EN |
		SQ_SPI_IER_RXFIFO_INT_EN |
		//SQ_SPI_IER_RXFIFO_OVR_INT_EN |
		SQ_SPI_IER_RX_COMPLETE_INT_EN,
		SQ_SPI_IER);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
		SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_RXFIFO_CLR |
		SQ_SPI_TXFIFO_CLR,
		SQ_SPI_FCR);
  
	/* Config SPI clock delay */
	sq_spi_write(
		SQ_SPI_PBTXRX_DELAY_NONE |
		SQ_SPI_PBCT_DELAY_NONE |
		SQ_SPI_PBCA_DELAY_1_2,
		SQ_SPI_DLYCR);
	
	/* Clear pattern buffer and compare buffer */
	memset((char *)sq_spi_pattern_buf, 0, PATTERN_BUF_SIZE_PURE*2);
	memset((char *)sq_spi_cmpr_buf, 0, PATTERN_BUF_SIZE_PURE*2);
	/* Make test pattern */
	sq_spi_make_test_pattern(sq_spi_pattern_buf, PATTERN_BUF_SIZE_PURE*2);
	
	if(sq_spi_get_current_mode() == MODE_CHAR_LEN_8){
		printf("write data to slave...\n");	
		pdata8[0]=SQ_HW_CMD_WRITE;
		memcpy((char *)&pdata8[1], (char *)sq_spi_pattern_buf, PATTERN_BUF_SIZE_PURE);
  	if (sq_spi_transfer(pdata8, NULL, SET_TX_RX_LEN(PATTERN_BUF_SIZE_PURE+1, 0)))
  		return -1;
  	if(sq_spi_wait_slave_fifo(SQ_SPI_SLAVE_TxFF_FULL))
			return -1;
		printf("read data from slave...\n");
		pdata8[0]=SQ_HW_CMD_READ;
		if (sq_spi_transfer(pdata8, sq_spi_cmpr_buf, SET_TX_RX_LEN(1, PATTERN_BUF_SIZE_PURE)))
  		return -1;
  	printf("compare data...\n");
		ret = sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, PATTERN_BUF_SIZE_PURE, 0);
	}
	else if(sq_spi_get_current_mode() == MODE_CHAR_LEN_16){
		printf("write data to slave...\n");	
		pdata16[0]=SQ_HW_CMD_WRITE;
		memcpy((char *)&pdata16[1], (char *)sq_spi_pattern_buf, PATTERN_BUF_SIZE_PURE*2);
  	if (sq_spi_transfer(pdata16, NULL, SET_TX_RX_LEN(PATTERN_BUF_SIZE_PURE+1, 0)))
  		return -1;
  	if(sq_spi_wait_slave_fifo_16(SQ_SPI_SLAVE_TxFF_FULL))
			return -1;
		printf("read data from slave...\n");
		pdata16[0]=SQ_HW_CMD_READ;
		if (sq_spi_transfer(pdata16, sq_spi_cmpr_buf, SET_TX_RX_LEN(1, PATTERN_BUF_SIZE_PURE)))
  		return -1;
  	printf("compare data...\n");
		ret = sq_spi_compare_memory(sq_spi_pattern_buf, sq_spi_cmpr_buf, PATTERN_BUF_SIZE_PURE*2, 0);
	}
	return ret;
}

extern int
sq_spi_slave_reg_run(void)
{
	 u8 cmd8[2];
	 u16 cmd16[2];
	  /* Reset SPI controller */
  sq_spi_write(
  	sq_spi_read(SQ_SPI_FWCR) |
    SQ_SPI_SOFT_RST, 
    SQ_SPI_FWCR);
            
	/* Enable SPI interrupt */
	sq_spi_write(
		SQ_SPI_IER_RX_COMPLETE_INT_EN,
		SQ_SPI_IER);
	
	/* Configure FIFO and clear Tx & Rx FIFO */
	sq_spi_write(
		SQ_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
		SQ_SPI_RXFIFO_CLR |
		SQ_SPI_TXFIFO_CLR,
		SQ_SPI_FCR);
  
	/* Config SPI clock delay */
	sq_spi_write(
		SQ_SPI_PBTXRX_DELAY_NONE |
		SQ_SPI_PBCT_DELAY_NONE |
		SQ_SPI_PBCA_DELAY_1_2,
		SQ_SPI_DLYCR);
	
	if(sq_spi_get_current_mode() == MODE_CHAR_LEN_8) {
	
		printf("read tx reg0\n");
		if(sq_spi_slave_wait_reg_value(SQ_HW_CMD_RDREG_0, 0x11))
			return -1;
		cmd8[0]=SQ_HW_CMD_WRREG_0;
		cmd8[1]=0xAA;
		printf("write rx reg0\n");
		if (sq_spi_transfer(cmd8, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg1\n");
		if(sq_spi_slave_wait_reg_value(SQ_HW_CMD_RDREG_1, 0x22))
			return -1;
		cmd8[0]=SQ_HW_CMD_WRREG_1;
		cmd8[1]=0xBB;
		printf("write rx reg1\n");
		if(sq_spi_transfer(cmd8, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg2\n");
		if(sq_spi_slave_wait_reg_value(SQ_HW_CMD_RDREG_2, 0x33))
			return -1;
		cmd8[0]=SQ_HW_CMD_WRREG_2;
		cmd8[1]=0xCC;
		printf("write rx reg2\n");
		if(sq_spi_transfer(cmd8, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg3\n");
		if(sq_spi_slave_wait_reg_value(SQ_HW_CMD_RDREG_3, 0x44))
			return -1;
		cmd8[0]=SQ_HW_CMD_WRREG_3;
		cmd8[1]=0xDD;
		printf("write rx reg3\n");
		if(sq_spi_transfer(cmd8, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
	}
	else if(sq_spi_get_current_mode() == MODE_CHAR_LEN_16) {
		
		printf("read tx reg0\n");
		if(sq_spi_slave_wait_reg_value_16(SQ_HW_CMD_RDREG_0, 0x11))
			return -1;
		cmd16[0]=SQ_HW_CMD_WRREG_0;
		cmd16[1]=0xAA;
		printf("write rx reg0\n");
		if (sq_spi_transfer(cmd16, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg1\n");
		if(sq_spi_slave_wait_reg_value_16(SQ_HW_CMD_RDREG_1, 0x22))
			return -1;
		cmd16[0]=SQ_HW_CMD_WRREG_1;
		cmd16[1]=0xBB;
		printf("write rx reg1\n");
		if(sq_spi_transfer(cmd16, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg2\n");
		if(sq_spi_slave_wait_reg_value_16(SQ_HW_CMD_RDREG_2, 0x33))
			return -1;
		cmd16[0]=SQ_HW_CMD_WRREG_2;
		cmd16[1]=0xCC;
		printf("write rx reg2\n");
		if(sq_spi_transfer(cmd16, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
		printf("read tx reg3\n");
		if(sq_spi_slave_wait_reg_value_16(SQ_HW_CMD_RDREG_3, 0x44))
			return -1;
		cmd16[0]=SQ_HW_CMD_WRREG_3;
		cmd16[1]=0xDD;
		printf("write rx reg3\n");
		if(sq_spi_transfer(cmd16, NULL,SET_TX_RX_LEN(2, 0)))
			return -1;
	
	}	
	
	return 0;
	
}

static int 
sq_spi_wait_slave_reset_en(void)
{
	u8 cmd, stat;
	u32 retry_cnt = 0;

	cmd = SQ_HW_CMD_RDREG_0;

	while(1) {
		if (sq_spi_transfer(&cmd, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if (stat == STA_RESET_EN)
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;
	}
	return 0;
}

static int 
sq_spi_slave_reset(void)
{
	u8 cmd[2];
	cmd[0] = SQ_HW_CMD_WRREG_0;
	cmd[1] = REG_CMD_RESET;
	if (sq_spi_transfer(cmd, NULL,SET_TX_RX_LEN(2, 0)))
		return -1;
	return 0;
}

static int 
sq_spi_wait_slave_ready(void)
{
	u8 stat;
	u32 retry_cnt = 0;
  u8 cmd=SQ_HW_CMD_RDREG_0;
	while(1) {
		if (sq_spi_transfer( &cmd, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if (stat == STA_READY)
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;
		
	}
	return 0;
}

static int 
sq_spi_wait_slave_fifo(u8 fs)
{
	u8 stat;
	u32 retry_cnt = 0;
  u8 cmd=SQ_HW_CMD_RDFS;
	while(1) {
		if (sq_spi_transfer( &cmd, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if ( (stat & fs)== fs )
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;		
	}
	return 0;
}

static int 
sq_spi_wait_slave_fifo_16(u8 fs)
{
	u16 stat;
	u32 retry_cnt = 0;
  u16 cmd=SQ_HW_CMD_RDFS;
	while(1) {
		if (sq_spi_transfer( &cmd, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if ( (stat & fs)== fs )
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;		
	}
	return 0;
}

static int
sq_spi_slave_write_data(u16 addr, u8 *buf, u16 len, int type)
{
	u16 i=0;
	u8 stat;
	u8 rdfs=SQ_HW_CMD_RDFS;
	
	u8 tx_buf[9] = {0};
	u8 cmd[6] = {0};
	cmd[0] = SQ_HW_CMD_WRITE;
	if(type==SLAVE_NORMAL)
		cmd[1] = SQ_CMD_WRITE;
	else if(type==SLAVE_HDMA)
		cmd[1] = SQ_CMD_DMAWR;
	cmd[2] = (addr >> 8);
	cmd[3] = (addr & 0xff);
	cmd[4] = (len >> 8);
	cmd[5] = (len & 0xff);

	if(sq_spi_transfer(cmd, NULL, SET_TX_RX_LEN(6,0)))
		return -1;
		
	tx_buf[0]=SQ_HW_CMD_WRITE;
	
	while(i<len) {
		if (sq_spi_transfer( &rdfs, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if( stat & SQ_SPI_SLAVE_RxFF_EMPTY ) {
			memcpy((char *)&tx_buf[1], (char *)&buf[i], SQ_SPI_FIFO_SIZE);	
			if(sq_spi_transfer(&tx_buf, &buf[i], SET_TX_RX_LEN(SQ_SPI_FIFO_SIZE+1,0)))
				return -1;
			i+=SQ_SPI_FIFO_SIZE;
		}
		else if(!(stat & SQ_SPI_SLAVE_RxFF_HALFFULL)) {
			memcpy((char *)&tx_buf[1], (char *)&buf[i], SQ_SPI_FIFO_SIZE/2);	
			if(sq_spi_transfer(&tx_buf, &buf[i], SET_TX_RX_LEN(SQ_SPI_FIFO_SIZE/2+1,0)))
				return -1;
			i+=(SQ_SPI_FIFO_SIZE/2);
		}
	}
	
	return 0;
}

static int
sq_spi_slave_read_data(u16 addr, u8 *buf, u16 len, int type)
{
	u16 i=0;
	u8 stat;
	u8 rdfs=SQ_HW_CMD_RDFS;
	u8 tx_buf=SQ_HW_CMD_READ;
	
	u8 cmd[6] = {0};
	cmd[0] = SQ_HW_CMD_WRITE;
	if(type==SLAVE_NORMAL)
		cmd[1] = SQ_CMD_READ;
	else if(type==SLAVE_HDMA)
		cmd[1] = SQ_CMD_DMARD;
	cmd[2] = (addr >> 8);
	cmd[3] = (addr & 0xff);
	cmd[4] = (len >> 8);
	cmd[5] = (len & 0xff);

	if(sq_spi_transfer(cmd, NULL, SET_TX_RX_LEN(6,0)))
		return -1;
		
	while(i<len) {
		if (sq_spi_transfer( &rdfs, &stat, SET_TX_RX_LEN(1, 1)))
			return -1;
		if( stat & SQ_SPI_SLAVE_TxFF_FULL ) {
			if(sq_spi_transfer(&tx_buf, &buf[i], SET_TX_RX_LEN(1,SQ_SPI_FIFO_SIZE)))
				return -1;
			i+=SQ_SPI_FIFO_SIZE;
		}
		else if( stat & SQ_SPI_SLAVE_TxFF_HALFFULL ) {
			if(sq_spi_transfer(&tx_buf, &buf[i], SET_TX_RX_LEN(1,SQ_SPI_FIFO_SIZE/2)))
				return -1;
			i+=(SQ_SPI_FIFO_SIZE/2);
		}			
	}
	
	return 0;
}


static int 
sq_spi_slave_exit(void)
{
	u8 cmd[2];
	cmd[0] = SQ_HW_CMD_WRITE;
	cmd[1] = SQ_CMD_EXIT;
	if (sq_spi_transfer(cmd, NULL,SET_TX_RX_LEN(2, 0)))
		return -1;
	return 0;
}

static int
sq_spi_slave_wait_reg_value(u8 regcmd, u8 wait_value)
{
	u8 rtv,cmd;
	u32 retry_cnt = 0;
  cmd=regcmd;

	while(1) {
		if (sq_spi_transfer(&cmd, &rtv, SET_TX_RX_LEN(1, 1)))
			return -1;
		if (rtv == wait_value)
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;
	}
	return 0;
}

static int
sq_spi_slave_wait_reg_value_16(u16 regcmd, u16 wait_value)
{
	u16 rtv,cmd;
	u32 retry_cnt = 0;
  cmd=regcmd;

	while(1) {
		if (sq_spi_transfer(&cmd, &rtv, SET_TX_RX_LEN(1, 1)))
			return -1;
		if (rtv == wait_value)
			break;
		if (retry_cnt > 2000) {
			printf("Retry count (%d) exceed threshold\n", retry_cnt);
			return -1;
		}
		retry_cnt++;
	}
	return 0;
}

static int 
sq_spi_slave_reset_en(void)
{
	u8 cmd[2] = {0};
	cmd[0] = SQ_HW_CMD_WRREG_0;
	cmd[1] = REG_CMD_RESET_EN;
	if (sq_spi_transfer(cmd, NULL,SET_TX_RX_LEN(2, 0)))
		return -1;
	return 0;
}
