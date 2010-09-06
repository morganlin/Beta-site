#ifndef __SQ_PWM_REG_H
#define __SQ_PWM_REG_H

#include "dependency.h"

#define PWMT_BASE_OFFSET		0X0010

#define PWMT_CNTR				0X0000
#define PWMT_HRC				0X0004
#define PWMT_LRC				0X0008
#define PWMT_CTRL				0X000C


// PWMT_CTRL (0X000C)
#define PWMT_CTRL_PRESCALE_MSK	0x00003E00
#define PWMT_CTRL_PRESCALE_S	9

#define PWMT_CTRL_CAP			BIT_SHIHT(8)
#define PWMT_CTRL_RST			BIT_SHIHT(7)
#define PWMT_CTRL_INT_CLR		BIT_SHIHT(6)
#define PWMT_CTRL_INT_EN		BIT_SHIHT(5)
#define PWMT_CTRL_SIG_CNTR		BIT_SHIHT(4)
#define PWMT_CTRL_OPT_EN		BIT_SHIHT(3)
#define PWMT_CTRL_EN			BIT_SHIHT(0)

struct sq_pwmt {
	int busy;
	unsigned int base;
	int irq;
	struct sq_pwmt_driver *drv;
};

struct sq_pwmt_driver {
	void (*claim_pwm_lock)(void);
	void (*release_pwm_lock)(void);
	void (*reset)(struct sq_pwmt *);
	void (*output_enable)(struct sq_pwmt *, int);		// 1:enable, 0:disable
	void (*enable)(struct sq_pwmt *, int);				// 1:enable, 0:disable
	void (*capture_mode_enable)(struct sq_pwmt *, int);				// 1:enable, 0:disable
	void (*clear_interrupt)(struct sq_pwmt *);
	void (*enable_interrupt)(struct sq_pwmt *, int);		// 1:enable, 0:disable
	void (*single_counter_mode_enable)(struct sq_pwmt *, int);				// 1:enable, 0:disable
	void (*set_counter)(struct sq_pwmt *, unsigned int);
	unsigned int (*read_hrc)(struct sq_pwmt *);
	unsigned int (*read_lrc)(struct sq_pwmt *);
	void (*write_hrc)(struct sq_pwmt *, unsigned int);
	void (*write_lrc)(struct sq_pwmt *, unsigned int);
	unsigned int (*read_prescale_factor)(struct sq_pwmt *);
	void (*write_prescale_factor)(struct sq_pwmt *, unsigned int);
	void (*set_ctrl)(struct sq_pwmt *, unsigned int);
};


extern struct sq_pwmt * get_sq_pwmt_structure(int num);
extern int release_sq_pwmt_structure(int num);
extern void sq_init_pwmt(void);


#endif	//__SQ_PWM_REG_H
