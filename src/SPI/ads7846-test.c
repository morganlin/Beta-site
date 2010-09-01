//ADS7846 Test Function 
#include "spi-master.h"
#include "regs-ads7846.h"
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

#ifdef CONFIG_INTC
#include "../../src/Interrupt/interrupt.h"
#else
#include "../.././src/VIC/vic-regs.h"
#endif

volatile unsigned int touch_count;
volatile unsigned int touch_flag;

static int
socle_spi_ads7846_read(u8 addr, u8 *buf, u32 len)
{
	u8 tx_buf[1] = {0};

	tx_buf[0] = addr;
	
	if (socle_spi_transfer(tx_buf, buf, SET_TX_RX_LEN(1, len)))
		return -1;
	return 0;
}

int irq_tmp=0;

/* 
Driver design note:
interrupt&pressed=0=>disable ts irq,timer=1ms,get xy,pressed=1,enable ts irq,timer=1ms
interrupt&pressed=1=>check if PSM,pressed=0/1

kernel timer routine:only when pressed=1&PSM=0=>pressed=0 & reenable ts irq, no longer to set the routine
otherwise, set the routine repeatedly, the routine is in charge of get_xy & button=UP/DOWN

ts_isr is only a trigger event to setup the timer routine

STS have to be checked before get_xy!
*/
void
ads7846_isr (void *pparam)
{
//printf("ads7846_isr\n");
	
	//ADS7846 IRQ disable
	#ifdef CONFIG_INTC
		iowrite32(ioread32(INTC0_IECR)&(~(1<<ADS7846_INTR)), INTC0_IECR);
	#else
		//iowrite32((0x1 << ADS7846_INTR), SOCLE_VICINTENCLEAR);
		iowrite32((0x1 << ADS7846_INTR), SOCLE_VICINTENCLEAR);
		free_irq(ADS7846_INTR);
	#endif
	
	touch_flag=1;
}

extern int
socle_spi_ads7846_touch(int autotest)
{
	u8 divisor;
	u8 xy[4];	
	touch_count=0;
	touch_flag=0;
	int cmd;

	/* Reset SPI controller */
	socle_spi_write(
			socle_spi_read(SOCLE_SPI_FWCR) | 
			SOCLE_SPI_SOFT_RST,
			SOCLE_SPI_FWCR);

	/* Configure SPI controller */
	socle_spi_write(
#if defined (CONFIG_PC9220) || defined (CONFIG_PC9223) || defined (CONFIG_MDK3D) || defined (CONFIG_MDKFHD)
			SOCLE_SPI_MASTER_SIGNAL_CTL_HW |
			SOCLE_SPI_MASTER_SIGNAL_ACT_NO |		
			SOCLE_SPI_MODE_MASTER |
#endif			
			SOCLE_SPI_SOFT_N_RST |
			SOCLE_SPI_TXRX_N_RUN |
			SOCLE_SPI_CLK_IDLE_AST |
			SOCLE_SPI_TXRX_SIMULT_DIS |
			SOCLE_SPI_CPOL_0 |
			SOCLE_SPI_CPHA_0 |
			SOCLE_SPI_TX_MSB_FIRST |
			SOCLE_SPI_OP_NORMAL,
			SOCLE_SPI_FWCR);


	/* Enable SPI interrupt */
	socle_spi_write(
			SOCLE_SPI_IER_RXFIFO_INT_EN |
			SOCLE_SPI_IER_RXFIFO_OVR_INT_EN |
			SOCLE_SPI_IER_RX_COMPLETE_INT_EN,
			SOCLE_SPI_IER);

	/* Configure FIFO and clear Tx & Rx FIFO */
	socle_spi_write(
			SOCLE_SPI_RXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_TXFIFO_INT_TRIGGER_LEVEL_4 |
			SOCLE_SPI_RXFIFO_CLR |
			SOCLE_SPI_TXFIFO_CLR,
			SOCLE_SPI_FCR);
     
	/* Set the SPI slaves select and characteristic control register */
	divisor = socle_spi_calculate_divisor(2000000); /* 2.0 MHz clock rate */

	socle_spi_write(
			SOCLE_SPI_CHAR_LEN_8 |
#if defined(CONFIG_MDK3D) || defined (CONFIG_MDKFHD)
			SOCLE_SPI_SLAVE_SEL_1 |
#else
			SOCLE_SPI_SLAVE_SEL_0 |
#endif
			SOCLE_SPI_CLK_DIV(divisor),
			SOCLE_SPI_SSCR);

	/* Config SPI clock delay */
	socle_spi_write(
			SOCLE_SPI_PBTXRX_DELAY_NONE |
			SOCLE_SPI_PBCT_DELAY_NONE |
			SOCLE_SPI_PBCA_DELAY_1_2,
			SOCLE_SPI_DLYCR);

	/* Set per char length */
	socle_spi_set_current_mode(MODE_CHAR_LEN_8);

	// enable interrupt
#if defined (CONFIG_PC9223)
	socle_scu_dev_enable(SOCLE_DEVCON_EXT_INT0);
#endif
	request_irq(ADS7846_INTR, ads7846_isr, null);



	printf("=======================\n");
	printf("====Please Touch Panel ====\n");
	printf("=======================\n");
	
	printf("     y     x    z1    z2\n");
	while(1)
	{
		if(touch_flag==1)
		{

			cmd = ADS7846_START | ADS7846_MODE_12BIT | ADS7846_DIFFERENTIAL | ADS7846_POWER_DOWN_CONVERSION;
			socle_spi_ads7846_read((cmd | ADS7846_SINGLE_POS_Y), &xy[0],1);
	                socle_spi_ads7846_read((cmd | ADS7846_SINGLE_POS_X), &xy[1],1);
        	        socle_spi_ads7846_read((cmd | ADS7846_SINGLE_POS_Z1), &xy[2],1);
                	socle_spi_ads7846_read((cmd | ADS7846_SINGLE_POS_Z2), &xy[3],1);

	                //printf("%6x%6x%6x%6x\n", xy[0], xy[1], xy[2], xy[3]);
	                printf("%d %d %d %d\n", xy[0], xy[1], xy[2], xy[3]);

			touch_flag=0;
			touch_count++;
			//ADS7846 IRQ enable			
		#ifdef CONFIG_INTC
			iowrite32(ioread32(INTC0_IECR)|(1<<ADS7846_INTR), INTC0_IECR);		
		#else
		#if 0
			iowrite32(ioread32(SOCLE_VICINTSELECT) & ~(0x1 << ADS7846_INTR),
                                SOCLE_VICINTSELECT);
		#else
			request_irq(ADS7846_INTR, ads7846_isr, null);
		#endif
		#endif
		}
		if(touch_count == 100)
			break;
	}
	
	// enable interrupt
#if defined (CONFIG_PC9223)
	socle_scu_dev_disable(SOCLE_DEVCON_EXT_INT0);
#endif
	free_irq(ADS7846_INTR);

	return 0;
}
