#include <platform.h>
#include <irqs.h>

#ifndef SOCLE_TIMER0
#error "Timer IP base address is not defined"
#endif

#ifndef SOCLE_INTC_TMR0
#error "Tiimer IRQ0 is not defined"
#endif

#ifndef SOCLE_INTC_TMR1
#error "Tiimer IRQ1 is not defined"
#endif

#ifndef SOCLE_INTC_TMR2
#error "Tiimer IRQ2 is not defined"
#endif


#if defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_PC9220) || defined(CONFIG_PC9223) 
#define SOCLE_TIMER_CHAIN_MODE_1_TEST 1
#define SOCLE_TIMER_CHAIN_MODE_2_TEST 1
#elif defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD)
#define SOCLE_TIMER_CHAIN_MODE_1_TEST 0
#define SOCLE_TIMER_CHAIN_MODE_2_TEST 0
#else
#define SOCLE_TIMER_CHAIN_MODE_1_TEST 0
#define SOCLE_TIMER_CHAIN_MODE_2_TEST 0
#endif
