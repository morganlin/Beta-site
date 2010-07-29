#ifndef __ADC_REG_H_INCLUDED
#define __ADC_REG_H_INCLUDED

#define ADC_DATA		0x00
#define ADC_STATUS		0x04
#define ADC_CTRL		0x08

#define ADC_TOUCH_SWITCH_XM_ENABLE	(0x0<<10)
#define ADC_TOUCH_SWITCH_YM_ENABLE	(0x0<<9)
#define ADC_TOUCH_SWITCH_XP_ENABLE	(0x0<<8)
#define ADC_TOUCH_SWITCH_YP_ENABLE	(0x0<<7)
#define ADC_TOUCH_SWITCH_XM_DISABLE	(0x1<<10)
#define ADC_TOUCH_SWITCH_YM_DISABLE	(0x1<<9)
#define ADC_TOUCH_SWITCH_XP_DISABLE	(0x1<<8)
#define ADC_TOUCH_SWITCH_YP_DISABLE	(0x1<<7)
#define ADC_TOUCH_SWITCH_MASK	(0xf<<7)
#define ADC_INT_STATUS			(0x1<<6)
#define ADC_INT_ENABLE			(0x1<<5)
#define ADC_INT_DISABLE		(0x0<<5)
#define ADC_INT_MASK			(0x1<<5)
#define ADC_START_CONVERSION	(0x1<<4)
#define ADC_POWER_UP			(0x1<<3)

#define ADC_SOURCE_0			0
#define ADC_SOURCE_1			1
#define ADC_SOURCE_2			2
#define ADC_SOURCE_3			3
#define ADC_SOURCE_4			4
#define ADC_SOURCE_5			5
#define ADC_SOURCE_6			6
#define ADC_SOURCE_7			7
#define ADC_SOURCE_MASK		0x7

#endif
