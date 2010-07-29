#ifndef _ads7846_h_included_
#define _ads7846_h_included_

#ifndef ADS7846_INTR
//#define ADS7846_INTR		-1
#define ADS7846_INTR		0	//jerry fix 2010/03/01
#endif

/* ADS7846 register definition */
#define ADS7846_START			(1 << 7) 
#define ADS7846_MODE_12BIT		(0 << 3)
#define ADS7846_MODE_8BIT		(1 << 3)
#define ADS7846_SINGLE_END		(1 << 2)
#define ADS7846_DIFFERENTIAL		(0 << 2)
#define ADS7846_POWER_DOWN_CONVERSION		(0 << 1)
#define ADS7846_POWER_DOWN_REF_OFF_ADC_ON	(1 << 1)
#define ADS7846_POWER_DOWN_REF_ON_ADC_OFF	(2 << 1)
#define ADS7846_POWER_DOWN_REF_ON_ADC_ON	(3 << 1)

/* Differential Reference Mode Input Configuration */
#define ADS7846_DIFF_POS_X                  (5 << 4)
#define ADS7846_DIFF_POS_Y                  (1 << 4)
#define ADS7846_DIFF_POS_Z1                 (3 << 4)
#define ADS7846_DIFF_POS_Z2                 (4 << 4)

/* Single-Ended Reference Mode Input Configuration */
#define ADS7846_SINGLE_POS_X                  (5 << 4)
#define ADS7846_SINGLE_POS_Y                  (1 << 4)
#define ADS7846_SINGLE_POS_Z1                 (3 << 4)
#define ADS7846_SINGLE_POS_Z2                 (4 << 4)
#define ADS7846_SINGLE_TEMP0                  (0 << 4)
#define ADS7846_SINGLE_BAT                    (2 << 4)
#define ADS7846_SINGLE_AUX                    (6 << 4)
#define ADS7846_SINGLE_TEMP1                  (7 << 4)

#endif // _ads7846_h_included_

