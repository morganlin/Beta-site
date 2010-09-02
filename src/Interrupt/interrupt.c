#include <platform.h>
#include <irqs.h>
#include <global.h>
#include <genlib.h>
#include <test_item.h>
#include "interrupt.h"


#define WAIT_LOOPS 1000

static void testISR (void* pparam);

struct _isr_data {
        int* m_pevent;
        int  m_irq;
};


extern struct test_item_container interrupt_main_container;

extern int
intr_test(int autotest)
{
     int ret = 0;

     ret = test_item_ctrl(&interrupt_main_container, autotest);
     
     return ret;
}


extern int
irq_test_mode_test(int autotest)
{
	int		event;
	int		num;
	int		result = 0;
	struct _isr_data	isrdata;

	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));
	
	// set interrupt controller into test mode
	iowrite32(1, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));

	printf("Vector Test ---\n");

	for (num = 0; num < INTC_MAX_IRQ; num++) {
		printf("%d ", num);
		event = 0;

		// prepare isr data
		isrdata.m_pevent = &event;
		isrdata.m_irq    = num;

		// enable interrupt
		request_irq(num, testISR, &isrdata);

		// s/w trigger interrupt
		iowrite32(0x1 << num, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		if (sq_wait_for_int(&event, 1)) {
			printf(" --- Error Termination\n");
			
			// s/w trigger interrupt clear
			iowrite32(0, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));
			
			// disable interrupt
			free_irq(num);

			result = -1;			
			break;
		}
		
		// s/w trigger interrupt clear
		iowrite32(0, SOCLE_INTC_IRQ_ISCR(INTC_REG_BASE));

		// disable interrupt
		free_irq(num);
	}
	if (!event)
		printf(" --- Finish\n");

	// reset interrupt controller into normal mode
	iowrite32(0, SOCLE_INTC_IRQ_TEST(INTC_REG_BASE));
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_IRQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_IRQ_ICCR(INTC_REG_BASE));

	return result;
}


extern int
fiq_test_mode_test(int autotest)
{
	int		event;
	int		num;
	int		result = 0;
	struct _isr_data	isrdata;

	// disable all interrupt
	iowrite32(0, SOCLE_INTC_FIQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_FIQ_ICCR(INTC_REG_BASE));
	
	// set interrupt controller into test mode
	iowrite32(1, SOCLE_INTC_FIQ_TEST(INTC_REG_BASE));

	printf("Vector Test ---\n");

	for (num = 0; num < INTC_MAX_FIQ; num++) {
		printf("%d ", num);
		event = 0;

		// prepare isr data
		isrdata.m_pevent = &event;
		isrdata.m_irq    = num;

		// enable interrupt
		request_fiq(num, testISR, &isrdata);

		// s/w trigger interrupt
		iowrite32(0x1 << num, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));

		if (sq_wait_for_int(&event, 1)) {
			printf(" --- Error Termination\n");
			
			// s/w trigger interrupt clear
			iowrite32(0, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));
			
			// disable interrupt
			free_fiq(num);

			result = -1;			
			break;
		}
		
		// s/w trigger interrupt clear
		iowrite32(0, SOCLE_INTC_FIQ_ISCR(INTC_REG_BASE));

		// disable interrupt
		free_fiq(num);
	}
	if (!event)
		printf(" --- Finish\n");

	// reset interrupt controller into normal mode
	iowrite32(0, SOCLE_INTC_FIQ_TEST(INTC_REG_BASE));
	
	// disable all interrupt
	iowrite32(0, SOCLE_INTC_FIQ_IECR(INTC_REG_BASE));

	// clear all interrupt
	iowrite32(0xFFFFFFFF, SOCLE_INTC_FIQ_ICCR(INTC_REG_BASE));

	return result;
}

static void
testISR (void* pparam)
{
	struct _isr_data* pdata = (struct _isr_data*)pparam;

	*(pdata->m_pevent) = 1;
}

