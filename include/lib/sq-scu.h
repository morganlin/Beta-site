            /********************************************************************************
* File Name     : include/configs/sq-scu.h
* Author        : Leonid Cheng
* Description   : SQ SCU Service Header
*
* Copyright (C) SQ Tech. Corp.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by the Free Software Foundation;
* either version 2 of the License, or (at your option) any later version.
* This program is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*   Version      : 2,0,0,1
*   History      :
*      1. 2009/08/03 Leonid cheng create this file
*
********************************************************************************/

#ifndef __SQ_SCU_H_INCLUDED
#define __SQ_SCU_H_INCLUDED


extern u32 sq_scu_cpu_clock_get (void);			//return CPU clock (Hz)
extern u32 sq_scu_ahb_clock_get (void);			//return AHB clock (Hz)
extern u32 sq_scu_apb_clock_get (void);			//return APB clock (Hz)
extern u32 sq_scu_uart_clock_get (int uart);			//return UART clock (Hz)
extern void sq_scu_show_system_info (void);		


extern int sq_scu_cpu_hclk_ratio_get (void); 				//return cpu/hclk ratio value
extern int sq_scu_cpu_hclk_ratio_set (int ratio);			//input ratio index, return 0:success -1:fail

//clock enable
extern int sq_scu_hclk_enable (int hclk, int en);			//input hclk index and enable(1:enable, 0:disable),	return 0:success -1:fail
extern int sq_scu_pclk_enable (int pclk, int en);			//input pclk index and enable(1:enable, 0:disable),	return 0:success -1:fail
extern int sq_scu_aclk_enable (int aclk, int en);			//input aclk index and enable(1:enable, 0:disable),	return 0:success -1:fail

/*	SCU_INFORM	*/
extern u32 sq_scu_info_get (int index);						//input info index(0~3), return info value or -1(fail)
extern void sq_scu_info_set (int index, u32 info);			//input info index(0~3) & info value, return 0:success -1:fail 

extern int sq_scu_mpll_clock_set (int mpll_clock);					//input mpll_clock index, retuen 0:success -1:fail
extern u32 sq_scu_mpll_clock_get (void);						//return mpll clock value
extern void sq_scu_mpll_power_status_set (int act);					//input mpll power status, 1:act 0:power down
extern int sq_scu_mpll_power_status_get (void);						//return mpll power status, 1:act 0:power down

extern int sq_scu_upll_clock_set (int upll_clock);					//input upll_clock index, retuen 0:success -1:fail
extern u32 sq_scu_upll_clock_get (void);						//return upll clock value
extern void sq_scu_upll_power_status_set (int act);					//input upll power status, 1:act 0:power down
extern int sq_scu_upll_power_status_get (void);						//return upll power status, 1:act 0:power down

//sw remap
extern void sq_scu_remap (int arg);
//sw reset
extern void sq_scu_sw_reset (void);
//cpu chip id
extern u32 sq_scu_chip_id (void);


#endif
