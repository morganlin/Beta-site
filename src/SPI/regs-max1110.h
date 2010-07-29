#ifndef _max1110_h_included_
#define _max1110_h_included_


/* MAX1110 register definition */
#define MAX1110_START				(1 << 7) 

#define MAX1110_CHx				(7 << 4) 
#define MAX1110_SINGLE_END_CH0	(0 << 4) 
#define MAX1110_SINGLE_END_CH1	(4 << 4) 
#define MAX1110_SINGLE_END_CH2	(1 << 4) 
#define MAX1110_SINGLE_END_CH3	(5 << 4) 
#define MAX1110_SINGLE_END_CH4	(2 << 4) 
#define MAX1110_SINGLE_END_CH5	(6 << 4) 
#define MAX1110_SINGLE_END_CH6	(3 << 4) 
#define MAX1110_SINGLE_END_CH7	(7 << 4) 
#define MAX1110_DIFFERENTIAL_CH0_CH1	(0 << 4)
#define MAX1110_DIFFERENTIAL_CH2_CH3	(1 << 4)
#define MAX1110_DIFFERENTIAL_CH4_CH5	(2 << 4)
#define MAX1110_DIFFERENTIAL_CH6_CH7	(3 << 4)
#define MAX1110_DIFFERENTIAL_CH1_CH0	(4 << 4)
#define MAX1110_DIFFERENTIAL_CH3_CH2	(5 << 4)
#define MAX1110_DIFFERENTIAL_CH5_CH4	(6 << 4)
#define MAX1110_DIFFERENTIAL_CH7_CH6	(7 << 4)

#define MAX1110_UNI_BIP			(1 << 3)
#define MAX1110_UNI_BIP_UNIPOLAR	(1 << 3)
#define MAX1110_UNI_BIP_BIPOLAR	(0 << 3)

#define MAX1110_SGL_DIF			(1 << 2)
#define MAX1110_SGL_DIF_SGL		(1 << 2)
#define MAX1110_SGL_DIF_DIF		(0 << 2)

#define MAX1110_PD1				(1 << 1)
#define MAX1110_PD1_FULL_OP		(1 << 1)
#define MAX1110_PD1_POWER_DOWN	(0 << 1)

#define MAX1110_PD0				(1 << 0)
#define MAX1110_PD0_EXT_CLK		(1 << 0)
#define MAX1110_PD0_INT_CLK		(0 << 0)


#endif // _max1110_h_included_

