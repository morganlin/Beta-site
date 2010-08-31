#ifndef _platform_H_
#define _platform_H_

#include <io.h>
#include <type.h>
#include <interrupt.h>

#define INTC_MAX_IRQ           		 	31			// interrupt source number
#define INTC_MAX_FIQ				31			// interrupt source number
#define MAX_SYSTEM_IRQ				31			// max. system interrupt source number

#define INTC_CONNECT_SYS_INT		SYS_INT_7	// interrupt controller connect which MIPS IP


#define SOCLE_MAIN_OSC				12000000
#define SOCLE_UART_OSC				11059200
#define SOCLE_CONSOLE_BAUD_RATE		        115200


#define MAPPING_MASK				0x1FFFFFFF
#define CACHE_BASE					0x60000000	// Cache Kenel Mode

#define KSEG_TO_PHY(x)				( ((int) (x)) & MAPPING_MASK )
#define PHY_TO_CACHE(x)				( ( ((int) (x)) & MAPPING_MASK ) | CACHE_BASE )

// memory mapped address
#define	SOCLE_MEMORY_ADDR_START			0x40000000
#define	SOCLE_MEMORY_ADDR_SIZE			0x04000000

#define	SOCLE_MEMORY_BANKS		2

#define SOCLE_MM_DDR_SDR_BANK0			0x00000000
#define SOCLE_MM_DDR_SDR_BANK1			0x40000000


#define	SOCLE_NORFLASH_BANKS		1

#define SOCLE_NOR_FLASH_BANK0        0x10000000	

// AHB device base address define
#define SOCLE_BRI0			0x19000000	
#define SOCLE_LCD0			0x18180000
#define SOCLE_ARBITER1		        0x18160000
#define SOCLE_NAND0		        0x18140000	
#define SOCLE_VIP0			0x18120000
#define SOCLE_VOP0			0x18100000
#define SOCLE_OTG1			0x180E0000
#define SOCLE_HDMA0		        0x180C0000
#define SOCLE_SDHC0			0x180A0000
#define SOCLE_OTG0			0x18080000
#define SOCLE_MAC0			0x18060000
#define SOCLE_INTC0			0x18040000
#define SOCLE_ARBITER0		        0x18020000
#define SOCLE_SDRSTMC0		        0x18000000

// APB device base address define
#define SOCLE_ADC0			0x19120000
#define SOCLE_SCU0			0x19100000
#define SOCLE_WDT0			0x190F0000
#define SOCLE_RTC0			0x190E0000
#define SOCLE_GPIO3			0x190D0000
#define SOCLE_GPIO2			0x190C0000
#define SOCLE_GPIO1			0x190B0000
#define SOCLE_GPIO0			0x190A0000
#define SOCLE_TIMER_PWM0	        0x19090000
#define SOCLE_TIMER0		        0x19080000
#define SOCLE_I2S0			0x19060000
#define SOCLE_I2C0			0x19050000
#define SOCLE_SPI1			0x19040000
#define SOCLE_SPI0			0x19030000
#define SOCLE_UART2			0x19020000
#define SOCLE_UART1			0x19010000
#define SOCLE_UART0			0x19000000

#define SOCLE_DTCM                              0x20040000
#define SOCLE_ITCM                              0x20000000//0x00000000
#define SOCLE_SLAVE_DTCM                        0x80000000
#define SOCLE_INT_DP_SRAM                       0xFFFF0000
#define SOCLE_AHB0_MDDRMC                       0x1800c000

//Use PANTHER7 DMA
#define PANTHER7_HDMA0	SOCLE_HDMA0

#define	PHY_IC_PLUS

// GPIO
#define SOCLE_GPIO_WITH_INT
#define SOCLE_GPIO_FLASH_LED_TEST	0
#define GPIO_FLASH_LED_PA	1
#define GPIO_FLASH_LED_PB	0

// Memory Dependencys----- Start -----
#define SOCLE_FPGA_SRAM_STAR    0x1cb20000
#define SOCLE_FPGA_SRAM_SIZE    0x00200000

#define SOCLE_MM_DDR_SDR_ADDR_START  0x1cb20000
#define SOCLE_MM_DDR_SDR_ADDR_SIZE   0x00200000

#define SOCLE_SRAM_BANK0_START	0x1CD20000
#define SOCLE_SRAM_BANK1_START	0x1CD30000

#define SOCLE_SRAM_BANK0_SIZE	0x2040  
#define SOCLE_SRAM_BANK1_SIZE	0x2040


#define SRAM_BANK0_START        SOCLE_SRAM_BANK0_START
#define SRAM_BANK1_START        SOCLE_SRAM_BANK1_START

#define SRAM_BANK0_SIZE	        SOCLE_SRAM_BANK0_SIZE
#define SRAM_BANK1_SIZE	        SOCLE_SRAM_BANK1_SIZE

#define SOCLE_ITCM_ADDR_STAR                    SOCLE_ITCM
#define SOCLE_ITCM_ADDR_SIZE                    0x00040000//256K

#define SOCLE_DTCM_ADDR_STAR                    SOCLE_DTCM
#define SOCLE_DTCM_ADDR_SIZE                    0x00020000//128K

#define SOCLE_ANOTHER_DTCM_ADDR                 SOCLE_SLAVE_DTCM 
#define SOCLE_ANOTHER_DTCM_SIZE                 0x00020000//128K

#define SOCLE_INT_DP_SRAM_ADDR                  SOCLE_INT_DP_SRAM
#define SOCLE_INT_DP_SRAM_SIZE                  0x2000//8k
#define SOCLE_MM_DDR_SDR_CONFIG_ADDR            SOCLE_AHB0_MDDRMC
// Memory Dependencys----- End -----

#endif //_platform_H_
