#include <platform.h>
#include <genlib.h>
#include <test_item.h>
#include "irqs.h"
#include "Interrupt/interrupt.h"
#include "regs-pwm.h"
#include "pwm-ctrl.h"

#define DUTY_ALL 250
#define DUTY_HIGH 150

void Socle_PWM_init(void)
{
	writel (0x80, PWMT0_CTRL);
	writel (DUTY_HIGH, PWMT0_HRC);
	writel (DUTY_ALL, PWMT0_LRC);
	writel (PWM_OUTPUT_ENABLE | PWM_TIMER_ENABLE, PWMT0_CTRL);
	writel (0, PWMT0_CNTR);
}

int clcd_brightness_decrement(int autotest)
{
	if (autotest)
	{
		while (readl (PWMT0_HRC) > (DUTY_ALL/10)) {
			writel (readl (PWMT0_HRC) - DUTY_ALL/10, PWMT0_HRC);
			MSDELAY(5);	/* 1 second delay */
		}
	}
	else
	{
		if (readl (PWMT0_HRC) > (DUTY_ALL/10))
			writel (readl (PWMT0_HRC) - DUTY_ALL/10, PWMT0_HRC);
	}
	return 0;

}

int clcd_brightness_increment(int autotest)
{
	if (autotest)
	{
		while((readl (PWMT0_HRC) + DUTY_ALL/10) < readl (PWMT0_LRC)) {
			writel (readl (PWMT0_HRC) + DUTY_ALL/10, PWMT0_HRC);
			MSDELAY(5);	/* 1 second delay */
		}
	}
	else
	{
		if (readl (PWMT0_HRC) + DUTY_ALL/10 < readl (PWMT0_LRC))
			writel (readl (PWMT0_HRC) + DUTY_ALL/10, PWMT0_HRC);
	}
	return 0;

}

int clcd_brightness_reset(int autotest)
{
	Socle_PWM_init();
	return 0;
}

