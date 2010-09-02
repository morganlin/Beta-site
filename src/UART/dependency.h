#include <platform.h>
#include <irqs.h>

/* Set the IP's base address */
#ifndef SQ_UART0
#error "UART IP base address is not defined"
#endif

#ifndef SQ_UART0
#define SQ_UART0 SOCLE_UART0
#define SOCLE_UART_IP_0_TEST 0
#else
#define SOCLE_UART_IP_0_TEST 1
#endif

#ifndef SQ_UART1
#define SQ_UART1 SQ_UART0
#define SOCLE_UART_IP_1_TEST 0
#else
#define SOCLE_UART_IP_1_TEST 1
#endif

#ifndef SQ_UART2
#define SQ_UART2 SQ_UART0
#define SOCLE_UART_IP_2_TEST 0
#else
#define SOCLE_UART_IP_2_TEST 1
#endif

#ifndef SOCLE_UART3
#define SOCLE_UART3 SQ_UART0
#define SOCLE_UART_IP_3_TEST 0
#else
#define SOCLE_UART_IP_3_TEST 1
#endif

/* Set the IP's irq */
#ifndef SQ_INTC_UART0
#ifdef SQ_INTC_UART
#define SQ_INTC_UART0 SQ_INTC_UART
#else
#error "UART IRQ is not defined"
#endif
#endif

#ifndef SQ_INTC_UART1
#define SQ_INTC_UART1 SQ_INTC_UART0
#endif

#ifndef SQ_INTC_UART2
#define SQ_INTC_UART2 SQ_INTC_UART0
#endif

#ifndef SQ_INTC_UART3
#define SQ_INTC_UART3 SQ_INTC_UART0
#endif

/* Configure the test menu */
#if defined(CONFIG_PANTHER7_HDMA)
#define SOCLE_UART_HWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_UART_HWDMA_PANTHER7_HDMA_TEST 0
#endif

#ifdef CONFIG_DMAC
#define SOCLE_UART_DMAC_TEST_IMPLEMENT 1
#else 
#define SOCLE_UART_DMAC_TEST_IMPLEMENT 0
#endif

#ifdef CONFIG_UART_DISP
#undef SOCLE_UART_IP_0_TEST
#define SOCLE_UART_IP_0_TEST 0
#endif

#define SOCLE_UART_GPS_0_TEST 0 
#define SOCLE_UART_GPS_1_TEST 0
#define SOCLE_UART_GPS_2_TEST 0
#define SOCLE_UART_GPS_3_TEST 0

#if defined(CONFIG_PC9223)
#define SOCLE_UART_EXT_0_TEST 1
#define SOCLE_UART_EXT_1_TEST 0
#define SOCLE_UART_EXT_2_TEST 0
#define SOCLE_UART_EXT_3_TEST 0
#else
#define SOCLE_UART_EXT_0_TEST 1 
#define SOCLE_UART_EXT_1_TEST 1
#define SOCLE_UART_EXT_2_TEST 0
#define SOCLE_UART_EXT_3_TEST 0
#endif
