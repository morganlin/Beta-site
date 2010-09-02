#include <platform.h>
#include <irqs.h>

#ifndef SQ_I2C0
#error "I2C IP base address is not defined"
#endif

#ifndef SOCLE_I2C1
#define SOCLE_I2C1 SQ_I2C0
#define SOCLE_I2C1_TEST 0
#else
#define SOCLE_I2C1_TEST 1
#endif

#ifndef SOCLE_I2C2
#define SOCLE_I2C2 SQ_I2C0
#define SOCLE_I2C2_TEST 0
#else
#define SOCLE_I2C2_TEST 1
#endif

#ifndef SQ_INTC_I2C0
#error "I2C IRQ is not defined"
#endif

#ifndef SOCLE_INTC_I2C1
#define SOCLE_INTC_I2C1 SQ_INTC_I2C0
#endif

#ifndef SOCLE_INTC_I2C2
#define SOCLE_INTC_I2C2 SQ_INTC_I2C0
#endif

#if defined(CONFIG_PC9223)
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 0
#else
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 1
#endif

#if defined(CONFIG_PC9223)
#define SOCLE_I2C_RM24C16_EEPROM_TEST 1
#else
#define SOCLE_I2C_RM24C16_EEPROM_TEST 0
#endif

#define SOCLE_I2C_TPS62353_TEST 0

#define TPS62353_IP0	0	
#define TPS62353_IP1	0
#define TPS62353_IP2	0

#define TPS62353_TYPE0	0
#define TPS62353_TYPE1	0
#define TPS62353_TYPE2	0

#define SOCLE_I2C_F75363_FAN_TEST 0
#define SOCLE_I2C_CMA3000_D0X_TEST 0
#define SOCLE_I2C_F75111_TEST 0

#if defined(CONFIG_PC9223)
#define SOCLE_I2C_SI4703_FM_TUNER_TEST 0
#else
#define SOCLE_I2C_SI4703_FM_TUNER_TEST 1
#endif


