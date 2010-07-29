#ifndef _pwm_h_included_
#define _pwm_h_included_

#include <platform.h>
#include <irqs.h>

#define PWM_BASE_ADDR	(SOCLE_TIMER_PWM0)

#define PWM_BASE		PWM_BASE_ADDR

#define PWMT0_CNTR	(PWM_BASE)
#define PWMT0_HRC	(PWM_BASE + 0x04)
#define PWMT0_LRC	(PWM_BASE + 0x08)
#define PWMT0_CTRL	(PWM_BASE + 0x0c)
#define PWMT1_CNTR	(PWM_BASE + 0x10)
#define PWMT1_HRC	(PWM_BASE + 0x14)
#define PWMT1_LRC	(PWM_BASE + 0x18)

#define PWM_OUTPUT_ENABLE	(1<<3)
#define PWM_TIMER_ENABLE	(1<<0)


#endif // _mailbox_h_included_
