#include <type.h>

/* enable IRQ interrupts */
extern void
enable_irq(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"bic %0, %0, #0x80\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}

/* enable FIQ interrupts */
extern void
enable_fiq(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"bic %0, %0, #0x40\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}

/* enable IRQ/FIQ interrupts */
extern void
enable_interrupts(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"bic %0, %0, #0xc0\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}


/*
 * disable IRQ interrupts
 */
extern void
disable_irq(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"orr %0, %0, #0x80\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}

/*
 * disable FIQ interrupts
 */
extern void
disable_fiq(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"orr %0, %0, #0x40\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}

/*
 * disable IRQ/FIQ interrupts
 */
extern void
disable_interrupts(void)
{
	unsigned long temp;
	__asm__ __volatile__(
			"mrs %0, cpsr\n"
			"orr %0, %0, #0xc0\n"
			"msr cpsr_c, %0"
			: "=r" (temp)
			:
			: "memory", "cc");
}


extern void
asm_delay(u32 inst)
{
	int tmp = (inst) / 22;

	__asm__ __volatile__ (
		"1:	subs %0, %0, #1\n"
		"	bhi 1b"
		:
		: "r" (tmp)
		: "memory");
}

extern u32
__xchg(u32 x, volatile void *ptr, u32 size)
{
	u32 ret;

	switch (size) {
	case 1:
		asm volatile (
			"@  __xchg1\n"
			" swpb %0, %1, [%2]"
			: "=&r" (ret)
			: "r" (x), "r" (ptr)
			: "memory", "cc");
		break;
	case 4:
		asm volatile (
			"@ __xchg4\n"
			" swp %0, %1, [%2]"
			: "=&r" (ret)
			: "r" (x), "r" (ptr)
			: "memory", "cc");
		break;
	}

	return ret;
}

/* cpu_arm926_do_idle */
extern void
cpu_do_idle(void)
{
	unsigned long tmp0, tmp1, tmp2;

	__asm__ __volatile__(
		"mov     %0, #0 \n"
		"mrc     p15, 0, %1, c1, c0, 0 \n"			// Read control register
		"mcr     p15, 0, %0, c7, c10, 4 \n"			// Drain write buffer
		"bic     %2, %1, #1 << 12 \n"
		"mcr     p15, 0, %2, c1, c0, 0 \n"			// Disable I cache
		"mcr     p15, 0, %0, c7, c0, 4 \n"			// Wait for interrupt
		"mcr     p15, 0, %1, c1, c0, 0 \n"			// Restore ICache enable
		: "=r" (tmp0), "=r" (tmp1), "=r" (tmp2)
		:
		: "memory");
}
