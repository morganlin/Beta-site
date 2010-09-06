#include <test_item.h>
#include "dependency.h"

extern int sq_pwm_test(int autotest);
extern int sq_buzzer_test(int autotest);

struct test_item pwm_main_test_items[] = {
	{
		"PWMT Test",
		sq_pwm_test,
		1,
		1
	},
#ifdef CONFIG_BUZZER
	{
		"Buzzer Test",
		sq_buzzer_test,
		1,
		1
	},
#endif
};

struct test_item_container pwm_main_container = {
     .menu_name = "PWMT Test Main Menu",
     .shell_name = "PWMT",
     .items = pwm_main_test_items,
     .test_item_size = sizeof(pwm_main_test_items)
};

extern int sq_pwmt_timer_mode(int autotest);
extern int sq_pwmt_capture_mode(int autotest);

struct test_item pwmt_main_test_items[] = {
	{
		"Timer Mode",
		sq_pwmt_timer_mode,
		1,
		1
	},
	{
		"Capture Mode",
		sq_pwmt_capture_mode,
		1,
		0
	}
};

struct test_item_container pwmt_main_container = {
     .menu_name = "PWM Timer Test Main Menu",
     .shell_name = "PWMT",
     .items = pwmt_main_test_items,
     .test_item_size = sizeof(pwmt_main_test_items)
};


extern int sq_pwmt_timer_0(int autotest);
extern int sq_pwmt_timer_1(int autotest);
extern int sq_pwmt_timer_2(int autotest);
extern int sq_pwmt_timer_3(int autotest);

struct test_item pwmt_test_items[] = {
	{
		"PWM Timer 0",
		sq_pwmt_timer_0,
		1,
		SQ_PWMT_0_ENABLE
	},
	{
		"PWM Timer 1",
		sq_pwmt_timer_1,
		1,
		SQ_PWMT_1_ENABLE
	},
	{
		"PWM Timer 2",
		sq_pwmt_timer_0,
		1,
		SQ_PWMT_2_ENABLE
	},
	{
		"PWM Timer 3",
		sq_pwmt_timer_0,
		1,
		SQ_PWMT_3_ENABLE
	},
};

struct test_item_container pwmt_test_container = {
     .menu_name = "PWM Timer Test Menu",
     .shell_name = "PWMT",
     .items = pwmt_test_items,
     .test_item_size = sizeof(pwmt_test_items)
};


extern int sq_pwmt_periodical_counter_mode_test(int autotest);
extern int sq_pwmt_single_counter_mode_test(int autotest);

struct test_item pwmt_counter_mode_test_items[] = {
	{
		"PWM Timer Periodical Counter Mode",
		sq_pwmt_periodical_counter_mode_test,
		1,
		1
	},
	{
		"PWM Timer Single Counter Mode",
		sq_pwmt_single_counter_mode_test,
		1,
		1
	},
};

struct test_item_container pwmt_counter_mode_container = {
     .menu_name = "PWM Timer Counter Mode Test Menu",
     .shell_name = "Counter",
     .items = pwmt_counter_mode_test_items,
     .test_item_size = sizeof(pwmt_counter_mode_test_items)
};

extern int sq_pwmt_capture_mode_irq (int autotest);
extern int sq_pwmt_capture_mode_pull (int autotest);

struct test_item pwmt_capture_mode_test_items[] = {
	{
		"irq mode",
		sq_pwmt_capture_mode_irq,
		0,
		0
	},
	{
		"pulling mode",
		sq_pwmt_capture_mode_pull,	
		1,
		1,
	},
};

struct test_item_container pwmt_capture_mode_container = {
     .menu_name = "PWM Timer Counter Mode Test Menu",
     .shell_name = "Capture mode",
     .items = pwmt_capture_mode_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_test_items)
};

extern int sq_pwmt_capture_mode_loopback(int autotest);
extern int sq_pwmt_capture_mode_current_sense(int autotest);

struct test_item pwmt_capture_mode_item_test_items[] = {
	{
		"loopback",
		sq_pwmt_capture_mode_loopback,
		1,
		SQ_PWMT_CAPTURE_LOOP,
	},
	{
		"current sense",
		sq_pwmt_capture_mode_current_sense,	
		0,
		SQ_PWMT_CURRENT_SENSE_TEST,
	},
};

struct test_item_container pwmt_capture_mode_item_container = {
     .menu_name = "PWM Timer Capture Mode item Test Menu",
     .shell_name = "Capture item",
     .items = pwmt_capture_mode_item_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_item_test_items)
};

extern int sq_pwmt_capture_mode_loop_duty_75(int autotest);
extern int sq_pwmt_capture_mode_loop_duty_50(int autotest);
extern int sq_pwmt_capture_mode_loop_duty_25(int autotest);

struct test_item pwmt_capture_mode_loop_test_items[] = {
	{
		"Loopback duty 75",
		sq_pwmt_capture_mode_loop_duty_75,
		1,
		1
	},
	{
		"cLoopback duty 50",
		sq_pwmt_capture_mode_loop_duty_50,	
		1,
		1
	},
	{
		"cLoopback duty 25",
		sq_pwmt_capture_mode_loop_duty_25,	
		1,
		1
	},
};

struct test_item_container pwmt_capture_mode_loop_container = {
     .menu_name = "PWM Timer Capture Mode loop Test Menu",
     .shell_name = "Capture Loop",
     .items = pwmt_capture_mode_loop_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_loop_test_items)
};

extern int sq_pwmt_capture_mode_from_0_to_1(int autotest);
extern int sq_pwmt_capture_mode_from_1_to_0(int autotest);

struct test_item pwmt_capture_mode_loop_duty_test_items[] = {
	{
		"PWMT0 tx, PWM1 rx",
		sq_pwmt_capture_mode_from_0_to_1,
		1,
		1
	},
	{
		"PWMT1 tx, PWM0 rx",
		sq_pwmt_capture_mode_from_1_to_0,
		1,
		1
	},
};

struct test_item_container pwmt_capture_mode_loop_duty_container = {
     .menu_name = "PWM Timer Capture Mode loop duty Test Menu",
     .shell_name = "Capture duty",
     .items = pwmt_capture_mode_loop_duty_test_items,
     .test_item_size = sizeof(pwmt_capture_mode_loop_duty_test_items)
};

extern int sq_buzzer_pwm_test(int autotest);
extern int sq_buzzer_gpio_test(int autotest);

struct test_item buzzer_main_test_items[] = {
	{
		"PWM control test",
		sq_buzzer_pwm_test,
		0,
		1
	},
#ifdef CONFIG_GPIO
	{
		"GPIO control test",
		sq_buzzer_gpio_test,
		1,
		0
	},
#endif
};

struct test_item_container buzzer_main_container = {
     .menu_name = "Buzzer Test Main Menu",
     .shell_name = "Buzzer Main",
     .items = buzzer_main_test_items,
     .test_item_size = sizeof(buzzer_main_test_items)
};

