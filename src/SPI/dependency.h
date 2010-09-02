#include <platform.h>
#include <irqs.h>

#ifndef SQ_SPI0
#error "SPI IP base is not defined"
#endif

#ifndef SQ_SPI1
#define SQ_SPI1 SQ_SPI0
#define SOCLE_SPI_IP_1_TEST 0
#else
#define SOCLE_SPI_IP_1_TEST 1
#endif

#ifndef SQ_INTC_SPI0
#error "SPI IRQ is not defined"
#endif

#ifndef SQ_INTC_SPI1
#define SQ_INTC_SPI1 SQ_INTC_SPI0
#endif

//20080201 jsho modify, for menu control
#ifdef CONFIG_PANTHER7_HDMA
#define SOCLE_SPI_HWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_SPI_HWDMA_PANTHER7_HDMA_TEST 0
#endif

#define SOCLE_SPI_EEPROM_0_TEST 0 
#define SOCLE_SPI_EEPROM_1_TEST 0
#define SOCLE_SPI_MARVEL_WIFI_0_TEST 0
#define SOCLE_SPI_MARVEL_WIFI_1_TEST 0
#define SOCLE_SPI_TSC2000_0_TEST 0
#define SOCLE_SPI_TSC2000_1_TEST 0
#define SOCLE_SPI_ADS7846_0_TEST 0
#define SOCLE_SPI_ADS7846_1_TEST 0
#define SOCLE_SPI_MAX1110_0_TEST 0
#define SOCLE_SPI_MAX1110_1_TEST 0
#define SPI_SLAVE_TEST 0



#define MAX1110_USE_PWM_NUM	1
#define MAX1110_DEFAULT_PRE_SCL		0x0		// default Prescale Factor
#define MAX1110_DEFAULT_HRC			0x100	// default HRC
#define MAX1110_DEFAULT_LRC			0x100	// default LRC

