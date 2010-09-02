/*
 * board/pdk_pc9223/pc9223-scu.c
 *
 *  Copyright (C) 2007 Socle
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <type.h>
#include <io.h>
#include <platform.h>
#include <sq-scu.h>
#include <pc9223-scu.h>
#include "regs-pc9223-scu.h"


//#define SOCLE_SCU_DEBUG

#ifdef SOCLE_SCU_DEBUG
#define SCUDBUG(fmt, args...) printf("%s() " fmt, __FUNCTION__, ## args)
#else
#define SCUDBUG(fmt, args...)
#endif

#define SCUMSG(fmt, args...) printf(fmt, ## args)

static struct socle_clock_st {
	u32 cpu_clock;
	u32 ahb_clock;
	u32 apb_clock;
	u32 uart_clock;
}socle_clock;

static u32 socle_scu_base = SQ_SCU0;

static inline u32 
socle_scu_read(u32 reg)
{
	u32 val;
		
	val = ioread32(socle_scu_base + reg);
	
	SCUDBUG("socle_scu_read : reg = 0x%08x, val = 0x%08x\n", reg, val);
	
	return val;
}

static inline void
sq_scu_write(u32 val, u32 reg) 
{
	SCUDBUG("sq_scu_write : reg = 0x%08x, val = 0x%08x\n", reg, val);
	
	iowrite32(val, socle_scu_base + reg);
}


	/* read chip ID	*/
extern u32 
sq_scu_chip_id (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_CID);

	return tmp;
}

static u32 socle_scu_pll_formula (int m, int n, int od, int xin);

static u32 
socle_scu_pll_formula (int m, int n, int od, int xin)
{
	u32 clock;

	xin = xin/1000;
	clock = xin * (m+1) / (n+1) / (od+1) ;
	clock = clock*1000;	

	return clock;
}

/* MPLL configuration */
extern int 
sq_scu_mpll_clock_set (int mpll_clock)
{
	u32 tmp, mpll;
	
	switch(mpll_clock){
		case SQ_SCU_CPU_CLOCK_33 :			
			tmp = SCU_CPU_CLOCK_33;
			break;
		case SQ_SCU_CPU_CLOCK_66 :			
			tmp = SCU_CPU_CLOCK_66;
			break;
		case SQ_SCU_CPU_CLOCK_80 :			
			tmp = SCU_CPU_CLOCK_80;
			break;
		case SQ_SCU_CPU_CLOCK_100 :			
			tmp = SCU_CPU_CLOCK_100;
			break;
		case SQ_SCU_CPU_CLOCK_132 :			
			tmp = SCU_CPU_CLOCK_132;
			break;
		case SQ_SCU_CPU_CLOCK_133 :			
			tmp = SCU_CPU_CLOCK_133;
			break;
		case SQ_SCU_CPU_CLOCK_150 :			
			tmp = SCU_CPU_CLOCK_150;
			break;
		case SQ_SCU_CPU_CLOCK_166 :			
			tmp = SCU_CPU_CLOCK_166;
			break;
		case SQ_SCU_CPU_CLOCK_200 :			
			tmp = SCU_CPU_CLOCK_200;
			break;
		case SQ_SCU_CPU_CLOCK_240 :			
			tmp = SCU_CPU_CLOCK_240;
			break;
		case SQ_SCU_CPU_CLOCK_252 :
       	    	        tmp = SCU_CPU_CLOCK_252;
         		   break;
		case SQ_SCU_CPU_CLOCK_264 :			
			tmp = SCU_CPU_CLOCK_264;
			break;
		case SQ_SCU_CPU_CLOCK_266 :			
			tmp = SCU_CPU_CLOCK_266;
			break;
		case SQ_SCU_CPU_CLOCK_280 :			
			tmp = SCU_CPU_CLOCK_280;
			break;
		case SQ_SCU_CPU_CLOCK_300 :			
			tmp = SCU_CPU_CLOCK_300;
			break;
		case SQ_SCU_CPU_CLOCK_320 :			
			tmp = SCU_CPU_CLOCK_320;
			break;
		case SQ_SCU_CPU_CLOCK_340 :			
			tmp = SCU_CPU_CLOCK_340;
			break;
		case SQ_SCU_CPU_CLOCK_350 :			
			tmp = SCU_CPU_CLOCK_350;
			break;
		case SQ_SCU_CPU_CLOCK_360 :			
			tmp = SCU_CPU_CLOCK_360;
			break;
		case SQ_SCU_CPU_CLOCK_380 :			
			tmp = SCU_CPU_CLOCK_380;
			break;
		case SQ_SCU_CPU_CLOCK_390 :			
			tmp = SCU_CPU_CLOCK_390;
			break;
		case SQ_SCU_CPU_CLOCK_400 :			
			tmp = SCU_CPU_CLOCK_400;
			break;
		case SQ_SCU_CPU_CLOCK_410 :
                        tmp = SCU_CPU_CLOCK_410;
                        break;
		case SQ_SCU_CPU_CLOCK_420 :			
			tmp = SCU_CPU_CLOCK_420;
			break;
		case SQ_SCU_CPU_CLOCK_430 :			
			tmp = SCU_CPU_CLOCK_430;
			break;
		case SQ_SCU_CPU_CLOCK_440 :			
			tmp = SCU_CPU_CLOCK_440;
			break;
		case SQ_SCU_CPU_CLOCK_450 :			
			tmp = SCU_CPU_CLOCK_450;
			break;
		case SQ_SCU_CPU_CLOCK_460 :			
			tmp = SCU_CPU_CLOCK_460;
			break;
		case SQ_SCU_CPU_CLOCK_470 :			
			tmp = SCU_CPU_CLOCK_470;
			break;
		case SQ_SCU_CPU_CLOCK_480 :			
			tmp = SCU_CPU_CLOCK_480;
			break;
		case SQ_SCU_CPU_CLOCK_490 :			
			tmp = SCU_CPU_CLOCK_490;
			break;
		case SQ_SCU_CPU_CLOCK_500 :			
			tmp = SCU_CPU_CLOCK_500;
			break;
		case SQ_SCU_CPU_CLOCK_510 :			
			tmp = SCU_CPU_CLOCK_510;
			break;
		case SQ_SCU_CPU_CLOCK_520 :
                        tmp = SCU_CPU_CLOCK_520;
                        break;
		case SQ_SCU_CPU_CLOCK_533 :
                        tmp = SCU_CPU_CLOCK_533;
                        break;
		default :		
			SCUMSG("unknow upll clock !!\n");
			return -1;
			break;
	}

	mpll = ((socle_scu_read(SQ_SCU_MPLLCON) & ~SCU_MPLLCON_PLL_MASK) | (tmp ));
	
	sq_scu_write(mpll, SQ_SCU_MPLLCON);
	
	sq_scu_cpu_clock_get();	

	return 0;
}
	
extern u32
sq_scu_mpll_clock_get (void)
{
	u32 m,n,od;
	u32 mclk;

	mclk = socle_scu_read(SQ_SCU_MPLLCON);
	
	n = (mclk & SCU_MPLLCON_N_MASK) >> SCU_MPLLCON_N;
	m = (mclk & SCU_MPLLCON_M_MASK) >> SCU_MPLLCON_M;
	od = (mclk & SCU_MPLLCON_OD_MASK) >> SCU_MPLLCON_OD;

	mclk = socle_scu_pll_formula(m, n, od, MPLL_XIN);

	return mclk;
}

/* MPLL power down/normal	*/
extern void 
sq_scu_mpll_power_status_set (int act)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON);

	if(act)
		sq_scu_write(tmp | SCU_PWMCON_PWR_NOR , SQ_SCU_PWMCON);
	else
		sq_scu_write(tmp & ~SCU_PWMCON_PWR_NOR , SQ_SCU_PWMCON);
		
	return ;
}

extern int 
socle_scu_mpll_status_get (void)		//return 1:act 0:power down
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_PWR_NOR;

	if(SCU_PWMCON_PWR_NOR == tmp)
		return 1;
	else
		return 0;
}


/* UPLL configuration */
extern int 
sq_scu_upll_clock_set (int clock)
{
	u32 tmp, upll;

	switch(clock){
		case SQ_SCU_UART_CLOCK_176:			
			tmp = SCU_UART_CLOCK_176;
			break;
		default :		
			SCUMSG("unknow upll clock !!\n");
			return -1;
			break;
	}

	upll = ((socle_scu_read(SQ_SCU_UPLLCON) & ~SCU_UPLLCON_PLL_MASK) | (tmp ));
	
	sq_scu_write(upll, SQ_SCU_UPLLCON);
	
	return 0;
}		

extern u32 
sq_scu_upll_clock_get (void)
{
	int m,n,od;
	u32 uclk;

	uclk = socle_scu_read(SQ_SCU_UPLLCON);
	
	n = (uclk & SCU_UPLLCON_N_MASK) >> SCU_UPLLCON_N;
	m = (uclk & SCU_UPLLCON_M_MASK) >>SCU_UPLLCON_M;
	od = (uclk & SCU_UPLLCON_OD_MASK) >> SCU_UPLLCON_OD;

	uclk = socle_scu_pll_formula(m, n, od,UPLL_XIN);

	return uclk;
}


/* UPLL power down/normal	*/
extern void 
sq_scu_upll_power_status_set (int act)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_UPLLCON);
	
	if(act)
		sq_scu_write(tmp & (~SCU_UPLLCON_PLL_PWR_DN) , SQ_SCU_UPLLCON);
	else
		sq_scu_write(tmp | SCU_UPLLCON_PLL_PWR_DN , SQ_SCU_UPLLCON);	
		
	return ;
}
	
extern int 
sq_scu_upll_power_status_get (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_UPLLCON) & SCU_UPLLCON_PLL_PWR_DN;

	if(SCU_MPLLCON_PLL_PWR_DN == tmp)
		return 0;
	else
		return 1;
}


extern int 
socle_scu_uart_clk_src_get (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & SCU_MCLKDIV_UART0_CLK_M) >> SCU_MCLKDIV_UART0_CLK_S;
			break;
		case 1:
			tmp = (tmp & SCU_MCLKDIV_UART1_CLK_M) >> SCU_MCLKDIV_UART1_CLK_S;
			break;
		case 2:
			tmp = (tmp & SCU_MCLKDIV_UART2_CLK_M) >> SCU_MCLKDIV_UART2_CLK_S;
			break;
		default :
			SCUMSG("unknow UART index\n");
			return -1;
			break;			
	}

	switch(tmp){
		case SCU_MCLKDIV_UART_CLK_24:
			tmp = SQ_SCU_UART_CLK_24;
			break;		
		case SCU_MCLKDIV_UART_CLK_UPLL:
			tmp = SQ_SCU_UART_CLK_UPLL;
			break;			
		case SCU_MCLKDIV_UART_CLK_UPLL_2:
			tmp = SQ_SCU_UART_CLK_UPLL_2;
			break;		
		case SCU_MCLKDIV_UART_CLK_UPLL_4:
			tmp = SQ_SCU_UART_CLK_UPLL_4;
			break;
	}
		
	return tmp;
}

	/*	hclk enable/disable	*/
extern int 
sq_scu_hclk_enable (int hclk, int en)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKEN);

	if(en){	
		switch(hclk){
			case SQ_SCU_HCLK_ROM :
				tmp = tmp | SCU_MCLK_ROM;
				break;
			case SQ_SCU_HCLK_DCM :
				tmp = tmp | SCU_MCLK_DCM;
				break;
			case SQ_SCU_HCLK_STSDR :
				tmp = tmp | SCU_MCLK_STSDR;
				break;
			case SQ_SCU_HCLK_NFC :
				tmp = tmp | SCU_MCLK_NFC;
				break;
			case SQ_SCU_HCLK_HDMA :
				tmp = tmp | SCU_MCLK_HDMA;
				break;
			case SQ_SCU_HCLK_VOP :
				tmp = tmp | SCU_MCLK_VOP;
				break;
			case SQ_SCU_HCLK_VIP :
				tmp = tmp | SCU_MCLK_VIP;
				break;
			case SQ_SCU_HCLK_OTG0 :
				tmp = tmp | SCU_MCLK_OTG0;
				break;
			case SQ_SCU_HCLK_OTG1 :
				tmp = tmp | SCU_MCLK_OTG1;
				break;			
			case SQ_SCU_HCLK_MAC :
				tmp = tmp | SCU_MCLK_MAC;
				break;		
			case SQ_SCU_HCLK_CLCD :
				tmp = tmp | SCU_MCLK_CLCD;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}
	}else{
		switch(hclk){
			case SQ_SCU_HCLK_ROM :
				tmp = tmp & ~SCU_MCLK_ROM;
				break;
			case SQ_SCU_HCLK_DCM :
				tmp = tmp & ~SCU_MCLK_DCM;
				break;
			case SQ_SCU_HCLK_STSDR :
				tmp = tmp & ~SCU_MCLK_STSDR;
				break;
			case SQ_SCU_HCLK_NFC :
				tmp = tmp & ~SCU_MCLK_NFC;
				break;
			case SQ_SCU_HCLK_HDMA :
				tmp = tmp & ~SCU_MCLK_HDMA;
				break;
			case SQ_SCU_HCLK_VOP :
				tmp = tmp & ~SCU_MCLK_VOP;
				break;
			case SQ_SCU_HCLK_VIP :
				tmp = tmp & ~SCU_MCLK_VIP;
				break;
			case SQ_SCU_HCLK_OTG0 :
				tmp = tmp & ~SCU_MCLK_OTG0;
				break;
			case SQ_SCU_HCLK_OTG1 :
				tmp = tmp & ~SCU_MCLK_OTG1;
				break;			
			case SQ_SCU_HCLK_MAC :
				tmp = tmp & ~SCU_MCLK_MAC;
				break;		
			case SQ_SCU_HCLK_CLCD :
				tmp = tmp & ~SCU_MCLK_CLCD;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}
	}		
				
	sq_scu_write(tmp, SQ_SCU_MCLKEN);	
	
	return 0;
}

	/*	pclk enable/disable	*/
extern int 
sq_scu_pclk_enable (int pclk, int en)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKEN);

	if(en){	
		switch(pclk){
			case SQ_SCU_PCLK_GPIO3 :
				tmp = tmp | SCU_MCLK_GPIO3;
				break;
			case SQ_SCU_PCLK_GPIO2 :
				tmp = tmp | SCU_MCLK_GPIO2;
				break;
			case SQ_SCU_PCLK_GPIO1 :
				tmp = tmp | SCU_MCLK_GPIO1;
				break;
			case SQ_SCU_PCLK_GPIO0 :
				tmp = tmp | SCU_MCLK_GPIO0;
				break;
			case SQ_SCU_PCLK_ADC :
				tmp = tmp | SCU_MCLK_ADC;
				break;
			case SQ_SCU_PCLK_PWM :
				tmp = tmp | SCU_MCLK_PWM;
				break;
			case SQ_SCU_PCLK_WDT :
				tmp = tmp | SCU_MCLK_WDT;
				break;
			case SQ_SCU_PCLK_RTC :
				tmp = tmp | SCU_MCLK_RTC;
				break;
			case SQ_SCU_PCLK_TIMER :
				tmp = tmp | SCU_MCLK_TIMER;
				break;
			case SQ_SCU_PCLK_SDMMC :
				tmp = tmp | SCU_MCLK_SDMMC;
				break;
			case SQ_SCU_PCLK_I2S :
				tmp = tmp | SCU_MCLK_I2S;
				break;
			case SQ_SCU_PCLK_I2C :
				tmp = tmp | SCU_MCLK_I2C;
				break;
			case SQ_SCU_PCLK_SPI1 :
				tmp = tmp | SCU_MCLK_SPI1;
				break;
			case SQ_SCU_PCLK_SPI0 :
				tmp = tmp | SCU_MCLK_SPI0;
				break;
			case SQ_SCU_PCLK_UART2 :
				tmp = tmp | SCU_MCLK_UART2;
				break;
			case SQ_SCU_PCLK_UART1 :
				tmp = tmp | SCU_MCLK_UART1;
				break;
			case SQ_SCU_PCLK_UART0 :
				tmp = tmp | SCU_MCLK_UART0;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}
	}else{
		switch(pclk){
			case SQ_SCU_PCLK_GPIO3 :
				tmp = tmp & ~SCU_MCLK_GPIO3;
				break;
			case SQ_SCU_PCLK_GPIO2 :
				tmp = tmp & ~SCU_MCLK_GPIO2;
				break;
			case SQ_SCU_PCLK_GPIO1 :
				tmp = tmp & ~SCU_MCLK_GPIO1;
				break;
			case SQ_SCU_PCLK_GPIO0 :
				tmp = tmp & ~SCU_MCLK_GPIO0;
				break;
			case SQ_SCU_PCLK_ADC :
				tmp = tmp & ~SCU_MCLK_ADC;
				break;
			case SQ_SCU_PCLK_PWM :
				tmp = tmp & ~SCU_MCLK_PWM;
				break;
			case SQ_SCU_PCLK_WDT :
				tmp = tmp & ~SCU_MCLK_WDT;
				break;
			case SQ_SCU_PCLK_RTC :
				tmp = tmp & ~SCU_MCLK_RTC;
				break;
			case SQ_SCU_PCLK_TIMER :
				tmp = tmp & ~SCU_MCLK_TIMER;
				break;
			case SQ_SCU_PCLK_SDMMC :
				tmp = tmp & ~SCU_MCLK_SDMMC;
				break;
			case SQ_SCU_PCLK_I2S :
				tmp = tmp & ~SCU_MCLK_I2S;
				break;
			case SQ_SCU_PCLK_I2C :
				tmp = tmp & ~SCU_MCLK_I2C;
				break;
			case SQ_SCU_PCLK_SPI1 :
				tmp = tmp & ~SCU_MCLK_SPI1;
				break;
			case SQ_SCU_PCLK_SPI0 :
				tmp = tmp & ~SCU_MCLK_SPI0;
				break;
			case SQ_SCU_PCLK_UART2 :
				tmp = tmp & ~SCU_MCLK_UART2;
				break;
			case SQ_SCU_PCLK_UART1 :
				tmp = tmp & ~SCU_MCLK_UART1;
				break;
			case SQ_SCU_PCLK_UART0 :
				tmp = tmp & ~SCU_MCLK_UART0;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}	
	}
				
	sq_scu_write(tmp, SQ_SCU_MCLKEN);	
	
	return 0;
}

	/*	aclk enable/disable	*/
extern int 
sq_scu_aclk_enable (int aclk, int en)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_ACLKEN);
	
	if(en){
		switch(aclk){
			case SQ_SCU_ACLK_SD_CLKOUT_FOR_STSDR :
				tmp = tmp | SCU_ACLK_ST_SDR;
				break;
			case SQ_SCU_ACLK_LCDCLK_VOPPCLK_FOR_LCDC_VOP :
				tmp = tmp | SCU_ACLK_LCD_VOP;
				break;		
			case SQ_SCU_ACLK_RMIICLK_FOR_MAC :
				tmp = tmp | SCU_ACLK_MAC;
				break;
			case SQ_SCU_ACLK_VIP :
				tmp = tmp | SCU_ACLK_VIP;
				break;
			case SQ_SCU_ACLK_UART :
				tmp = tmp | SCU_ACLK_UART;
				break;
			case SQ_SCU_ACLK_UTMICLK_FOR_OTG0 :
				tmp = tmp | SCU_ACLK_UTMI_OTG0;
				break;
			case SQ_SCU_ACLK_UTMICLK_FOR_OTG1 :
				tmp = tmp | SCU_ACLK_UTMI_OTG1;
				break;
			case SQ_SCU_ACLK_ADCCLK_FOR_ADC :
				tmp = tmp | SCU_ACLK_ADC;
				break;
			case SQ_SCU_ACLK_I2SCLK_FOR_I2S :
				tmp = tmp | SCU_ACLK_I2S;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART2 :
				tmp = tmp | SCU_ACLK_UART2;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART1 :
				tmp = tmp | SCU_ACLK_UART1;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART0 :
				tmp = tmp | SCU_ACLK_UART0;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}
	}else{
		switch(aclk){
			case SQ_SCU_ACLK_SD_CLKOUT_FOR_STSDR :
				tmp = tmp & ~SCU_ACLK_ST_SDR;
				break;
			case SQ_SCU_ACLK_LCDCLK_VOPPCLK_FOR_LCDC_VOP :
				tmp = tmp & ~SCU_ACLK_LCD_VOP;
				break;		
			case SQ_SCU_ACLK_RMIICLK_FOR_MAC :
				tmp = tmp & ~SCU_ACLK_MAC;
				break;
			case SQ_SCU_ACLK_VIP :
				tmp = tmp & ~SCU_ACLK_VIP;
				break;
			case SQ_SCU_ACLK_UART :
				tmp = tmp & ~SCU_ACLK_UART;
				break;
			case SQ_SCU_ACLK_UTMICLK_FOR_OTG0 :
				tmp = tmp & ~SCU_ACLK_UTMI_OTG0;
				break;
			case SQ_SCU_ACLK_UTMICLK_FOR_OTG1 :
				tmp = tmp & ~SCU_ACLK_UTMI_OTG1;
				break;
			case SQ_SCU_ACLK_ADCCLK_FOR_ADC :
				tmp = tmp & ~SCU_ACLK_ADC;
				break;
			case SQ_SCU_ACLK_I2SCLK_FOR_I2S :
				tmp = tmp & ~SCU_ACLK_I2S;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART2 :
				tmp = tmp & ~SCU_ACLK_UART2;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART1 :
				tmp = tmp & ~SCU_ACLK_UART1;
				break;
			case SQ_SCU_ACLK_UCLK_FOR_UART0 :
				tmp = tmp & ~SCU_ACLK_UART0;
				break;
			default :
				SCUMSG("unknow IP number\n");
				return -1;
				break;
		}
	}		

	sq_scu_write(tmp, SQ_SCU_ACLKEN);	
	
	return 0;
}

/* CPU/AHB clock ratio	*/
extern int 
sq_scu_cpu_hclk_ratio_set (int ratio)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & ~SCU_MCLKDIV_CLK_RATIO_MASK;

	switch(ratio){
		case SQ_SCU_CLOCK_RATIO_1_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_1_1;
			break;
		case SQ_SCU_CLOCK_RATIO_2_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_2_1;
			break;
		case SQ_SCU_CLOCK_RATIO_3_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_3_1;
			break;
		case SQ_SCU_CLOCK_RATIO_4_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_4_1;
			break;
		case SQ_SCU_CLOCK_RATIO_8_1 :
			tmp = tmp |SCU_MCLKDIV_CLK_RATIO_8_1;
			break;
		default :
			SCUMSG("unknow ratio value\n");
			return -1;
			break;			
	}
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return 0;
}
	
extern int 
sq_scu_cpu_hclk_ratio_get (void)
{
	u32 tmp;
	int ratio;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & SCU_MCLKDIV_CLK_RATIO_MASK;

	switch(tmp){
		case SCU_MCLKDIV_CLK_RATIO_1_1 :
			ratio = 1;
			break;
		case SCU_MCLKDIV_CLK_RATIO_2_1 :
			ratio = 2;
			break;
		case SCU_MCLKDIV_CLK_RATIO_3_1 :
			ratio = 3;
			break;
		case SCU_MCLKDIV_CLK_RATIO_4_1 :
			ratio = 4;
			break;
		case SCU_MCLKDIV_CLK_RATIO_8_1 :
			ratio = 8;
			break;
		default :
			SCUMSG("unknow ratio value\n");
			return -1;
			break;
	}	
	
	return ratio;
	
}



extern void 
sq_scu_sw_reset(void)
{
	sq_scu_write(sq_scu_chip_id(), SQ_SCU_SWRST);

}


extern void 
sq_scu_remap(int arg)
{
	sq_scu_write(sq_scu_chip_id(), SQ_SCU_REMAP);
}

	


/*	SCU_INFORM0	*/
	/*	User defined information register	*/
extern void 
sq_scu_info_set (int index, u32 info)	
{
	sq_scu_write(info, SQ_SCU_INFORM0 + index*4);
			
	return ;
}		

extern u32 
sq_scu_info_get (int index)						//return information0 value
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_INFORM0 + index*4);
			
	return tmp;
}		




extern u32
sq_scu_cpu_clock_get (void)
{	
	/*	get power mode */
	if(0 == socle_scu_mpll_status_get())
		socle_clock.cpu_clock = MPLL_XIN;	
	else
		socle_clock.cpu_clock = sq_scu_mpll_clock_get();
				
	return socle_clock.cpu_clock ;
}

extern u32
sq_scu_ahb_clock_get (void)
{
	int ratio;
	
	ratio = sq_scu_cpu_hclk_ratio_get();

	socle_clock.ahb_clock = sq_scu_cpu_clock_get() / ratio;
	
	return socle_clock.ahb_clock;
}

extern u32
sq_scu_apb_clock_get (void)
{
	socle_clock.apb_clock = sq_scu_ahb_clock_get() / 2;
	
	return socle_clock.apb_clock;
}


extern u32
sq_scu_uart_clock_get (int uart)
{
	int tmp;
	int div;

	/*	get power mode */
	tmp = socle_scu_uart_clk_src_get(uart);
	switch(tmp){
		case SQ_SCU_UART_CLK_24 :
			socle_clock.uart_clock = MPLL_XIN;
			return socle_clock.uart_clock;
			break;
		case SQ_SCU_UART_CLK_UPLL:
			div = 1;
			break;
		case SQ_SCU_UART_CLK_UPLL_2:
			div = 2;
			break;
		case SQ_SCU_UART_CLK_UPLL_4:
			div = 4;
			break;
		default :
			return -1;
			break;
	}
	
	socle_clock.uart_clock = sq_scu_upll_clock_get() / div;
					
	return socle_clock.uart_clock ;
}


extern void
sq_scu_show_system_info (void)
{
	sq_scu_apb_clock_get();
	
	SCUMSG("CPU = %d MHz , HCLCK = %d MHz ",
		socle_clock.cpu_clock/1000000, socle_clock.ahb_clock/1000000);

        if(sq_scu_sdram_bus_width_status() == SQ_SCU_SDRAM_BUS_WIDTH_32)
                SCUMSG("(32bit SDR) \n");
        else
                SCUMSG("(16bit SDR) \n");
		
        return ;
}





extern int 
sq_scu_hardmacro_chip_stop_mode_disable (int chip)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_ACLKEN);
	
	switch(chip){
		case SQ_SCU_SDC_HARDMACRO :
			tmp = tmp | SCU_ACLK_SDC_HARDMACRO;
			break;
		case SQ_SCU_MAC_HARDMACRO :
			tmp = tmp | SCU_ACLK_MAC_HARDMACRO;
			break;
		case SQ_SCU_LCDC_HARDMACRO :
			tmp = tmp | SCU_ACLK_LCDC_HARDMACRO;
			break;
		case SQ_SCU_OTG0_HARDMACRO :
			tmp = tmp | SCU_ACLK_OTG0_HARDMACRO;
			break;
		case SQ_SCU_OTG1_HARDMACRO :
			tmp = tmp | SCU_ACLK_OTG1_HARDMACRO;
			break;
		case SQ_SCU_NFC_HARDMACRO :
			tmp = tmp | SCU_ACLK_NFC_HARDMACRO;
			break;
		case SQ_SCU_VIP_HARDMACRO :
			tmp = tmp | SCU_ACLK_VIP_HARDMACRO;
			break;
		case SQ_SCU_VOP_HARDMACRO :
			tmp = tmp | SCU_ACLK_VOP_HARDMACRO;
			break;
		default :
			SCUMSG("unknow chip number\n");
			return -1;
			break;
	}		

	sq_scu_write(tmp, SQ_SCU_ACLKEN);	
	
	return 0;
}

extern int 
sq_scu_hardmacro_chip_stop_mode_no_pwr_control (int chip)
{	
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_ACLKEN);
	
	switch(chip){
		case SQ_SCU_SDC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_SDC_HARDMACRO;
			break;
		case SQ_SCU_MAC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_MAC_HARDMACRO;
			break;
		case SQ_SCU_LCDC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_LCDC_HARDMACRO;
			break;
		case SQ_SCU_OTG0_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_OTG0_HARDMACRO;
			break;
		case SQ_SCU_OTG1_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_OTG1_HARDMACRO;
			break;
		case SQ_SCU_NFC_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_NFC_HARDMACRO;
			break;
		case SQ_SCU_VIP_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_VIP_HARDMACRO;
			break;
		case SQ_SCU_VOP_HARDMACRO :
			tmp = tmp & ~SCU_ACLK_VOP_HARDMACRO;
			break;
		default :
			SCUMSG("unknow chip number\n");
			return -1;
			break;
	}		

	sq_scu_write(tmp, SQ_SCU_ACLKEN);	
	
	return 0;
}	
		


// Device control 
extern int 
sq_scu_dev_enable(u32 dev)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	
	switch(dev){
		case SQ_DEVCON_NFC :
			tmp = tmp | SCU_DEVCON_NFC_GPIO;
			break;
		case SQ_DEVCON_MAC :
			tmp = tmp | SCU_DEVCON_MAC_GPIO;
			break;
		case SQ_DEVCON_TMR :
			tmp = tmp | SCU_DEVCON_TMR_GPIO;
			break;
		case SQ_DEVCON_PWM0 :
			tmp = tmp | SCU_DEVCON_PWM0_GPIO;
			break;
		case SQ_DEVCON_PWM1 :
			tmp = tmp | SCU_DEVCON_PWM1_GPIO;
			break;
		case SQ_DEVCON_EXT_INT0 :
			tmp = (tmp & ~SCU_DEVCON_INT0) | SCU_DEVCON_INT0_EXT_INT0;
			break;
		case SQ_DEVCON_EXT_INT0_NFIQ :
			tmp = (tmp & ~SCU_DEVCON_INT0) | SCU_DEVCON_INT0_NFIQ;
			break;
		case SQ_DEVCON_EXT_INT1 :
			tmp = tmp | SCU_DEVCON_INT1;
			break;
		case SQ_DEVCON_LCDC :
			tmp = (tmp & ~SCU_DEVCON_LCD_GPIO) | SCU_DEVCON_LCD_GPIO_LCD;
			break;
		case SQ_DEVCON_LCDC_VOP :
			tmp = (tmp & ~SCU_DEVCON_LCD_GPIO) | SCU_DEVCON_LCD_GPIO_VOP;
			break;
		case SQ_DEVCON_VIP :
			tmp = tmp | SCU_DEVCON_VIP_GPIO;
			break;
		case SQ_DEVCON_SPI0 :
			tmp = tmp | SCU_DEVCON_SPI0_GPIO;
			break;
		case SQ_DEVCON_SPI1 :
			tmp = tmp | SCU_DEVCON_SPI1_GPIO;
			break;
		case SQ_DEVCON_I2S_TX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_TX;
			break;
		case SQ_DEVCON_I2S_RX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_RX;
			break;
		case SQ_DEVCON_I2S_TX_RX :
			tmp = (tmp & ~SCU_DEVCON_I2S_GPIO) | SCU_DEVCON_I2S_GPIO_TX_RX;
			break;
		case SQ_DEVCON_I2C :
			tmp = tmp | SCU_DEVCON_I2C_GPIO;
			break;
		case SQ_DEVCON_SDMMC :
			tmp = tmp | SCU_DEVCON_SDMMC_GPIO;
			break;
		case SQ_DEVCON_UART0 :
			tmp = tmp | SCU_DEVCON_UART0_GPIO;
			break;
		case SQ_DEVCON_UART1 :
			tmp = tmp | SCU_DEVCON_UART1_GPIO;
			break;
		case SQ_DEVCON_UART2 :
			tmp = tmp | SCU_DEVCON_UART2_GPIO;
			break;
		default :
			SCUMSG("unknow IP number\n");
			return -1;
			break;
	}
				
	sq_scu_write(tmp, SQ_SCU_DEVCON);	

	return 0;
}

	
extern int 
sq_scu_dev_disable(u32 dev)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	
	switch(dev){
		case SQ_DEVCON_NFC :
			tmp = tmp & ~SCU_DEVCON_NFC_GPIO;
			break;
		case SQ_DEVCON_MAC :
			tmp = tmp & ~SCU_DEVCON_MAC_GPIO;
			break;
		case SQ_DEVCON_TMR :
			tmp = tmp & ~SCU_DEVCON_TMR_GPIO;
			break;
		case SQ_DEVCON_PWM0 :
			tmp = tmp & ~SCU_DEVCON_PWM0_GPIO;
			break;
		case SQ_DEVCON_PWM1 :
			tmp = tmp & ~SCU_DEVCON_PWM1_GPIO;
			break;
		case SQ_DEVCON_EXT_INT0 :
			tmp = tmp & ~SCU_DEVCON_INT0;
			break;
		case SQ_DEVCON_EXT_INT0_NFIQ :
			tmp = tmp & ~SCU_DEVCON_INT0;
			break;
		case SQ_DEVCON_EXT_INT1 :
			tmp = tmp & ~SCU_DEVCON_INT1;
			break;
		case SQ_DEVCON_LCDC :
			tmp = tmp & ~SCU_DEVCON_LCD_GPIO;
			break;
		case SQ_DEVCON_LCDC_VOP :
			tmp = tmp & ~SCU_DEVCON_LCD_GPIO;
			break;
		case SQ_DEVCON_VIP :
			tmp = tmp & ~SCU_DEVCON_VIP_GPIO;
			break;
		case SQ_DEVCON_SPI0 :
			tmp = tmp & ~SCU_DEVCON_SPI0_GPIO;
			break;
		case SQ_DEVCON_SPI1 :
			tmp = tmp & ~SCU_DEVCON_SPI1_GPIO;
			break;
		case SQ_DEVCON_I2S_TX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SQ_DEVCON_I2S_RX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SQ_DEVCON_I2S_TX_RX :
			tmp = tmp & ~SCU_DEVCON_I2S_GPIO;
			break;
		case SQ_DEVCON_I2C :
			tmp = tmp & ~SCU_DEVCON_I2C_GPIO;
			break;
		case SQ_DEVCON_SDMMC :
			tmp = tmp & ~SCU_DEVCON_SDMMC_GPIO;
			break;
		case SQ_DEVCON_UART0 :
			tmp = tmp & ~SCU_DEVCON_UART0_GPIO;
			break;
		case SQ_DEVCON_UART1 :
			tmp = tmp & ~SCU_DEVCON_UART1_GPIO;
			break;
		case SQ_DEVCON_UART2 :
			tmp = tmp & ~SCU_DEVCON_UART2_GPIO;
			break;
		default :
			SCUMSG("unknow IP number\n");
			return -1;
			break;
	}
				
	sq_scu_write(tmp, SQ_SCU_DEVCON);	
	
	return 0;

}


/*	MCLKDIV		*/

	/* PLL lock period	*/
extern void 
sq_scu_pll_lock_period_set (int period)	
{
	u32 tmp;

	if(period < 2)
		period = 2;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & ~SCU_MCLKDIV_PLL_LOCK_PERIOD_M;
	tmp = tmp | period;
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return ;
}	

extern int 
sq_scu_pll_lock_period_get ()	
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & SCU_MCLKDIV_PLL_LOCK_PERIOD_M;
	tmp = tmp >> SCU_MCLKDIV_PLL_LOCK_PERIOD_S;
	
	return tmp;
}	

extern void 
sq_scu_adc_clk_div_set (int div)	
{
	u32 tmp;

	if(div < 2)
		div = 2;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & ~SCU_MCLKDIV_ADC_CLK_DIV_M;
	tmp = tmp | div;
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return ;
}	

extern int 
sq_scu_adc_clk_div_get ()	
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV) & SCU_MCLKDIV_ADC_CLK_DIV_M;
	tmp = tmp >> SCU_MCLKDIV_ADC_CLK_DIV_S;
	
	return tmp;
}	

extern int 
sq_scu_uart_clk_24_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_24 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			SCUMSG("unknow UART index\n");
			return -1;
			break;			
	}
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return 0;
}

extern int 
sq_scu_uart_clk_upll_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			SCUMSG("unknow UART index\n");
			return -1;
			break;			
	}
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return 0;
}

extern int 
sq_scu_uart_clk_upll_2_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_2 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			SCUMSG("unknow UART index\n");
			return -1;
			break;			
	}
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);
	
	return 0;
}

extern int 
sq_scu_uart_clk_upll_4_set (int uart)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_MCLKDIV);

	switch(uart){
		case 0 :
			tmp = (tmp & ~SCU_MCLKDIV_UART0_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART0_CLK_S);
			break;
		case 1:
			tmp = (tmp & ~SCU_MCLKDIV_UART1_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART1_CLK_S);
			break;
		case 2:
			tmp = (tmp & ~SCU_MCLKDIV_UART2_CLK_M) | (SCU_MCLKDIV_UART_CLK_UPLL_4 << SCU_MCLKDIV_UART2_CLK_S);
			break;
		default :
			SCUMSG("unknow UART index\n");
			return -1;
			break;			
	}
	sq_scu_write(tmp, SQ_SCU_MCLKDIV);

	return 0;
	
}


extern void 
sq_scu_lcdc_clk_input_mpll_outpput(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_LCD_CLK_MPLL_OUTPUT;

	sq_scu_write(tmp, SQ_SCU_DEVCON);

}

extern void 
sq_scu_lcdc_clk_input_mpll_xin(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_LCD_CLK_MPLL_OUTPUT;

	sq_scu_write(tmp, SQ_SCU_DEVCON);

}


extern void 
sq_scu_hdma_req45_spi0(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_HDMA45_SPI1;

	sq_scu_write(tmp, SQ_SCU_DEVCON);

}

extern void 
sq_scu_hdma_req45_spi1(void)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp |= SCU_DEVCON_HDMA45_SPI1;

	sq_scu_write(tmp, SQ_SCU_DEVCON);

}

extern int 
sq_scu_hdma_req01_uart(int uart)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UART_HDMA01_M;
	switch(uart){
		case 0:
			tmp |= (UART0_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		case 1:
			tmp |= (UART1_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		case 2:
			tmp |= (UART2_WITH_HDMA << SCU_DEVCON_UART_HDMA01_S);
			break;
		default :
			SCUMSG("unknow uart number\n");
			return -1;
			break;			
	}

	sq_scu_write(tmp, SQ_SCU_DEVCON);
	
	return 0;
}

extern int 
sq_scu_hdma_req23_uart(int uart)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	tmp &= ~SCU_DEVCON_UART_HDMA23_M;
	switch(uart){
		case 0:
			tmp |= (UART0_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		case 1:
			tmp |= (UART1_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		case 2:
			tmp |= (UART2_WITH_HDMA << SCU_DEVCON_UART_HDMA23_S);
			break;
		default :
			SCUMSG("unknow uart number\n");
			return -1;
			break;			
	}

	sq_scu_write(tmp, SQ_SCU_DEVCON);
	
	return 0;
}

extern void 
sq_scu_wdt_reset_enable(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	
	if(en == 1)
		tmp &= ~ SCU_DEVCON_WDT_RST;
	else
		tmp |= SCU_DEVCON_WDT_RST;
		
	sq_scu_write(tmp, SQ_SCU_DEVCON);	
}

extern void 
sq_scu_sw_reset_enable(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	
	if(en == 1)
		tmp &= ~SCU_DEVCON_SW_RST;
	else
		tmp |= SCU_DEVCON_SW_RST;
		
	SCUDBUG("SCU_DEVCON_SW_RST = 0x%08x, tmp = 0x%08x\n", SCU_DEVCON_SW_RST, tmp);
		
	sq_scu_write(tmp, SQ_SCU_DEVCON);	
}





//Power Mode setting 
	/*	SDRAM data bus width status	*/
extern int 
sq_scu_sdram_bus_width_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_SDR_WIDTH;

	if(tmp == SCU_PWMCON_SDR_WIDTH_32)
		return SQ_SCU_SDRAM_BUS_WIDTH_32;
	else
		return SQ_SCU_SDRAM_BUS_WIDTH_16;
	
}

	/*	DCM mode setting status	*/
extern int 
sq_scu_dcm_mode_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_DCM_MODE;

	if(tmp == SCU_PWMCON_DCM_MODE_DCM)
		return SQ_SCU_DCM_MODE_DCM;
	else
		return SQ_SCU_DCM_MODE_NOR;
	
}
	
	/*	TPS MAC status	*/
extern int 
sq_scu_tps_mac_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_TPS_MAC;

	if(tmp == SCU_PWMCON_TPS_MAC)
		return 1;
	else
		return 0;
	
}
	
	/*	RPS MAC status	*/
extern int 
socle_scu_rps_mac_status (void)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_RPS_MAC;

	if(tmp == SCU_PWMCON_RPS_MAC)
		return 1;
	else
		return 0;
	
}
	
	/*	Boot source selection status	*/
extern int 
sq_scu_boot_source_status (void)
{
	u32 tmp, status=0;

	tmp = socle_scu_read(SQ_SCU_PWMCON) & SCU_PWMCON_BOOT_MODE;

	switch(tmp){
		case SCU_PWMCON_BOOT_MODE_NOR_16 :
			status = SQ_SCU_BOOT_NOR_16;
			break;
		case SCU_PWMCON_BOOT_MODE_NOR_8 :
			status = SQ_SCU_BOOT_NOR_8;
			break;
		case SCU_PWMCON_BOOT_MODE_NAND :
			status = SQ_SCU_BOOT_NAND;
			break;
		case SCU_PWMCON_BOOT_MODE_ISP:
			status = SQ_SCU_BOOT_ISP_ROM;
			break;
	}
		return status;
		
}


	/*	Stand by wait for interrupt */
extern void 
sq_scu_pw_standbywfi_enable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON);

	if(i ==1)
		sq_scu_write(tmp | SCU_PWMCON_STANDBYWFI , SQ_SCU_PWMCON);
	else 
		sq_scu_write(tmp & ~SCU_PWMCON_STANDBYWFI , SQ_SCU_PWMCON);
	
	return ;
}

	/*	stop mode -- systen clock	*/
extern void 
sq_scu_stop_mode_enable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON);

	if(i == 1)
		sq_scu_write(tmp | SCU_PWMCON_PWR_STOP , SQ_SCU_PWMCON);
	else 
		sq_scu_write(tmp & ~SCU_PWMCON_PWR_STOP , SQ_SCU_PWMCON);

	return ;
}

	/*	slow mode -- systen clock	*/
extern void 
sq_scu_slow_mode_disable (int i)
{
	u32 tmp;

	tmp = socle_scu_read(SQ_SCU_PWMCON);

	if(i == 1)
		sq_scu_write(tmp & ~SCU_PWMCON_PWR_NOR, SQ_SCU_PWMCON);
	else 
		sq_scu_write(tmp | SCU_PWMCON_PWR_NOR, SQ_SCU_PWMCON);

	return ;
}


extern void 
sq_scu_nfiq_polarity_high(int en)
{
	u32 tmp;
	
	tmp = socle_scu_read(SQ_SCU_DEVCON) ;
	
	if(en == 1)
		tmp |= SCU_DEVCON_FIQ_POLAR_HIGH;
	else
		tmp &= ~SCU_DEVCON_FIQ_POLAR_HIGH;
		
	sq_scu_write(tmp, SQ_SCU_DEVCON);	
}

