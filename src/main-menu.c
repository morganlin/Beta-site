#ifndef CONFIG_ROM

#include "test_item.h"

//Test entry for all IPs
extern int vfp_test (int autotest);
extern int gpio_test(int autotest);
extern int socle_mp_gpio_test(int autotest);
extern int memory_test(int autotest);
extern int cache_test(int autotest);
extern int intr_test(int autotest);
extern int socle_vic_test(int autotest);
extern int uart_main_test(int autotest);
extern int timer_test(int autotest);
extern int wdt_test(int autotest);
extern int rtc_test(int autotest);
extern int spi_test(int autotest);
extern int udc_test(int autotest);
extern int pci_test(int autotest);
extern int pcie_test(int autotest);
extern int mac_test(int autotest);
extern int extend_board_test(int autotest);
extern int extend_hdma_test(int autotest);
extern int hdma_test(int autotest);
extern int panther7_hdma_test(int autotest);
extern int multi_mem_test(int autotest);
extern int i2c_test(int autotest);
extern int mpeg4_test (int autotest);
extern int sd_test(int autotest);
extern int sdhc_host_test(int autotest);
extern int sdhc_slave_test(int autotest);
extern int a2a_test(int autotest);
extern int nor_flash_test(int autotest);
extern int nor_mxic_test(int autotest);
extern int nand_test(int autotest);
extern int nfc_ctrl_test(int autotest);
extern int nfc_ctrl_fpga_test(int autotest);
extern int ehci_main_test(int autotest);
extern int ohci_main_test(int autotest);
extern int ide_test(int autotest);
extern int mailbox_test(int autotest);
extern int i2s_test(int autotest);
extern int scu_test(int autotest);
extern int lcd_test(int autotest);
extern int pwm_test(int autotest);
extern int adc_test(int autotest);
extern int vip_vop_test(int autotest);
extern int hdvip_test(int autotest);
extern int lcm_test(int autotest);
extern int power_save_test(int autotest);
extern int otg_test(int autotest);
extern int mfc_test(int autotest);
extern int gpu_test(int autotest);
extern int gmac_test(int autotest);
extern int dmac_test(int autotest);
extern int ac97_test(int autotest);

struct test_item main_test_items [] = {	
#ifdef	CONFIG_GPIO
	{
		"GPIO Testing",
		gpio_test,
		1,
		1
	},
#endif

#ifdef CONFIG_MP_GPIO
	{
		"MP GPIO Testing",
		socle_mp_gpio_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_MEMORY
	{
		"Memory Testing",
		memory_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_CACHE
	{
		"CACHE Testing",
		cache_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_INTERRUPT
#ifdef	CONFIG_VIC
	{
		"VIC Testing",
		socle_vic_test,
		1,
		1
	},
#else
	{
		"Interrupt Testing",
		intr_test,
		1,
		1
	},
#endif
#endif
	
#ifdef	CONFIG_UART
	{
		"UART Testing",
		uart_main_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_TIMER
	{
		"Timer Testing",
		timer_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_RTC
	{
		"RTC Testing",
		rtc_test,
		1,
		1
	},
#endif

#ifdef CONFIG_SPI
	{
		"SPI Testing",
		spi_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_UDC
	{	
		"UDC Testing",
		udc_test,
		0,
		1
	},
#endif

#ifdef	CONFIG_PCI
	{
		"PCI Testing",
		pci_test,
		1,
		1
	},
#endif

#ifdef CONFIG_PCI_EXPRESS
	{
		"PCI Express Testing",
		pcie_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_MAC
	{
		"MAC Testing",
		mac_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_PSEUDO_EXTBOARD
	{
		"Ext Board Pseudo Device Testing",
		extend_board_test,
		1,
		1
	},
#endif

#ifdef  CONFIG_HDMA_EXTBOARD
	{
		"Ext Board HDMA Testing",
		extend_hdma_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_HDMA
	{
		"HDMA Testing",
		hdma_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_DMAC 
	{
		"PL080 DMAC Testing",
		dmac_test,
		1,
		1
	},
#endif
#ifdef CONFIG_PANTHER7_HDMA
	{
		"HDMA Testing for Panther7",
		panther7_hdma_test,
		1,
		1
	},
#endif

#ifdef  CONFIG_MULTI_MEM
	{
		"Multi-port MEM Testing",
		multi_mem_test,
		1,
		1
	},
#endif

#ifdef  CONFIG_I2S
        {
		"I2S Testing",
		i2s_test,
		1,
		1
        },
#endif


#ifdef	CONFIG_I2C
	{
		"I2C Testing",
		i2c_test,
		1,
		1
	},
#endif

#ifdef CONFIG_MPEG4
	{
		"MPEG4 Testing",
		mpeg4_test,
		1,
		1
	},
#endif

#ifdef CONFIG_SDMMC
	{
		"SDMMC Testing",
		sd_test,
		1,
		1
	},
#endif

#ifdef CONFIG_SDHC_HOST
	{
		"SDHC Host Testing",
		sdhc_host_test,
		1,
		1
	},
#endif

#ifdef CONFIG_SDHC_SLAVE
	{
		"SDHC Slave Testing",
		sdhc_slave_test,
		0,
		1
	},
#endif

#ifdef CONFIG_A2ADMA
	{
		"A2A DMA Testing",
		a2a_test,
		1,
		1
	},
#endif

#ifdef CONFIG_NOR
	{
		"NOR-FLASH Testing",
		nor_flash_test,
		1,
		1
	},
#endif

#ifdef CONFIG_NOR_MXIC
	{
		"NOR-FLASH-MXIC Testing",
		nor_mxic_test,
		1,
		1
	},
#endif

#ifdef CONFIG_NAND
	{
		"NAND-FLASH Testing",
		nand_test,
		1,
		1
	},
#endif

#ifdef CONFIG_NAND_NFC
	{
		"NAND-FLASH NFC Testing",
		nfc_ctrl_test,
		1,
		1
	},
#endif

#ifdef CONFIG_EHCI
	{
		"EHCI Testing",
		ehci_main_test,
		1,
		1
	},
#endif

#ifdef CONFIG_OHCI
	{
		"OHCI Testing",
		ohci_main_test,
		1,
		1
	},
#endif

#ifdef CONFIG_IDE
	{
		"IDE Testing",
		ide_test,
		1,
		1
	},
#endif

#ifdef CONFIG_MAILBOX
	{
		"MailBox Testing",
		mailbox_test,
		0,
		1
	},
#endif

#ifdef CONFIG_SCU
	{
		"SCU Testing",
		scu_test,
		1,
		1
	},
#endif

/* added by morganlin */
#ifndef GDR_LCD
#ifdef CONFIG_LCD
	{
		"LCD Testing",
		lcd_test,
		1,
		1
	},
#endif
#endif

#ifdef CONFIG_PWM
	{
		"PWMT Testing",
		pwm_test,
		1,
		1
	},
#endif

#ifdef CONFIG_ADC
	{
		"ADC Testing",
		adc_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_VIOP
	{
		"VIOP Testing",
		vip_vop_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_HDVIP
	{
		"HDVIP Testing",
		hdvip_test,
		1,
		1
	},
#endif

#ifdef	CONFIG_LCM
	{
		"LCM Testing",
		lcm_test,
		1,
		1
	},
#endif

#ifdef CONFIG_7230_PS
	{
		"7230 POWER SAVING Testing",
		power_save_test,
		1,
		1
	},
#endif

#ifdef CONFIG_OTG
        {
                "OTG Testing",
                otg_test,
                1,
                1
        },
#endif

#ifdef CONFIG_GPU
        {
                "GPU Testing",
                gpu_test,
                1,
                1
        },
#endif

#ifdef CONFIG_MFC
        {
                "MFC Testing",
                mfc_test,
                1,
                1
        },
#endif

#ifdef CONFIG_GMAC
        {
                "GMAC Testing",
                gmac_test,
                1,
                1
        },
#endif

#ifdef CONFIG_AC97
        {
                "AC97 Testing",
                ac97_test,
                1,
                1
        },
#endif

///////////////////////////////////////////////
// WDT must be put last to reboot the board!!
#ifdef	CONFIG_WDT
	{
		"WDT Testing",
		wdt_test,
		1,
		1
	},
#endif

};

struct test_item_container main_test_item_container = {
#if defined (CONFIG_PC9223)
        "PDK-PC9223 Development Kit Test Program",
        "PDK-PC9223",
#else
#error "PLATFORM is not well defined!"
#endif
	.items = main_test_items,
	.test_item_size = sizeof(main_test_items)
};

#endif	/* CONFIG_ROM */

