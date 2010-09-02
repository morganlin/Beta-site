#ifdef CONFIG_ROM

extern int main(void);

extern int
TestEntry(void)
{
	return main();
}

#else /* CONFIG_ROM */

#include <global.h>
#include <genlib.h>
#include <irqs.h>
#include <platform.h>
#include <test_item.h>
#include <dma/dma.h>
#include <revision.h>
#include <sq-scu.h>

#ifdef CONFIG_GPIO
#include <GPIO/gpio.h>
#endif

#ifdef CONFIG_MP_GPIO
#include <MP_GPIO/mp-gpio.h>
#endif 

#ifdef CONFIG_PWM
#include <PWM/pwmt-regs.h>
#endif

#ifdef SEMI_HOST_FUNCTION
#include <epios.h>	//For EPI SemiHost HIF Function
#endif

#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

#if 0
#define BENCHMARK_TESTING
#endif

#define SCU_AHB_MODE (0x1<<6)

extern struct test_item_container main_test_item_container;
int cpu_clk_mhz;
char cpu_type[] = CPU_TYPE;
int sq_memory_addr_start = SQ_MEMORY_ADDR_START;

/*============================================*/

#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
static char f_name[]={':','t','t','\0'};
static u32_t sysopen_array[]={(u32_t)f_name,1,3};
static int
semihost_init(u32_t* array)
{
	int c;
	__asm__ __volatile__ (
		"mov r0,#1 \n"
		"mov r1,%[input]\n"
		"swi #0x123456\n"
		"mov %[output],r0"
		:[output] "=r" (c)
		:[input] "r" (array));
	return c;
}
#endif	//end SEMI_HOST_FUNCTION_ARM

static void
socle_init_interrupt(void)
{
	// set upt default handle to system (CPU) interrupt
#if defined(CPU_ARCH_MIPS)
	initSysIntTrap();
#endif
	// initial interrupt controller
	init_irq_controller();
	init_fiq_controller();

#if defined(CPU_ARCH_MIPS)
	/* do not enable INT 6, this is for internal timer interrupt source (for 4Kc) */
	EnableInt_x(M_StatusIM0 | M_StatusIM1 | M_StatusIM2 | M_StatusIM3 |
	            M_StatusIM4 | M_StatusIM5 | M_StatusIM7);
#endif
	enable_interrupts();
}


#if defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)

#define FCR_FIFO_EN     0x01            /* Fifo enable */
#define FCR_RXSR        0x02            /* Receiver soft reset */
#define FCR_TXSR        0x04            /* Transmitter soft reset */

#define MCR_DTR         0x01
#define MCR_RTS         0x02
#define MCR_DMA_EN      0x04
#define MCR_TX_DFR      0x08

#define LCR_WLS_MSK     0x03            /* character length slect mask */
#define LCR_WLS_5       0x00            /* 5 bit character length */
#define LCR_WLS_6       0x01            /* 6 bit character length */
#define LCR_WLS_7       0x02            /* 7 bit character length */
#define LCR_WLS_8       0x03            /* 8 bit character length */
#define LCR_STB         0x04            /* Number of stop Bits, off = 1, on = 1.5 or 2) */
#define LCR_PEN         0x08            /* Parity eneble */
#define LCR_EPS         0x10            /* Even Parity Select */
#define LCR_STKP        0x20            /* Stick Parity */
#define LCR_SBRK        0x40            /* Set Break */
#define LCR_BKSE        0x80            /* Bank select enable */

/* useful defaults for LCR */
#define LCR_8N1		0x03

#define LCRVAL	LCR_8N1                                  /* 8 data, 1 stop, no parity */
#define MCRVAL	(MCR_DTR | MCR_RTS)                      /* RTS/DTR */
#define FCRVAL	(FCR_FIFO_EN | FCR_RXSR | FCR_TXSR )     /* Clear & enable FIFOs */


static void
ns16550_init(int baud_rate)
{
#include "UART/dependency.h"
	int baud_divisor = sq_scu_uart_clock_get (0) / 16 / baud_rate;

	volatile unsigned long *p;

	p = (unsigned long *) SQ_UART0;

	*(p + 1) = 0x0;
	*(p + 3) = LCR_BKSE | LCRVAL;
	/* MSB first */
	*(p + 1) = (baud_divisor >> 8) & 0xff;
	*p = baud_divisor & 0xff;
	*(p + 3) = LCRVAL;
	*(p + 4) = MCRVAL;
	*(p + 2) = FCRVAL;
}
#endif


static void
socle_init_service(void)
{
	socle_init_interrupt();

	socle_init_dma();

#ifdef CONFIG_GPIO
        socle_init_gpio_irq();
#endif

#ifdef CONFIG_MP_GPIO
        socle_mp_gpio_init_irq();
#endif

#ifdef CONFIG_PWM
	socle_init_pwmt();
#endif
}


extern int
TestEntry(void)
{
	int result = 0;

#if defined(SEMI_HOST_FUNCTION_ARM) && defined(UART_DEBUG)

#if defined(CONFIG_PC9223)
        	sq_scu_dev_enable(SQ_DEVCON_UART0);
#endif

	ns16550_init(SQ_CONSOLE_BAUD_RATE);		// initiate UART console
#endif

	socle_init_service();

#ifdef SEMI_HOST_FUNCTION
	HIF_debug_print_init();
#endif

#if 1
#if defined(SEMI_HOST_FUNCTION_ARM) && !defined(UART_DEBUG)
	{
		int file_handle = 0;
		file_handle = semihost_init(sysopen_array);
		printf("Semi_host file handle = 0x%8x\n", file_handle);
	}
#endif
#endif

	printf("\nRevision: %s\n" "Build on %s %s\n", REVISION, __DATE__, __TIME__);

	cpu_clk_mhz = sq_scu_cpu_clock_get() / (1000 * 1000);

	result = test_item_ctrl(&main_test_item_container, 0);
	
	printf("\n\nExit the Socle Diagnostic Program!!\n\n");

	return result;
}

#endif	/* CONFIG_ROM */

