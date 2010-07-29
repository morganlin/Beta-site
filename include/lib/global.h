#ifndef _global_h_included_
#define _global_h_included_

#include <io.h>
#include <type.h>
#include <genlib.h>
#include <socle-scu.h>
#include <platform.h>
#include <irqs.h>

//#define  BIG_ENDIAN //endian switch for usb and else

#define	FPGA_1_1		1
#define	FPGA_2_1		2
#define	ASIC_1_1		3
#define	ASIC_2_1		4

#if defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD)
#else
#define	BOARD			FPGA_1_1	//For FPGA & LII_A...arthur
#endif
/*#define	BOARD			ASIC_1_1 //For ASIC inverse MAC MDIO*/

#if ((BOARD == FPGA_1_1) || (BOARD == FPGA_2_1))
#define __MAC_FPGA__
#endif

// Endia define
#define F_LITTLE_ENDIAN		0
#define F_BIG_ENDIAN		1

// CPU endia define
#ifdef  SYS_CPU_BIG
#define	CPU_BYTE_ORDER		F_BIG_ENDIAN
#else
#define	CPU_BYTE_ORDER		F_LITTLE_ENDIAN
#endif

// CPU type
#if defined (CPU_ARCH_MIPS)
#define CPU_TYPE	"MIPS4Kc"
#elif defined (CONFIG_ARM9)
#define CPU_TYPE	"ARM926EJ-S"
#elif defined (CONFIG_ARM9_HI)
#define CPU_TYPE	"ARM926EJ-S High Vector"
#elif defined (CONFIG_ARM7)
#define CPU_TYPE	"ARM726EJ-S"
#elif defined (CONFIG_ARM7_HI)
#define CPU_TYPE	"ARM726EJ-S High Vector"
#elif defined (CONFIG_ARM11)
#define CPU_TYPE	"ARM1176JZ"
#else
#error "CPU_ARCH is not well defined!"
#endif


// Mega Alignment
extern int socle_memory_addr_start;
#define TEST_PATTERN_START		((((((u32_t)_end) >> 20 )+ 1) << 20) | socle_memory_addr_start)


#define	CLKnS(freqH)			(1000*1000*1000/(freqH))
#define nS2CLK(nS,freqH)		(  ( ((int)((nS)/CLKnS((freqH)))) < ((double)(nS)/CLKnS((freqH))) ) ? ((int)((nS)/CLKnS((freqH)))+1) : ((int)((nS)/CLKnS((freqH))))  )

// common macro
#define BIT_MASK(nbits)			((0x1 << (nbits)) - 1)
#define BIT_SHIHT(nbits)		(0x1 << (nbits))

extern int cpu_clk_mhz;

extern void asm_delay(u32 inst);
#define USDELAY(usec)				(asm_delay((usec)*cpu_clk_mhz))
#define MSDELAY(msec)				(asm_delay((msec)*1000))

/*
 * Input	:	flag: a pointer points to a flag, when *flag == 1, exit this loop
 *				sec: wait time (seconds)
 * Output	:	return 0: succeed, -1: timeout
 */
extern int socle_wait_for_int(volatile int *flag, int sec);

/*
 * Input	:	addr: ip address
 *				mask: wait mask
 *				exp_val: expectative value
 *				sec: wait time (seconds)
 * Output	:	return 0: succeed, -1: timeout
 */
extern int socle_wait_by_poll(int addr, int mask, int exp_val, int sec);

#define xchg(ptr, x)				\
	((__typeof__(*(ptr)))__xchg((u32)(x), (ptr), sizeof(*(ptr))))
extern u32 __xchg(u32 x, volatile void *ptr, u32 size);

# define spin_lock_init(lock)				\
	do { (void) xchg(lock, 0); } while (0)

# define spin_lock(lock)					\
	while (xchg(lock, 1) != 0);

# define spin_unlock(lock)					\
	do { (void) xchg(lock, 0); } while (0)

# define spin_lock_irq(lock)					\
{											\
	extern void disable_irq(void);				\
	disable_irq();							\
	spin_lock(lock);							\
}

# define spin_unlock_irq(lock)					\
{											\
	extern void enable_irq(void);				\
	spin_unlock(lock);						\
	enable_irq();							\
}

extern void cpu_do_idle(void);

#endif  // _global_h_included_
