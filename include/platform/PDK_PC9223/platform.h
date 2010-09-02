#ifndef _platform_H_
#define _platform_H_

#include <io.h>
#include <type.h>
#include <interrupt.h>

#define INTC_MAX_IRQ           		 	31			// interrupt source number
#define INTC_MAX_FIQ				31			// interrupt source number
#define MAX_SYSTEM_IRQ				31			// max. system interrupt source number

#define INTC_CONNECT_SYS_INT		SYS_INT_7	// interrupt controller connect which MIPS IP


#define SQ_MAIN_OSC				12000000
#define SQ_UART_OSC				11059200
#define SQ_CONSOLE_BAUD_RATE		        115200


#define MAPPING_MASK				0x1FFFFFFF
#define CACHE_BASE					0x60000000	// Cache Kenel Mode

#define KSEG_TO_PHY(x)				( ((int) (x)) & MAPPING_MASK )
#define PHY_TO_CACHE(x)				( ( ((int) (x)) & MAPPING_MASK ) | CACHE_BASE )

// memory mapped address
#define	SQ_MEMORY_ADDR_START			0x40000000
#define	SQ_MEMORY_ADDR_SIZE			0x04000000

#define	SQ_MEMORY_BANKS		2

#define SQ_MM_DDR_SDR_BANK0			0x00000000
#define SQ_MM_DDR_SDR_BANK1			0x40000000


#define	SQ_NORFLASH_BANKS		1

#define SQ_NOR_FLASH_BANK0        0x10000000	

// AHB device base address define
#define SQ_BRI0			0x19000000	
#define SQ_LCD0			0x18180000
#define SQ_ARBITER1		        0x18160000
#define SQ_NAND0		        0x18140000	
#define SQ_VIP0			0x18120000
#define SQ_VOP0			0x18100000
#define SQ_OTG1			0x180E0000
#define SQ_HDMA0		        0x180C0000
#define SQ_SDHC0			0x180A0000
#define SQ_OTG0			0x18080000
#define SQ_MAC0			0x18060000
#define SQ_INTC0			0x18040000
#define SQ_ARBITER0		        0x18020000
#define SQ_SDRSTMC0		        0x18000000

// APB device base address define
#define SQ_ADC0			0x19120000
#define SQ_SCU0			0x19100000
#define SQ_WDT0			0x190F0000
#define SQ_RTC0			0x190E0000
#define SQ_GPIO3		0x190D0000
#define SQ_GPIO2		0x190C0000
#define SQ_GPIO1		0x190B0000
#define SQ_GPIO0		0x190A0000
#define SQ_TIMER_PWM0	        0x19090000
#define SQ_TIMER0		0x19080000
#define SQ_I2S0			0x19060000
#define SQ_I2C0			0x19050000
#define SQ_SPI1			0x19040000
#define SQ_SPI0			0x19030000
#define SQ_UART2		0x19020000
#define SQ_UART1		0x19010000
#define SQ_UART0		0x19000000

#define SQ_DTCM                              0x20040000
#define SQ_ITCM                              0x20000000
#define SQ_SLAVE_DTCM                        0x80000000
#define SQ_INT_DP_SRAM                       0xFFFF0000
#define SQ_AHB0_MDDRMC                       0x1800c000

//Use PANTHER7 DMA
#define PANTHER7_HDMA0	SQ_HDMA0

#define	PHY_IC_PLUS

// GPIO
#define SQ_GPIO_WITH_INT
#define SQ_GPIO_FLASH_LED_TEST	0
#define GPIO_FLASH_LED_PA	1
#define GPIO_FLASH_LED_PB	0

// Memory Dependencys----- Start -----
#define SQ_FPGA_SRAM_STAR    0x1cb20000
#define SQ_FPGA_SRAM_SIZE    0x00200000

#define SQ_MM_DDR_SDR_ADDR_START  0x1cb20000
#define SQ_MM_DDR_SDR_ADDR_SIZE   0x00200000

#define SQ_SRAM_BANK0_START	0x1CD20000
#define SQ_SRAM_BANK1_START	0x1CD30000

#define SQ_SRAM_BANK0_SIZE	0x2040  
#define SQ_SRAM_BANK1_SIZE	0x2040


#define SRAM_BANK0_START        SQ_SRAM_BANK0_START
#define SRAM_BANK1_START        SQ_SRAM_BANK1_START

#define SRAM_BANK0_SIZE	        SQ_SRAM_BANK0_SIZE
#define SRAM_BANK1_SIZE	        SQ_SRAM_BANK1_SIZE

#define SQ_ITCM_ADDR_STAR                    SQ_ITCM
#define SQ_ITCM_ADDR_SIZE                    0x00040000//256K

#define SQ_DTCM_ADDR_STAR                    SQ_DTCM
#define SQ_DTCM_ADDR_SIZE                    0x00020000//128K

#define SQ_ANOTHER_DTCM_ADDR                 SQ_SLAVE_DTCM 
#define SQ_ANOTHER_DTCM_SIZE                 0x00020000//128K

#define SQ_INT_DP_SRAM_ADDR                  SQ_INT_DP_SRAM
#define SQ_INT_DP_SRAM_SIZE                  0x2000//8k
#define SQ_MM_DDR_SDR_CONFIG_ADDR            SQ_AHB0_MDDRMC
// Memory Dependencys----- End -----

#endif //_platform_H_
