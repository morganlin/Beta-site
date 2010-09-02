#ifndef __SQ_DIAG_CONFIG_H
#define __SQ_DIAG_CONFIG_H
/*
 * Automatically generated header file: don't edit
 */


/*
 *  Socle Platforms
 */

#define CONFIG_PC9223 1
#define ENABLE_PC9223 1
#define USE_PC9223(...)  __VA_ARGS__
#define SKIP_PC9223(...)



/*
 *  Socle CPUs
 */

#define CONFIG_ARM9 1
#define ENABLE_ARM9 1
#define USE_ARM9(...)  __VA_ARGS__
#define SKIP_ARM9(...)



/*
 *  Socle CPU Endians
 */

#undef CONFIG_BIG_ENDIAN
#define ENABLE_BIG_ENDIAN 0
#define USE_BIG_ENDIAN(...)
#define SKIP_BIG_ENDIAN(...)  __VA_ARGS__

#define CONFIG_LITTLE_ENDIAN 1
#define ENABLE_LITTLE_ENDIAN 1
#define USE_LITTLE_ENDIAN(...)  __VA_ARGS__
#define SKIP_LITTLE_ENDIAN(...)



/*
 *  Socle Interrupt
 */

#define CONFIG_INTC 1
#define ENABLE_INTC 1
#define USE_INTC(...)  __VA_ARGS__
#define SKIP_INTC(...)

#undef CONFIG_VIC
#define ENABLE_VIC 0
#define USE_VIC(...)
#define SKIP_VIC(...)  __VA_ARGS__



/*
 *  Socle IPs Test Items
 */

#define CONFIG_MEMORY 1
#define ENABLE_MEMORY 1
#define USE_MEMORY(...)  __VA_ARGS__
#define SKIP_MEMORY(...)

#undef CONFIG_MDDR
#define ENABLE_MDDR 0
#define USE_MDDR(...)
#define SKIP_MDDR(...)  __VA_ARGS__

#undef CONFIG_ITCM
#define ENABLE_ITCM 0
#define USE_ITCM(...)
#define SKIP_ITCM(...)  __VA_ARGS__

#undef CONFIG_DTCM
#define ENABLE_DTCM 0
#define USE_DTCM(...)
#define SKIP_DTCM(...)  __VA_ARGS__

#undef CONFIG_SRAM
#define ENABLE_SRAM 0
#define USE_SRAM(...)
#define SKIP_SRAM(...)  __VA_ARGS__

#undef CONFIG_DP_SRAM
#define ENABLE_DP_SRAM 0
#define USE_DP_SRAM(...)
#define SKIP_DP_SRAM(...)  __VA_ARGS__

#define CONFIG_GPIO 1
#define ENABLE_GPIO 1
#define USE_GPIO(...)  __VA_ARGS__
#define SKIP_GPIO(...)

#undef CONFIG_MP_GPIO
#define ENABLE_MP_GPIO 0
#define USE_MP_GPIO(...)
#define SKIP_MP_GPIO(...)  __VA_ARGS__

#undef CONFIG_CACHE
#define ENABLE_CACHE 0
#define USE_CACHE(...)
#define SKIP_CACHE(...)  __VA_ARGS__

#define CONFIG_INTERRUPT 1
#define ENABLE_INTERRUPT 1
#define USE_INTERRUPT(...)  __VA_ARGS__
#define SKIP_INTERRUPT(...)

#define CONFIG_UART 1
#define ENABLE_UART 1
#define USE_UART(...)  __VA_ARGS__
#define SKIP_UART(...)

#define CONFIG_TIMER 1
#define ENABLE_TIMER 1
#define USE_TIMER(...)  __VA_ARGS__
#define SKIP_TIMER(...)

#define CONFIG_WDT 1
#define ENABLE_WDT 1
#define USE_WDT(...)  __VA_ARGS__
#define SKIP_WDT(...)

#define CONFIG_RTC 1
#define ENABLE_RTC 1
#define USE_RTC(...)  __VA_ARGS__
#define SKIP_RTC(...)

#define CONFIG_SPI 1
#define ENABLE_SPI 1
#define USE_SPI(...)  __VA_ARGS__
#define SKIP_SPI(...)

#undef CONFIG_UDC
#define ENABLE_UDC 0
#define USE_UDC(...)
#define SKIP_UDC(...)  __VA_ARGS__

#undef CONFIG_PCI
#define ENABLE_PCI 0
#define USE_PCI(...)
#define SKIP_PCI(...)  __VA_ARGS__

#undef CONFIG_PCI_EXPRESS
#define ENABLE_PCI_EXPRESS 0
#define USE_PCI_EXPRESS(...)
#define SKIP_PCI_EXPRESS(...)  __VA_ARGS__

#define CONFIG_MAC 1
#define ENABLE_MAC 1
#define USE_MAC(...)  __VA_ARGS__
#define SKIP_MAC(...)

#undef CONFIG_HDMA
#define ENABLE_HDMA 0
#define USE_HDMA(...)
#define SKIP_HDMA(...)  __VA_ARGS__

#define CONFIG_PANTHER7_HDMA 1
#define ENABLE_PANTHER7_HDMA 1
#define USE_PANTHER7_HDMA(...)  __VA_ARGS__
#define SKIP_PANTHER7_HDMA(...)

#undef CONFIG_DMAC
#define ENABLE_DMAC 0
#define USE_DMAC(...)
#define SKIP_DMAC(...)  __VA_ARGS__

#undef CONFIG_MPEG4
#define ENABLE_MPEG4 0
#define USE_MPEG4(...)
#define SKIP_MPEG4(...)  __VA_ARGS__

#undef CONFIG_I2S
#define ENABLE_I2S 0
#define USE_I2S(...)
#define SKIP_I2S(...)  __VA_ARGS__

#define CONFIG_MS6335 1
#define ENABLE_MS6335 1
#define USE_MS6335(...)  __VA_ARGS__
#define SKIP_MS6335(...)

#undef CONFIG_UDA1342TS
#define ENABLE_UDA1342TS 0
#define USE_UDA1342TS(...)
#define SKIP_UDA1342TS(...)  __VA_ARGS__

#define CONFIG_I2C 1
#define ENABLE_I2C 1
#define USE_I2C(...)  __VA_ARGS__
#define SKIP_I2C(...)

#undef CONFIG_SI4703
#define ENABLE_SI4703 0
#define USE_SI4703(...)
#define SKIP_SI4703(...)  __VA_ARGS__

#undef CONFIG_F75111
#define ENABLE_F75111 0
#define USE_F75111(...)
#define SKIP_F75111(...)  __VA_ARGS__

#undef CONFIG_SDMMC
#define ENABLE_SDMMC 0
#define USE_SDMMC(...)
#define SKIP_SDMMC(...)  __VA_ARGS__

#define CONFIG_SDHC_HOST 1
#define ENABLE_SDHC_HOST 1
#define USE_SDHC_HOST(...)  __VA_ARGS__
#define SKIP_SDHC_HOST(...)

#define CONFIG_SDHC 1
#define ENABLE_SDHC 1
#define USE_SDHC(...)  __VA_ARGS__
#define SKIP_SDHC(...)

#undef CONFIG_SDHC_SLAVE
#define ENABLE_SDHC_SLAVE 0
#define USE_SDHC_SLAVE(...)
#define SKIP_SDHC_SLAVE(...)  __VA_ARGS__

#undef CONFIG_A2ADMA
#define ENABLE_A2ADMA 0
#define USE_A2ADMA(...)
#define SKIP_A2ADMA(...)  __VA_ARGS__

#undef CONFIG_NOR
#define ENABLE_NOR 0
#define USE_NOR(...)
#define SKIP_NOR(...)  __VA_ARGS__

#define CONFIG_NOR_MXIC 1
#define ENABLE_NOR_MXIC 1
#define USE_NOR_MXIC(...)  __VA_ARGS__
#define SKIP_NOR_MXIC(...)

#undef CONFIG_NAND
#define ENABLE_NAND 0
#define USE_NAND(...)
#define SKIP_NAND(...)  __VA_ARGS__

#undef CONFIG_NAND_NFC
#define ENABLE_NAND_NFC 0
#define USE_NAND_NFC(...)
#define SKIP_NAND_NFC(...)  __VA_ARGS__

#undef CONFIG_EHCI
#define ENABLE_EHCI 0
#define USE_EHCI(...)
#define SKIP_EHCI(...)  __VA_ARGS__

#undef CONFIG_OHCI
#define ENABLE_OHCI 0
#define USE_OHCI(...)
#define SKIP_OHCI(...)  __VA_ARGS__

#undef CONFIG_IDE
#define ENABLE_IDE 0
#define USE_IDE(...)
#define SKIP_IDE(...)  __VA_ARGS__

#undef CONFIG_MAILBOX
#define ENABLE_MAILBOX 0
#define USE_MAILBOX(...)
#define SKIP_MAILBOX(...)  __VA_ARGS__

#undef CONFIG_SCU
#define ENABLE_SCU 0
#define USE_SCU(...)
#define SKIP_SCU(...)  __VA_ARGS__

#define CONFIG_LCD 1
#define ENABLE_LCD 1
#define USE_LCD(...)  __VA_ARGS__
#define SKIP_LCD(...)

#undef CONFIG_PANEL35
#define ENABLE_PANEL35 0
#define USE_PANEL35(...)
#define SKIP_PANEL35(...)  __VA_ARGS__

#undef CONFIG_PANEL48
#define ENABLE_PANEL48 0
#define USE_PANEL48(...)
#define SKIP_PANEL48(...)  __VA_ARGS__

#undef CONFIG_PANEL70
#define ENABLE_PANEL70 0
#define USE_PANEL70(...)
#define SKIP_PANEL70(...)  __VA_ARGS__

#define CONFIG_PWM 1
#define ENABLE_PWM 1
#define USE_PWM(...)  __VA_ARGS__
#define SKIP_PWM(...)

#define CONFIG_BUZZER 1
#define ENABLE_BUZZER 1
#define USE_BUZZER(...)  __VA_ARGS__
#define SKIP_BUZZER(...)

#undef CONFIG_ADC
#define ENABLE_ADC 0
#define USE_ADC(...)
#define SKIP_ADC(...)  __VA_ARGS__

#undef CONFIG_VIOP
#define ENABLE_VIOP 0
#define USE_VIOP(...)
#define SKIP_VIOP(...)  __VA_ARGS__

#undef CONFIG_HDVIP
#define ENABLE_HDVIP 0
#define USE_HDVIP(...)
#define SKIP_HDVIP(...)  __VA_ARGS__

#undef CONFIG_LCM
#define ENABLE_LCM 0
#define USE_LCM(...)
#define SKIP_LCM(...)  __VA_ARGS__

#undef CONFIG_7230_PS
#define ENABLE_7230_PS 0
#define USE_7230_PS(...)
#define SKIP_7230_PS(...)  __VA_ARGS__

#define CONFIG_OTG 1
#define ENABLE_OTG 1
#define USE_OTG(...)  __VA_ARGS__
#define SKIP_OTG(...)

#undef CONFIG_GPU
#define ENABLE_GPU 0
#define USE_GPU(...)
#define SKIP_GPU(...)  __VA_ARGS__

#undef CONFIG_MFC
#define ENABLE_MFC 0
#define USE_MFC(...)
#define SKIP_MFC(...)  __VA_ARGS__

#undef CONFIG_GMAC
#define ENABLE_GMAC 0
#define USE_GMAC(...)
#define SKIP_GMAC(...)  __VA_ARGS__

#undef CONFIG_AC97
#define ENABLE_AC97 0
#define USE_AC97(...)
#define SKIP_AC97(...)  __VA_ARGS__



/*
 *  Socle Debugers
 */

#define CONFIG_SEMIHOST 1
#define ENABLE_SEMIHOST 1
#define USE_SEMIHOST(...)  __VA_ARGS__
#define SKIP_SEMIHOST(...)

#undef CONFIG_GDB
#define ENABLE_GDB 0
#define USE_GDB(...)
#define SKIP_GDB(...)  __VA_ARGS__



/*
 *  Socle UART Display
 */

#define CONFIG_UART_DISP 1
#define ENABLE_UART_DISP 1
#define USE_UART_DISP(...)  __VA_ARGS__
#define SKIP_UART_DISP(...)



/*
 *  Socle ROM Code
 */

#undef CONFIG_ROM
#define ENABLE_ROM 0
#define USE_ROM(...)
#define SKIP_ROM(...)  __VA_ARGS__

#endif /* __SQ_DIAG_CONFIG_H */
