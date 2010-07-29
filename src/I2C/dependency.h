#include <platform.h>
#include <irqs.h>

#ifndef SOCLE_I2C0
#error "I2C IP base address is not defined"
#endif

#ifndef SOCLE_I2C1
#define SOCLE_I2C1 SOCLE_I2C0
#define SOCLE_I2C1_TEST 0
#else
#define SOCLE_I2C1_TEST 1
#endif

#ifndef SOCLE_I2C2
#define SOCLE_I2C2 SOCLE_I2C0
#define SOCLE_I2C2_TEST 0
#else
#define SOCLE_I2C2_TEST 1
#endif

#ifndef SOCLE_INTC_I2C0
#error "I2C IRQ is not defined"
#endif

#ifndef SOCLE_INTC_I2C1
#define SOCLE_INTC_I2C1 SOCLE_INTC_I2C0
#endif

#ifndef SOCLE_INTC_I2C2
#define SOCLE_INTC_I2C2 SOCLE_INTC_I2C0
#endif

#if defined(CONFIG_PC7210) || defined(CONFIG_PC9002) || defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD) || defined(CONFIG_PC9223)
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 0
#else
#define SOCLE_I2C_AT24C02B_EEPROM_TEST 1
#endif

#if defined(CONFIG_PC9223)
#define SOCLE_I2C_RM24C16_EEPROM_TEST 1
#else
#define SOCLE_I2C_RM24C16_EEPROM_TEST 0
#endif

#if defined(CONFIG_MSMV) || defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD)
#define SOCLE_I2C_TPS62353_TEST 1

#define TPS62353_IP0	1
#define TPS62353_IP1	1
#define TPS62353_IP2	1

#define TPS62353_TYPE0	0		//0:VSEL PIN LOW (Change valtage from register VSEL0)	: default valtage 1.0V
#define TPS62353_TYPE1	0
#define TPS62353_TYPE2	1		//1:VSEL PIN HIGH (Change valtage from register VSEL1)	: default valtage 1.2V

#define SOCLE_I2C_F75363_FAN_TEST 1
#define SOCLE_I2C_CMA3000_D0X_TEST 1
#define SOCLE_I2C_F75111_TEST 1

#else
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

#endif

#if defined(CONFIG_PC9223)
#define SOCLE_I2C_SI4703_FM_TUNER_TEST 0
#else
#define SOCLE_I2C_SI4703_FM_TUNER_TEST 1
#endif


