#include <genlib.h>
#include <type.h>
#include <sq-scu.h>
#include <io.h>

//#define CONFIG_SOCLE_WAIT_DEBUG
#ifdef CONFIG_SOCLE_WAIT_DEBUG
	#define WAIT_DBG(fmt, args...) printf("SOCLE_WAIT: " fmt, ## args)
#else
	#define WAIT_DBG(fmt, args...)
#endif


extern int
socle_wait_for_int(volatile int *flag, int sec)
{
	int t = 0, threshold = sq_scu_cpu_clock_get() / 95 * sec;

#if defined(CONFIG_ARM7) || (CONFIG_ARM7_HI)
	threshold /= 2;
#endif

	WAIT_DBG("flag = %d, sec = %d, threshold = 0x%08x\n", *flag, sec, threshold);
        /* added by morganlin */
       // printf("flag = %d, sec = %d, threshold = 0x%08x\n", *flag, sec, threshold);
        
	while (1 != *flag) {
                /* added by morganlin */
                //printf("t = %d , threshold = %d *flag= %d\n",t,threshold,*flag);
		if (t >= threshold) {
                        /* added by morganlin */
		       // printf("t >= threshold , then return -1\n");
                        return -1;
                }
                t++;
	}
	*flag = 0;

	WAIT_DBG("t = %x\n", t);

	return 0;
}

extern int
socle_wait_by_poll(int addr, int mask, int exp_val, int sec)
{
	int t = 0, val, threshold = sq_scu_cpu_clock_get() / 170 * sec;

#if defined(CONFIG_ARM7) || (CONFIG_ARM7_HI)
	threshold /= 2;
#endif

	exp_val &= mask;
	WAIT_DBG("addr = 0x%08x, mask = 0x%08x, exp_val = 0x%08x, sec = %d, threshold = 0x%08x\n", addr, mask, exp_val, sec, threshold);

	do {
		val = ioread32(addr);
		val &= mask;

		if (t >= threshold)
			return -1;
		t++;
	} while (exp_val != val);

	WAIT_DBG("t = %x\n", t);

	return 0;
}
