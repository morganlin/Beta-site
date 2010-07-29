#include <platform.h>
#include <irqs.h>

/* Set the IP's base address */
#ifdef SOCLE_LCD0
#define SOCLE_LCD_BASE	SOCLE_LCD0
#else
#define SOCLE_LCD_BASE	SOCLE_ES3
//#error "LCD IP base address is not defined"
#endif

/* Set the IP's irq */
 
#ifndef SOCLE_LCD_IRQ
#ifdef SOCLE_INTC_LCD0
#define SOCLE_LCD_IRQ	SOCLE_INTC_LCD0
#else
//#define SOCLE_LCD_IRQ	SOCLE_INTC_EXTERNAL2
#define SOCLE_LCD_IRQ	SOCLE_INTC_EXTERNAL2
//#error "LCD IRQ is not defined"
#endif
#endif

/* Set IP version dependance */

#ifdef CONFIG_PC9220
#define VGA_OUT 0
#define SDTV_OUT 0
#define HDMI_OUT 0
#elif defined(CONFIG_PC9223)
#define VGA_OUT 0
#define SDTV_OUT 1
#define HDMI_OUT 0
#elif defined(CONFIG_MDK3D)
#define VGA_OUT 1
#define SDTV_OUT 1
#define HDMI_OUT 1
#else
#define VGA_OUT 0
#define SDTV_OUT 0
#define HDMI_OUT 0
#endif

 
#define PAGE0_ADDR		(SOCLE_MEMORY_ADDR_START + 0x1000000)
#define PAGE1_ADDR		(SOCLE_MEMORY_ADDR_START + 0x1200000)
#define LUT_ADDR			(SOCLE_MEMORY_ADDR_START + 0x1400000)
