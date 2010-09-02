#include <platform.h>
#include <irqs.h>
#include "regs-wdt.h"

#if defined(CONFIG_PC9223)
#include <pc9223-scu.h>
#endif

extern int
wdt_test(int autotest)
{
	int ret = 0, pseudo_flag = 0;
#if defined(CONFIG_PC9223)
	sq_scu_wdt_reset_enable(1);
#endif

	// set reload reg, prescaler
	iowrite32(sq_scu_apb_clock_get() / 60 * 3, SQ_WDTLR);
	WDT_PRESCALE(PRESCALE_64);

	printf("WDT: The system will reset now!!\n");

	WDT_RST_EN();
	WDT_EN();

	if (socle_wait_for_int(&pseudo_flag, 5)) {
		printf("Timeout!! The system does not reset!!\n");
		ret = -1;
	}

	return ret;
}
