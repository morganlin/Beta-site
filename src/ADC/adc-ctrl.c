#include <test_item.h>
#include <genlib.h>
#include <global.h>
#include <platform.h>
#include "adc-regs.h"

#ifndef SQ_INTC_EXTERNAL0
#define SQ_INTC_EXTERNAL0 0
#endif

#define CHANNEL_MAX	8
#define X_PLATE_RESISTANCE		100

//#define CONFIG_ADC_DEBUG
#ifdef CONFIG_ADC_DEBUG
	#define DBG(fmt, args...) printf("ADC: %s(): " fmt, __FUNCTION__, ## args)
#else
	#define DBG(fmt, args...)
#endif

u32 adc_base;
u32 adc_irq;
u32 adc_source;
u32 adc_interrupt = 0;
u32 adc_conversion_int = 0;
u32 touch_press = 0;
u32 touch_count = 0;

static void adc_isr(void *pparam);
static void touch_screen_isr(void *pparam);
static void adc_initial(void);
static int individual_channel_test(void);
static int all_channel_test(void);
static int touch_screen_test(void);
static int  adc_wait_conversion_by_poll(void);
static int  adc_wait_conversion_by_int(void);

static enum adc_test_item_status {
			INDIVIDUAL_CHANNEL = 0,
			ALL_CHANNEL,
			TOUCH_SCREEN,
		} adc_test_item;

static inline void
socle_adc_write(u32 reg, u32 val, u32 base)
{
	DBG("reg=0x%04x, val=0x%08x, base=0x%08x\n", reg, val, base);
	iowrite32(val, base + reg);
}

static inline u32
socle_adc_read(u32 reg, u32 base)
{
	u32 val = ioread32(base + reg);
	DBG("reg=0x%04x, val=0x%08x, base=0x%08x\n", reg, val, base);
	return val;
}

extern struct test_item_container adc_main_container;

extern int
adc_test(int autotest)
{
	int ret = 0;

	adc_base = SQ_ADC0;
	adc_irq = SQ_INTC_ADC0;

	adc_initial();
		
	ret = test_item_ctrl(&adc_main_container, autotest);
	
	return ret;
}

extern struct test_item_container adc_mode_container;

extern int
adc_individual_channel_test(int autotest)
{
	int ret = 0;

	adc_test_item = INDIVIDUAL_CHANNEL;
		
	ret = test_item_ctrl(&adc_mode_container, autotest);
	
	return ret;
}

extern int
adc_all_channel_test(int autotest)
{
	int ret = 0;

	adc_test_item = ALL_CHANNEL;
	
	ret = test_item_ctrl(&adc_mode_container, autotest);
	
	return ret;
}

extern int
adc_touch_screen_test(int autotest)
{
	int ret = 0;

	request_irq(SQ_INTC_EXTERNAL0, touch_screen_isr, NULL);
	
	adc_test_item = TOUCH_SCREEN;

	ret = test_item_ctrl(&adc_mode_container, autotest);

	return ret;
}

extern struct test_item_container adc_channel_container;
	
extern int
adc_polling_test(int autotest)
{
	int ret = 0;
	
	adc_interrupt = ADC_INT_DISABLE;

	if (adc_test_item == ALL_CHANNEL)
		ret = all_channel_test();
	else if(adc_test_item == INDIVIDUAL_CHANNEL)
		ret = test_item_ctrl(&adc_channel_container, autotest);
	else if(adc_test_item == TOUCH_SCREEN)
		ret = touch_screen_test();

	return ret;
}

extern int
adc_interrupt_test(int autotest)
{
	int ret = 0;
	
	adc_interrupt = ADC_INT_ENABLE;

	//request irq
	request_irq(adc_irq, adc_isr, NULL);

	if (adc_test_item == ALL_CHANNEL)
		ret = all_channel_test();
	else if(adc_test_item == INDIVIDUAL_CHANNEL)
		ret = test_item_ctrl(&adc_channel_container, autotest);
	else if(adc_test_item == TOUCH_SCREEN)
		ret = touch_screen_test();

	return ret;
}

static void
adc_initial(void)
{
	int temp;

	temp = socle_adc_read(ADC_CTRL, adc_base);
#if defined(CONFIG_PC9223)
	temp = temp & (~ADC_POWER_UP);
#else
	temp = temp | ADC_POWER_UP;
#endif
	socle_adc_write(ADC_CTRL, temp, adc_base);

}

extern int
adc_channel_0_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_0;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_1_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_1;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_2_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_2;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_3_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_3;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_4_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_4;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_5_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_5;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_6_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_6;

	ret = individual_channel_test();

	return ret;
}

extern int
adc_channel_7_test(int autotest)
{
	int ret = 0;
	
	adc_source = ADC_SOURCE_7;

	ret = individual_channel_test();

	return ret;
}

static int
individual_channel_test(void)
{
	int ret = 0;
	int temp;
	int data;
	u32 voltage_1;
	u32 voltage_2;

	temp = socle_adc_read(ADC_CTRL, adc_base);
	temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK);
	temp = temp | adc_source | adc_interrupt | ADC_START_CONVERSION;
	
	socle_adc_write(ADC_CTRL, temp, adc_base);

	if (adc_interrupt == ADC_INT_ENABLE)
		data = adc_wait_conversion_by_int();
	else
		data = adc_wait_conversion_by_poll();
	data = (data*1000) / 312;
	voltage_1 = data/1000;
	voltage_2 = data%1000;
	printf("channel[%d] = %d.%03d V\n", adc_source, voltage_1, voltage_2);

	return ret;
}

static int
all_channel_test(void)
{
	int ret = 0;
	int temp;
	int data;
	u32 voltage_1;
	u32 voltage_2;
		
	for (adc_source=0; adc_source<CHANNEL_MAX; adc_source++) {
		
		temp = socle_adc_read(ADC_CTRL, adc_base);
		temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK);
		temp = temp | adc_source | adc_interrupt | ADC_START_CONVERSION;

		socle_adc_write(ADC_CTRL, temp, adc_base);

		if (adc_interrupt == ADC_INT_ENABLE)
			data = adc_wait_conversion_by_int();
		else
			data = adc_wait_conversion_by_poll();

		data = (data*1000) / 312;
		voltage_1 = data/1000;
		voltage_2 = data%1000;
		printf("channel[%d] = %d.%03d V\n", adc_source, voltage_1, voltage_2);
	}
	
	return ret;
}

static int
touch_screen_test(void)
{
	int ret = 0;
	int temp;
	u32 data;
	u32 x_pos, y_pos, z_value, zx, zy;

	touch_count = 0;
		
	while(1) {

		if (touch_press) {
			touch_press = 0;

			// X position measurment
			temp = socle_adc_read(ADC_CTRL, adc_base);
			temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK) & (~ADC_TOUCH_SWITCH_MASK);
			temp = temp | ADC_SOURCE_2 | adc_interrupt | ADC_START_CONVERSION \
					| ADC_TOUCH_SWITCH_XP_ENABLE
					| ADC_TOUCH_SWITCH_XM_ENABLE
					| ADC_TOUCH_SWITCH_YP_DISABLE
					| ADC_TOUCH_SWITCH_YM_DISABLE;
			
			socle_adc_write(ADC_CTRL, temp, adc_base);

			if (adc_interrupt == ADC_INT_ENABLE)
				data = adc_wait_conversion_by_int();
			else
				data = adc_wait_conversion_by_poll();

			x_pos = data;

			// Y position measurment
			temp = socle_adc_read(ADC_CTRL, adc_base);
			temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK) & (~ADC_TOUCH_SWITCH_MASK);
			temp = temp | ADC_SOURCE_0 | adc_interrupt | ADC_START_CONVERSION \
					| ADC_TOUCH_SWITCH_XP_DISABLE
					| ADC_TOUCH_SWITCH_XM_DISABLE
					| ADC_TOUCH_SWITCH_YP_ENABLE
					| ADC_TOUCH_SWITCH_YM_ENABLE;
			
			socle_adc_write(ADC_CTRL, temp, adc_base);

			if (adc_interrupt == ADC_INT_ENABLE)
				data = adc_wait_conversion_by_int();
			else
				data = adc_wait_conversion_by_poll();

			y_pos = data;

			// Zx value measurment
			temp = socle_adc_read(ADC_CTRL, adc_base);
			temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK) & (~ADC_TOUCH_SWITCH_MASK);
			temp = temp | ADC_SOURCE_3 | adc_interrupt | ADC_START_CONVERSION \
					| ADC_TOUCH_SWITCH_XP_DISABLE
					| ADC_TOUCH_SWITCH_XM_ENABLE
					| ADC_TOUCH_SWITCH_YP_ENABLE
					| ADC_TOUCH_SWITCH_YM_DISABLE;
			
			socle_adc_write(ADC_CTRL, temp, adc_base);

			if (adc_interrupt == ADC_INT_ENABLE)
				data = adc_wait_conversion_by_int();
			else
				data = adc_wait_conversion_by_poll();

			zx = data;

			// Zy value measurment
			temp = socle_adc_read(ADC_CTRL, adc_base);
			temp = temp & (~ADC_INT_MASK) & (~ADC_SOURCE_MASK) & (~ADC_TOUCH_SWITCH_MASK);
			temp = temp | ADC_SOURCE_0 | adc_interrupt | ADC_START_CONVERSION \
					| ADC_TOUCH_SWITCH_XP_DISABLE
					| ADC_TOUCH_SWITCH_XM_ENABLE
					| ADC_TOUCH_SWITCH_YP_ENABLE
					| ADC_TOUCH_SWITCH_YM_DISABLE;
			
			socle_adc_write(ADC_CTRL, temp, adc_base);

			if (adc_interrupt == ADC_INT_ENABLE)
				data = adc_wait_conversion_by_int();
			else
				data = adc_wait_conversion_by_poll();

			zy = data;

			z_value = X_PLATE_RESISTANCE * x_pos / 1024 * ((zx / zy) - 1);

			printf("X : %d  ;  Y : %d  ;  Z : %d\n", x_pos, y_pos, z_value);

			touch_count++;
			if(touch_count >= 30)
				break;
		
		}
	
	}

	return ret;

}

static int 
adc_wait_conversion_by_poll(void)
{
	int data;

	DBG("----start\n");
	
	while(socle_adc_read(ADC_STATUS, adc_base));

#if defined(CONFIG_PC9223)
	USDELAY(50);
#endif
	
	data = socle_adc_read(ADC_DATA, adc_base);
	
	return data;
}

static int 
adc_wait_conversion_by_int(void)
{
	int data;

	DBG("----start\n");
	
	while(!adc_conversion_int);
	adc_conversion_int = 0;
	
	data = socle_adc_read(ADC_DATA, adc_base);

	return data;
	
}

static void
adc_isr(void *pparam)
{
	int temp;
	
	DBG("---adc_isr\n");
	
	// clear isr
	temp = socle_adc_read(ADC_CTRL,  adc_base);
	temp = temp & (~ADC_INT_STATUS);
	socle_adc_write(ADC_CTRL, temp, adc_base);
	
	adc_conversion_int = 1;
		
}

static void
touch_screen_isr(void *pparam)
{
	touch_press = 1;
}
