#include <platform.h>
#include <irqs.h>

/* Set the IP's base address */
#ifdef SQ_LCD0
#define SOCLE_LCD_BASE	SQ_LCD0
#else
#define SOCLE_LCD_BASE	SOCLE_ES3
//#error "LCD IP base address is not defined"
#endif

/* Set the IP's irq */
 
#ifndef SOCLE_LCD_IRQ
#ifdef SQ_INTC_LCD0
#define SOCLE_LCD_IRQ	SQ_INTC_LCD0
#else
//#define SOCLE_LCD_IRQ	SOCLE_INTC_EXTERNAL2
#define SOCLE_LCD_IRQ	SOCLE_INTC_EXTERNAL2
//#error "LCD IRQ is not defined"
#endif
#endif

/* Set IP version dependance */

#if defined(CONFIG_PC9223)
#define VGA_OUT 0
#define SDTV_OUT 1
#define HDMI_OUT 0
#else
#define VGA_OUT 0
#define SDTV_OUT 0
#define HDMI_OUT 0
#endif

 
#define PAGE0_ADDR		(SQ_MEMORY_ADDR_START + 0x1000000)
#define PAGE1_ADDR		(SQ_MEMORY_ADDR_START + 0x1200000)
#define LUT_ADDR		(SQ_MEMORY_ADDR_START + 0x1400000)
