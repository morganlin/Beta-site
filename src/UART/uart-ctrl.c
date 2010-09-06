#include <genlib.h>
#include <global.h>
#include <test_item.h>
#include <type.h>
#include <io.h>
#include <dma/dma.h>
#include "uart-regs.h"
#include "dependency.h"
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

static inline void
sq_uart_write(u32 reg, u32 value, u32 base)
{
	base = base + reg;
	iowrite32(value, base);
}

static inline u32
sq_uart_read(u32 reg, u32 base)
{
	u32 val;

	base = base + reg;
	val = ioread32(base);
	return val;
}

static void sq_uart_set_baudrate(int baudrate);
static int sq_uart_normal(int autotest);
static int sq_uart_hwdma_panther7_hdma(int autotest);
static void sq_uart_make_test_pattern(u8 *buffer, u32 size);
static void sq_uart_isr(void *pparam);
static void sq_uart_show_lsr_error(void);
static int sq_uart_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
				     int skip_cmpr_result);
static void sq_uart_init_environment(int dma_req_en);
extern int sq_uart_hwdma_panther7_hdma_bursttype_test(int autotest);
static void sq_uart_tx_dma_page_interrupt(void *data);
static void sq_uart_rx_dma_page_interrupt(void *data);
static int gps_location(int gps[]);

static struct sq_dma_notifier sq_uart_tx_dma_notifier = {
	.page_interrupt = sq_uart_tx_dma_page_interrupt,
};

static struct sq_dma_notifier sq_uart_rx_dma_notifier = {
	.page_interrupt = sq_uart_rx_dma_page_interrupt,
};

static u32 sq_uart_tx_dma_ch_num, sq_uart_rx_dma_ch_num;
static u32 sq_uart_tx_dma_ext_hdreq, sq_uart_rx_dma_ext_hdreq;
static u32 sq_uart_dma_burst;
static u32 sq_uart_base;
int (*uart_test)(int autotest);

static volatile int sq_uart_tx_complete_flag = 0;
static volatile int sq_uart_rx_complete_flag = 0;
extern struct test_item_container sq_uart_ip_test_container;

extern int 
uart_main_test(int autotest)
{
	int ret = 0;
	
/*	UART clock enable	*/
	ret = test_item_ctrl(&sq_uart_ip_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_uart_type_test_container;

extern int
sq_uart_0_test(int autotest)
{
	int ret = 0;

	sq_uart_base = SQ_UART0;

#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_UART0);
	sq_scu_hdma_req01_uart(0);
	sq_scu_hdma_req23_uart(0);
#endif

	{
		extern struct test_item sq_uart_module_test_items[];
		
		sq_uart_module_test_items[0].enable = SQ_UART_GPS_0_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item sq_uart_transfer_test_items[];
		sq_uart_transfer_test_items[1].enable = SQ_UART_EXT_0_TEST;
	}
	/* Disable all interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_DIS |
			 SQ_UART_IE_TBE_DIS |
			 SQ_UART_IE_RBR_DIS,
			 sq_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	sq_uart_read(SQ_UART_IIR, sq_uart_base);
	sq_uart_read(SQ_UART_LSR, sq_uart_base);

	request_irq(SQ_INTC_UART0, sq_uart_isr, NULL);

	sq_uart_tx_dma_ext_hdreq = 1; 
	sq_uart_rx_dma_ext_hdreq = 0;

	ret = test_item_ctrl(&sq_uart_type_test_container, autotest);

	free_irq(SQ_INTC_UART0);

#if defined(CONFIG_PC9223)
	sq_scu_dev_disable(SQ_DEVCON_UART0);
#endif
	return ret;
}

extern int
sq_uart_1_test(int autotest)
{
	int ret = 0;

	sq_uart_base = SQ_UART1;

#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_UART1);
	sq_scu_hdma_req23_uart(1);
#endif

	{
		extern struct test_item sq_uart_module_test_items[];
		
		sq_uart_module_test_items[0].enable = SQ_UART_GPS_1_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item sq_uart_transfer_test_items[];
		sq_uart_transfer_test_items[1].enable = SQ_UART_EXT_1_TEST;
	}
	/* Disable all interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_DIS |
			 SQ_UART_IE_TBE_DIS |
			 SQ_UART_IE_RBR_DIS,
			 sq_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	sq_uart_read(SQ_UART_IIR, sq_uart_base);
	sq_uart_read(SQ_UART_LSR, sq_uart_base);

	request_irq(SQ_INTC_UART1, sq_uart_isr, NULL);


	sq_uart_tx_dma_ext_hdreq = 3; 
	sq_uart_rx_dma_ext_hdreq = 2;

	ret = test_item_ctrl(&sq_uart_type_test_container, autotest);

	free_irq(SQ_INTC_UART1);

#if defined(CONFIG_PC9223)
        sq_scu_dev_disable(SQ_DEVCON_UART1);
#endif

	return ret;
}

extern int
sq_uart_2_test(int autotest)
{
	int ret = 0;

	sq_uart_base = SQ_UART2;

#if defined(CONFIG_PC9223)
        sq_scu_dev_enable(SQ_DEVCON_UART2);
        sq_scu_hdma_req23_uart(2);
#endif

	{
		extern struct test_item sq_uart_module_test_items[];
		
		sq_uart_module_test_items[0].enable = SQ_UART_GPS_2_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item sq_uart_transfer_test_items[];
		sq_uart_transfer_test_items[1].enable = SQ_UART_EXT_2_TEST;
	}
	/* Disable all interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_DIS |
			 SQ_UART_IE_TBE_DIS |
			 SQ_UART_IE_RBR_DIS,
			 sq_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	sq_uart_read(SQ_UART_IIR, sq_uart_base);
	sq_uart_read(SQ_UART_LSR, sq_uart_base);

	request_irq(SQ_INTC_UART2, sq_uart_isr, NULL);

	sq_uart_tx_dma_ext_hdreq = 3; 
	sq_uart_rx_dma_ext_hdreq = 2;	

	ret = test_item_ctrl(&sq_uart_type_test_container, autotest);

	free_irq(SQ_INTC_UART2);

#if defined(CONFIG_PC9223)
        sq_scu_dev_disable(SQ_DEVCON_UART2);
#endif

	return ret;
}

extern int
sq_uart_3_test(int autotest)
{
	int ret = 0;

	sq_uart_base = SQ_UART3;

	{
		extern struct test_item sq_uart_module_test_items[];
		
		sq_uart_module_test_items[0].enable = SQ_UART_GPS_3_TEST;
	}
	//2008 jsho add, for memu control
	{
		extern struct test_item sq_uart_transfer_test_items[];
		sq_uart_transfer_test_items[1].enable = SQ_UART_EXT_3_TEST;
	}
	/* Disable all interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_DIS |
			 SQ_UART_IE_TBE_DIS |
			 SQ_UART_IE_RBR_DIS,
			 sq_uart_base);

	/* Read IIR and LSR to clear the pending interrupt */
	sq_uart_read(SQ_UART_IIR, sq_uart_base);
	sq_uart_read(SQ_UART_LSR, sq_uart_base);

	request_irq(SQ_INTC_UART3, sq_uart_isr, NULL);

	sq_uart_tx_dma_ext_hdreq = 3; 
	sq_uart_rx_dma_ext_hdreq = 2;

	ret = test_item_ctrl(&sq_uart_type_test_container, autotest);

	free_irq(SQ_INTC_UART3);

	return ret;
}

extern struct test_item_container sq_uart_transfer_test_container;

extern int
sq_uart_normal_test(int autotest)
{
	int ret = 0;

	uart_test = sq_uart_normal;
	ret = test_item_ctrl(&sq_uart_transfer_test_container, autotest);
	return ret;
}

extern struct test_item_container sq_uart_panther7_hdma_bursttype_test_container;

extern int 
sq_uart_hwdma_panther7_hdma_test(int autotest)
{
	int ret = 0;

	uart_test = sq_uart_hwdma_panther7_hdma_bursttype_test;
        sq_uart_tx_dma_ch_num = PANTHER7_HDMA_CH_0;
        sq_uart_rx_dma_ch_num = PANTHER7_HDMA_CH_1;

	sq_request_dma(sq_uart_tx_dma_ch_num, &sq_uart_tx_dma_notifier);
	sq_request_dma(sq_uart_rx_dma_ch_num, &sq_uart_rx_dma_notifier);
	ret = test_item_ctrl(&sq_uart_transfer_test_container, autotest);
	sq_disable_dma(sq_uart_tx_dma_ch_num);
	sq_disable_dma(sq_uart_rx_dma_ch_num);
	sq_free_dma(sq_uart_tx_dma_ch_num);
	sq_free_dma(sq_uart_rx_dma_ch_num);
	return ret;
}

extern struct test_item_container sq_uart_panther7_hdma_bursttype_test_container;

extern int sq_uart_hwdma_panther7_hdma_bursttype_test(int autotest)
{
	int ret = 0;

	ret = test_item_ctrl(&sq_uart_panther7_hdma_bursttype_test_container, autotest);
	return ret;
}

extern int 
sq_uart_panther7_hdma_bursttype_single(int autotest)
{
	sq_uart_dma_burst = SQ_DMA_BURST_SINGLE;
	return sq_uart_hwdma_panther7_hdma(autotest);
}

extern int 
sq_uart_panther7_hdma_bursttype_incr4(int autotest)
{
	sq_uart_dma_burst = SQ_DMA_BURST_INCR4;
	return sq_uart_hwdma_panther7_hdma(autotest);
}

extern int 
sq_uart_panther7_hdma_bursttype_incr8(int autotest)
{
	sq_uart_dma_burst = SQ_DMA_BURST_INCR8;
	return sq_uart_hwdma_panther7_hdma(autotest);
}

extern struct test_item_container sq_uart_baudrate_test_container;

extern int
sq_uart_inter_loopback_test(int autotest)
{
	int ret = 0;

	sq_uart_write(SQ_UART_MCR, SQ_UART_MCR_LOOPBACK, sq_uart_base);
	ret = test_item_ctrl(&sq_uart_baudrate_test_container, autotest);
	return ret;
}

extern int
sq_uart_ext_loopback_test(int autotest)
{
	int ret = 0;

	sq_uart_write(SQ_UART_MCR, SQ_UART_MCR_NORMAL, sq_uart_base);
	ret = test_item_ctrl(&sq_uart_baudrate_test_container, autotest);

	return ret;
}

extern struct test_item_container sq_uart_module_test_container;

extern int
sq_uart_module_test(int autotest)
{
	int ret = 0;

	sq_uart_write(SQ_UART_MCR, SQ_UART_MCR_NORMAL, sq_uart_base);
	ret = test_item_ctrl(&sq_uart_module_test_container, autotest);

	return ret;
}



extern int
sq_uart_gps_test(int autotest)
{
	int ret = 0, i = 0, tmp[1000]={0}, idx = 0;
	int gps[100], gps_index;

	sq_uart_set_baudrate(9600);

	/* Initialize the test environment */
	sq_uart_init_environment(0);

	printf("GPS Data:\n");
	while (i < 1000) {
		if (SQ_UART_LSR_DR == (sq_uart_read(SQ_UART_LSR, sq_uart_base) & SQ_UART_LSR_DR)) {
			while (SQ_UART_LSR_DR == (sq_uart_read(SQ_UART_LSR, sq_uart_base) & SQ_UART_LSR_DR)) {
				tmp[idx] = sq_uart_read(SQ_UART_RBR, sq_uart_base);
				idx++;
				//printf("%d(%c) ", tmp[idx], tmp[idx]);
			}
			//printf("\n");
			i++;
		}
		if (idx >= 1000)
			break;
	}
	
	gps_index=0;
	for (i = 0; i < idx; i++) {
		if ((10 == tmp[i]) || (13 == tmp[i])){
			printf("\n");
			gps_index=0;
			ret = gps_location(gps);
			if(!ret){
				return ret;
			}
		}else{
			printf("%c", tmp[i]);
			gps[gps_index++] = tmp[i];
//			printf("%d(%c) ", tmp[i], tmp[i]);
		}
	}
	printf("\n");

	return ret;
}

static int 
gps_location(int gps[])
{
	int index, i, j;
	char data[8][16];
	
	
	if((gps[3] == 'R') && (gps[4] == 'M') && (gps[5] == 'C')){
		if((gps[18] == 'A')){
			for(index=7,j=0;index<=16;index++, j++)
				data[0][j]=gps[index];
			data[0][j]=0;			

			i=1;
			for(index=20, j=0;index<70;index++){
				if(gps[index] == ','){
					data[i][j]=0;
					i++;
					j=0;
					if(i==8)
						break;
				}else{
					data[i][j++]=gps[index];
				}		
			}
		}else
			return -1;		
	}else
		return -1;
	

	printf("\n\n==========GPS Information=============\n");
	printf("Date(ddmmyy) : %s\n", data[7]);
	printf("UTC Time(hhmmss.sss) : %s\n", data[0]);
	printf("Latitude : %s\n", data[1]);
	printf("Longitude : %s\n", data[3]);
	printf("North or Soutg : %s\n", data[2]);
	printf("East or West : %s\n", data[4]);
	printf("Speed Over Ground : %s\n", data[5]);
	printf("Course Over Ground : %s\n", data[6]);
	
	return 0;
} 


extern int
sq_uart_baudrate_75(int autotest)
{
	sq_uart_set_baudrate(75);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_300(int autotest)
{
	sq_uart_set_baudrate(300);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_1200(int autotest)
{
	sq_uart_set_baudrate(1200);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_2400(int autotest)
{
	sq_uart_set_baudrate(2400);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_9600(int autotest)
{
	sq_uart_set_baudrate(9600);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_19200(int autotest)
{
	sq_uart_set_baudrate(19200);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_38400(int autotest)
{
	sq_uart_set_baudrate(38400);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_57600(int autotest)
{
	sq_uart_set_baudrate(57600);
	return uart_test(autotest);
}

extern int
sq_uart_baudrate_115200(int autotest)
{
	sq_uart_set_baudrate(115200);
	return uart_test(autotest);
}

static void
sq_uart_set_baudrate(int baudrate)
{
	u32 old_lcr;
	u32 dlsb, dmsb;

	dlsb = sq_scu_uart_clock_get (0) / (16 * baudrate);

	dmsb = dlsb >> 8;
	dlsb &= 0xff;
	old_lcr = sq_uart_read(SQ_UART_LCR, sq_uart_base);
	sq_uart_write(SQ_UART_LCR, old_lcr|SQ_UART_LCR_DIV_LATCH_EN, sq_uart_base);
	sq_uart_write(SQ_UART_DLL, dlsb, sq_uart_base);
	sq_uart_write(SQ_UART_DLH, dmsb, sq_uart_base);
	sq_uart_write(SQ_UART_LCR, old_lcr&(~SQ_UART_LCR_DIV_LATCH_EN), sq_uart_base);
}

#define PATTERN_BUF_ADDR 0x00a00000
#define PATTERN_BUF_SIZE 2048

static u8 *sq_uart_pattern_buf = (u8 *)PATTERN_BUF_ADDR;
static u32 sq_uart_pattern_buf_idx = 0;
static u8 *sq_uart_cmpr_buf = (u8 *)(PATTERN_BUF_ADDR + PATTERN_BUF_SIZE);
static u32 sq_uart_cmpr_buf_idx = 0;

static int
sq_uart_normal(int autotest)
{
	/* Initialize the test environment */
	sq_uart_init_environment(0);

	/* Make the test pattern */
	sq_uart_make_test_pattern(sq_uart_pattern_buf, PATTERN_BUF_SIZE);

	/* Trigger the transfer */
	sq_uart_write(SQ_UART_THR, sq_uart_pattern_buf[sq_uart_pattern_buf_idx++], sq_uart_base);

	/* Enable specify interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_EN |
			 SQ_UART_IE_TBE_EN |
			 SQ_UART_IE_RBR_EN,
			 sq_uart_base);

	/* Wait for transfer to be complete */
	if (sq_wait_for_int(&sq_uart_rx_complete_flag, 10)) {
		printf("Timeout\n");
		return -1;
	}

	if (-1 == sq_uart_rx_complete_flag)
		return -1; 
	else
		/* Compare the memory */
		return sq_uart_compare_memory(sq_uart_pattern_buf, sq_uart_cmpr_buf, PATTERN_BUF_SIZE,
						 autotest);
}

static void
sq_uart_make_test_pattern(u8 *buf, u32 size)
{
	int i;

	for (i = 0; i < size; i++)
		buf[i] = i;
}

static void 
sq_uart_isr(void* pparam)
{
	u32 tmp;
	int i;

	do {
		/* Read & clear the interrupt status */
		tmp = sq_uart_read(SQ_UART_IIR, sq_uart_base);

		/* Check if any error occur */
		if (SQ_UART_IIR_LS_INT == (tmp & SQ_UART_IIR_LS_INT)) {
			sq_uart_rx_complete_flag = -1;
			sq_uart_show_lsr_error();
			return;
		}
	
		/* Check if it is receive data interrupt or not */
		if ((SQ_UART_IIR_RBR_INT == (tmp & SQ_UART_IIR_RBR_INT)) || 
		    (SQ_UART_IIR_TO_INT == (tmp & SQ_UART_IIR_TO_INT))) {
//		    	printf("cyli test rx %d\n", sq_uart_cmpr_buf_idx);
			while (SQ_UART_LSR_DR == (sq_uart_read(SQ_UART_LSR, sq_uart_base) & SQ_UART_LSR_DR))
				sq_uart_cmpr_buf[sq_uart_cmpr_buf_idx++] = sq_uart_read(SQ_UART_RBR, sq_uart_base);
			if (PATTERN_BUF_SIZE == sq_uart_cmpr_buf_idx) {
				sq_uart_rx_complete_flag = 1;
				return;
			}
		}

		/* Check if it is the transmit buffer empty interrupt or not */
		if (SQ_UART_IIR_TBE_INT == (tmp & SQ_UART_IIR_TBE_INT)) {
			if (sq_uart_pattern_buf_idx > PATTERN_BUF_SIZE) {
				sq_uart_rx_complete_flag = -1;
				printf("\nTransfered data is more than the size of test pattern: %d\n", sq_uart_pattern_buf_idx);
				return;
	
			}
			
			if (PATTERN_BUF_SIZE == sq_uart_pattern_buf_idx) {
				return;
			}else {
				if (1 == sq_uart_pattern_buf_idx) {
					for (i = 0; i < (SQ_UART_FIFO_SIZE - 1); i++)
						sq_uart_write(SQ_UART_THR, sq_uart_pattern_buf[sq_uart_pattern_buf_idx++], sq_uart_base);
				} else {
					for (i = 0; i < SQ_UART_FIFO_SIZE; i++) 
						sq_uart_write(SQ_UART_THR, sq_uart_pattern_buf[sq_uart_pattern_buf_idx++], sq_uart_base);
				}
			}
		}
	} while (0 == (tmp & SQ_UART_IIR_PEND_INT));
}

static void
sq_uart_show_lsr_error(void)
{
	u32 lsr;

	lsr = sq_uart_read(SQ_UART_LSR, sq_uart_base);
	if (SQ_UART_LSR_ERR == (lsr & SQ_UART_LSR_ERR))
		printf("\nSq UART host:At least one parity error, framming error or break indications have been received and are inside the FIFO\n");
	if (SQ_UART_LSR_TEMT == (lsr & SQ_UART_LSR_TEMT))
		printf("\nSq UART host: Transmitter Empty indicator\n");
	if (SQ_UART_LSR_THRE == (lsr & SQ_UART_LSR_THRE))
		printf("\nSq UART host: Transmit FIFO is empty\n");
	if (SQ_UART_LSR_BI == (lsr & SQ_UART_LSR_BI))
		printf("\nSq UART host: Break Interrupt indicator\n");
	if (SQ_UART_LSR_FE == (lsr & SQ_UART_LSR_FE))
		printf("\nSq UART host: Framming Error (FE) indicator\n");
	if (SQ_UART_LSR_PE == (lsr & SQ_UART_LSR_PE))
		printf("\nSq UART host: Parity Error indicator\n");
	if (SQ_UART_LSR_OE == (lsr & SQ_UART_LSR_OE))
		printf("\nSq UART host: Overrun Error indicator\n");
	if (SQ_UART_LSR_DR == (lsr & SQ_UART_LSR_DR))
		printf("\nSq UART host: Data Ready indicator\n");
}

static int
sq_uart_compare_memory(u8 *mem, u8 *cmpr_mem, u32 size,
			  int skip_cmpr_result)
{
	int i;
	u32 *mem32 = (u32 *)mem;
	u32 *cmpr_mem32 = (u32 *)cmpr_mem;
	int err_flag = 0;

	for (i = 0; i < (size >> 2); i++) {
		if (mem32[i] != cmpr_mem32[i]) {
			err_flag |= -1;
			if (!skip_cmpr_result) {
				printf("\nWord %d, 0x%08x (0x%08x) != 0x%08x (0x%08x)", i, mem32[i], &mem32[i], 
				       cmpr_mem32[i], &cmpr_mem32[i]);
			}
		}
	}
	printf("\n");
	return err_flag;
}

static void
sq_uart_init_environment(int dma_req_en)
{
	/* Reset the variables */
	sq_uart_rx_complete_flag = 0;
	sq_uart_tx_complete_flag = 0;
	sq_uart_pattern_buf_idx = 0;
	sq_uart_cmpr_buf_idx = 0;

	/* Clear the buffers */
	memset((char *)sq_uart_pattern_buf, 0x0, PATTERN_BUF_SIZE);     
	memset((char *)sq_uart_cmpr_buf, 0x0, PATTERN_BUF_SIZE);     


	/* Read & clear the interrupt status */
	(void)sq_uart_read(SQ_UART_IIR, sq_uart_base);	

	/* Use external clock */
	sq_uart_write(SQ_UART_CTRL, 0x1, sq_uart_base);

	/* Setup the FIFO control */
	sq_uart_write(SQ_UART_FCR,
			 SQ_UART_FCR_THRESH_8 |
			 SQ_UART_FCR_TX_CLR |
			 SQ_UART_FCR_RX_CLR |
			 SQ_UART_FCR_FIFO_EN |
			 (dma_req_en << 3),
			 sq_uart_base);

	/* Setup the Line control */
	sq_uart_write(SQ_UART_LCR,
			 SQ_UART_LCR_DIV_LATCH_DIS |
			 SQ_UART_LCR_BRK_DIS |
			 SQ_UART_LCR_STICK_DIS |
			 SQ_UART_LCR_PARITY_ODD |
			 SQ_UART_LCR_PARITY_DIS |
			 SQ_UART_LCR_STOP_1 |
			 SQ_UART_LCR_NBITS_8,
			 sq_uart_base);
}

//FIXME

static int 
sq_uart_hwdma_panther7_hdma(int autotest)
{
	/* Initialize the test environment */
	sq_uart_init_environment(1);

	/* Enable specify interrupts */
	sq_uart_write(SQ_UART_IER,
			 SQ_UART_IE_MS_DIS |
			 SQ_UART_IE_LS_EN |
			 SQ_UART_IE_TBE_DIS |
			 SQ_UART_IE_RBR_DIS,
			 sq_uart_base);

	/* Make the test pattern */
	sq_uart_make_test_pattern(sq_uart_pattern_buf, PATTERN_BUF_SIZE);

	/* Configure the hardware dma settng of HDMA for tx channels */
	sq_disable_dma(sq_uart_tx_dma_ch_num);
	sq_set_dma_mode(sq_uart_tx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_uart_tx_dma_ch_num, sq_uart_tx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_uart_tx_dma_ch_num, sq_uart_dma_burst);
	sq_set_dma_source_address(sq_uart_tx_dma_ch_num, (u32)sq_uart_pattern_buf);
	sq_set_dma_destination_address(sq_uart_tx_dma_ch_num, SQ_UART_THR+sq_uart_base);
	sq_set_dma_source_direction(sq_uart_tx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_destination_direction(sq_uart_tx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_data_size(sq_uart_tx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_transfer_count(sq_uart_tx_dma_ch_num, PATTERN_BUF_SIZE);
	sq_set_dma_slice_count(sq_uart_tx_dma_ch_num, 8);
	sq_set_dma_page_number(sq_uart_tx_dma_ch_num, 1);
	sq_set_dma_buffer_size(sq_uart_tx_dma_ch_num, PATTERN_BUF_SIZE);

	/* Configure the hardware dma settng of HDMA for rx channels */
	sq_disable_dma(sq_uart_rx_dma_ch_num);
	sq_set_dma_mode(sq_uart_rx_dma_ch_num, SQ_DMA_MODE_SLICE);
	sq_set_dma_ext_hdreq_number(sq_uart_rx_dma_ch_num, sq_uart_rx_dma_ext_hdreq);
	sq_set_dma_burst_type(sq_uart_rx_dma_ch_num, sq_uart_dma_burst);
	sq_set_dma_source_address(sq_uart_rx_dma_ch_num, SQ_UART_RBR+sq_uart_base);
	sq_set_dma_destination_address(sq_uart_rx_dma_ch_num, (u32)sq_uart_cmpr_buf);
	sq_set_dma_source_direction(sq_uart_rx_dma_ch_num, SQ_DMA_DIR_FIXED);
	sq_set_dma_destination_direction(sq_uart_rx_dma_ch_num, SQ_DMA_DIR_INCR);
	sq_set_dma_data_size(sq_uart_rx_dma_ch_num, SQ_DMA_DATA_BYTE);
	sq_set_dma_transfer_count(sq_uart_rx_dma_ch_num, PATTERN_BUF_SIZE);
	sq_set_dma_slice_count(sq_uart_rx_dma_ch_num, 8);
	sq_set_dma_page_number(sq_uart_rx_dma_ch_num, 1);
	sq_set_dma_buffer_size(sq_uart_rx_dma_ch_num, PATTERN_BUF_SIZE);

	/* Enable the dma to run*/
	sq_enable_dma(sq_uart_tx_dma_ch_num);
	sq_enable_dma(sq_uart_rx_dma_ch_num);

	/* Wait for transfer to be complete */
	if (sq_wait_for_int(&sq_uart_rx_complete_flag, 80)) {
		printf("Timeout\n");
		return -1;
	}

	if (-1 == sq_uart_rx_complete_flag)
		return -1; 
	else
		/* Compare the memory */
		return sq_uart_compare_memory(sq_uart_pattern_buf, sq_uart_cmpr_buf, PATTERN_BUF_SIZE,
						 autotest);
}

static void 
sq_uart_tx_dma_page_interrupt(void *data)
{
	sq_uart_tx_complete_flag = 1;
}

static void 
sq_uart_rx_dma_page_interrupt(void *data)
{
	sq_uart_rx_complete_flag = 1;
}











