#include <global.h>
#include <irqs.h>
#include "pwmt-regs.h"
#ifdef CONFIG_PC9223
#include <pc9223-scu.h>
#endif

//#define CONFIG_SQ_PWMT_DEBUG
#ifdef CONFIG_SQ_PWMT_DEBUG
	#define PWMT_DBG(fmt, args...) printf("SQ_PWMT: " fmt, ## args)
#else
	#define PWMT_DBG(fmt, args...)
#endif


static int pwmt_lock = 0;

/* sq_pwmt_driver */
static void sq_pwmt_claim_lock(void);
static void sq_pwmt_release_lock(void);
static void sq_pwmt_reset(struct sq_pwmt *p_pwm_st);
static void sq_pwmt_output_enable(struct sq_pwmt *p_pwm_st, int en);
static void sq_pwmt_enable(struct sq_pwmt *p_pwm_st, int en);
static void sq_pwmt_capture_mode_enable(struct sq_pwmt *p_pwm_st, int en);
static void sq_pwmt_clear_interrupt(struct sq_pwmt *p_pwm_st);
static void sq_pwmt_enable_interrupt(struct sq_pwmt *p_pwm_st, int en);
static void sq_pwmt_single_counter_mode_enable(struct sq_pwmt *p_pwm_st, int en);
static void sq_pwmt_set_counter(struct sq_pwmt *p_pwm_st, u32 data);
static u32 sq_pwmt_read_hrc(struct sq_pwmt *p_pwm_st);
static u32 sq_pwmt_read_lrc(struct sq_pwmt *p_pwm_st);
static void sq_pwmt_write_hrc(struct sq_pwmt *p_pwm_st, u32 data);
static void sq_pwmt_write_lrc(struct sq_pwmt *p_pwm_st, u32 data);
static u32 sq_pwmt_read_prescale_factor(struct sq_pwmt *p_pwm_st);
static void sq_pwmt_write_prescale_factor(struct sq_pwmt *p_pwm_st, u32 data);
static void sq_pwmt_set_ctrl(struct sq_pwmt *p_pwm_st, u32 data);

static struct sq_pwmt_driver sq_pwm_drv = {
	.claim_pwm_lock	= sq_pwmt_claim_lock,
	.release_pwm_lock	= sq_pwmt_release_lock,
	.reset	= sq_pwmt_reset,
	.output_enable	= sq_pwmt_output_enable,
	.enable	= sq_pwmt_enable,
	.capture_mode_enable	= sq_pwmt_capture_mode_enable,
	.clear_interrupt	= sq_pwmt_clear_interrupt,
	.enable_interrupt	= sq_pwmt_enable_interrupt,
	.single_counter_mode_enable	= sq_pwmt_single_counter_mode_enable,
	.set_counter	= sq_pwmt_set_counter,
	.read_hrc	= sq_pwmt_read_hrc,
	.read_lrc	= sq_pwmt_read_lrc,
	.write_hrc	= sq_pwmt_write_hrc,
	.write_lrc	= sq_pwmt_write_lrc,
	.read_prescale_factor	= sq_pwmt_read_prescale_factor,
	.write_prescale_factor	= sq_pwmt_write_prescale_factor,
	.set_ctrl	= sq_pwmt_set_ctrl,
};

/* sq pwmt devie */
static struct sq_pwmt sq_pwm[SQ_PWM_NUM];


static inline void
pwmt_read(u32 offset, u32 *data, u32 base)
{
	*data = ioread32(base + offset);

	PWMT_DBG("pwmt_read(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, *data);
}

static inline void
pwmt_write(u32 offset, u32 data, u32 base)
{
	iowrite32(data, base + offset);

	PWMT_DBG("pwmt_write(): base:0x%08x, offset:0x%08x, data:0x%08x\n", base, offset, data);
}

static void
sq_pwmt_claim_lock(void)
{
	pwmt_lock = 1;
}

static void
sq_pwmt_release_lock(void)
{
	pwmt_lock = 0;
}

static void
sq_pwmt_reset(struct sq_pwmt *p_pwm_st)
{
	pwmt_write(PWMT_CTRL, PWMT_CTRL_RST, p_pwm_st->base);
}

static void
sq_pwmt_output_enable(struct sq_pwmt *p_pwm_st, int en)
{
	u32 tmp;

	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_OPT_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_OPT_EN, p_pwm_st->base);
}

static void
sq_pwmt_enable(struct sq_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_EN, p_pwm_st->base);
}

static void
sq_pwmt_capture_mode_enable(struct sq_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_CAP, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_CAP, p_pwm_st->base);
}

static void
sq_pwmt_clear_interrupt(struct sq_pwmt *p_pwm_st)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);
	pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_INT_CLR, p_pwm_st->base);
}

static void
sq_pwmt_enable_interrupt(struct sq_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_INT_EN, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_INT_EN, p_pwm_st->base);
}

static void
sq_pwmt_single_counter_mode_enable(struct sq_pwmt *p_pwm_st, int en)
{
	u32 tmp;
	
	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);

	if (en)
		pwmt_write(PWMT_CTRL, tmp | PWMT_CTRL_SIG_CNTR, p_pwm_st->base);
	else
		pwmt_write(PWMT_CTRL, tmp & ~PWMT_CTRL_SIG_CNTR, p_pwm_st->base);
}

static void
sq_pwmt_set_counter(struct sq_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_CNTR, data, p_pwm_st->base);
}

static u32
sq_pwmt_read_hrc(struct sq_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_HRC, &data, p_pwm_st->base);
	
	return data;
}

static u32
sq_pwmt_read_lrc(struct sq_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_LRC, &data, p_pwm_st->base);
	
	return data;
}

static void
sq_pwmt_write_hrc(struct sq_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_HRC, data, p_pwm_st->base);
}

static void
sq_pwmt_write_lrc(struct sq_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_LRC, data, p_pwm_st->base);
}

static u32
sq_pwmt_read_prescale_factor(struct sq_pwmt *p_pwm_st)
{
	u32 data;

	pwmt_read(PWMT_CTRL, &data, p_pwm_st->base);
	data = (data & PWMT_CTRL_PRESCALE_MSK) >> PWMT_CTRL_PRESCALE_S;

	return data;
}

static void
sq_pwmt_write_prescale_factor(struct sq_pwmt *p_pwm_st, u32 data)
{
	u32 tmp;

	pwmt_read(PWMT_CTRL, &tmp, p_pwm_st->base);
	pwmt_write(PWMT_CTRL, (tmp & PWMT_CTRL_PRESCALE_MSK) | (data << PWMT_CTRL_PRESCALE_S), p_pwm_st->base);
}

static void
sq_pwmt_set_ctrl(struct sq_pwmt *p_pwm_st, u32 data)
{
	pwmt_write(PWMT_CTRL, data, p_pwm_st->base);
}


extern struct sq_pwmt *
get_sq_pwmt_structure(int num)
{
	struct sq_pwmt *p = NULL;

	if (SQ_PWM_NUM <= num) {
		printf("Error request PWMT num = %d!, max num = %d\n", num, SQ_PWM_NUM - 1);
		return p;
	}

	sq_pwmt_claim_lock();

	if (sq_pwm[num].busy) {
		printf("PWMT[%d] is busy!\n", num);
	} else {
		sq_pwm[num].busy = 1;
		p = &sq_pwm[num];
	}
	
	PWMT_DBG("PWMT[%d] base = 0x%08x, irq = %d\n", num, p->base, p->irq);

	sq_pwmt_release_lock();

	return p;
}

extern int
release_sq_pwmt_structure(int num)
{
	int ret = 0;

	if (SQ_PWM_NUM <= num) {
		printf("Error request PWMT num = %d!, max num = %d\n", num, SQ_PWM_NUM - 1);
		return -1;
	}

	sq_pwmt_claim_lock();

	sq_pwm[num].busy = 0;
	
	PWMT_DBG("Release PWMT[%d]!\n", num);

	sq_pwmt_release_lock();

	return ret;
}


extern void
sq_init_pwmt(void)
{
	int irq, i;
	u32 base;

	PWMT_DBG("pwmt init\n");

#if defined(CONFIG_PC9223)
	sq_scu_dev_enable(SQ_DEVCON_PWM0);
	sq_scu_dev_enable(SQ_DEVCON_PWM1);
#endif

	base = SQ_TIMER_PWM0;
	irq = SQ_INTC_PWM0;

	for (i = 0; i < SQ_PWM_NUM; i++) {
		if (i) {
			base += PWMT_BASE_OFFSET;
			irq++;
		}

		PWMT_DBG("sq_pwm[%d] base = 0x%08x, irq = %d\n", i, base, irq);
		sq_pwm[i].base = base;
		sq_pwm[i].irq = irq;
		sq_pwm[i].drv = &sq_pwm_drv;
		sq_pwm[i].busy = 0;

		sq_pwmt_reset(&sq_pwm[i]);
	}
}
