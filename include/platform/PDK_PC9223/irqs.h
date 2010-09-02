#ifndef	_irqs_h_included_
#define	_irqs_h_included_

#define	SQ_INTC_EXTERNAL1		30
#define	SQ_INTC_EXTERNAL0		29
#define	SQ_INTC_GPIO3			28
#define	SQ_INTC_NAND0			27
#define	SQ_INTC_GPIO2			26
#define	SQ_INTC_LCD0			25
#define	SQ_INTC_GPIO1			24
#define	SQ_INTC_GPIO0			23
#define	SQ_INTC_HDMA0			22
#define	SQ_INTC_SDHC0			21
#define	SQ_INTC_OTG1     		20
#define	SQ_INTC_OTG0     		19
#define	SQ_INTC_VIP0       		18
#define	SQ_INTC_VOP0          		17
#define	SQ_INTC_MAC0     	   	15
#define	SQ_INTC_ADC0        		14
#define	SQ_INTC_RTC0			13
#define	SQ_INTC_PWM1         	        12
#define	SQ_INTC_PWM0      	  	11
#define	SQ_INTC_TMR2			10
#define	SQ_INTC_TMR1     		9
#define	SQ_INTC_TMR0        		8
#define	SQ_INTC_I2S0        	 	6
#define	SQ_INTC_I2C0         	 	5
#define	SQ_INTC_SPI1       	 	4
#define	SQ_INTC_SPI0      	 	3
#define	SQ_INTC_UART2    	   	2
#define	SQ_INTC_UART1        		1
#define	SQ_INTC_UART0        		0

#define	PANTHER7_INTC_HDMA0		SQ_INTC_HDMA0
#define ADS7846_INTR	SQ_INTC_EXTERNAL0

#endif	//_irqs_h_included_
