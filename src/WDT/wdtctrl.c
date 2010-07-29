#include <platform.h>
#include <irqs.h>
#include "regs-wdt.h"
#if defined(CONFIG_PC9220)
#include <pc9220-scu.h>
#endif

#if defined(CONFIG_PC9223)
#include <pc9223-scu.h>
#endif

extern int
wdt_test(int autotest)
{
	int ret = 0, pseudo_flag = 0;

#if defined(CONFIG_CDK) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK)
	iowrite32(ioread32(SOCLE_SCU0 + 0x28) | (0x1 << 13), SOCLE_SCU0 + 0x28);
#endif
#if defined(CONFIG_PC9220) || defined(CONFIG_PC9223)
	socle_scu_wdt_reset_enable(1);
#endif

	// set reload reg, prescaler
	iowrite32(socle_scu_apb_clock_get() / 60 * 3, SOCLE_WDTLR);
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
