#include <platform.h>
#include <irqs.h>

//#define SKIP_AUTO_BOOT_TEST

/******************************************************************************/
// IP Configurtation Parameters

#if defined(CONFIG_PC9223)
#define DMA_IMPLEMENT                           0
#define BCH4_IMPLEMENT                          0
#define BCH8_IMPLEMENT                          0
#define BCH16_IMPLEMENT                 	0
#define HM_IMPLEMENT                            0
#define PAGE_2K_IMPLEMENT                       1
#define PAGE_4K_IMPLEMENT                       0
#define FLLOOKUP_IMPLEMENT                      1
#define PROTECT_IMPLEMENT                       1
#define NFDATA_WIDTH8                           1
#define NFDATA_WIDTH32		                0
#define TIMER_BITS                              5
#define PSN_IMPLEMENT				0
#elif defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD)				// For MDK3D platfrom FPGA test
#define DMA_IMPLEMENT				1
#define BCH4_IMPLEMENT				1
#define BCH8_IMPLEMENT				0
#define BCH16_IMPLEMENT			0
#define HM_IMPLEMENT				0
#define PAGE_2K_IMPLEMENT			1
#define PAGE_4K_IMPLEMENT			0
#define FLLOOKUP_IMPLEMENT			1
#define PROTECT_IMPLEMENT			1
#define NFDATA_WIDTH8				1
#define NFDATA_WIDTH32			0
#define TIMER_BITS					7
#else
#define DMA_IMPLEMENT                           1
#define BCH4_IMPLEMENT                          1
#define BCH8_IMPLEMENT                          0
#define BCH16_IMPLEMENT                 	0
#define HM_IMPLEMENT                            0
#define PAGE_2K_IMPLEMENT                       0
#define PAGE_4K_IMPLEMENT                       1
#define FLLOOKUP_IMPLEMENT                      1
#define PROTECT_IMPLEMENT                       1
#define NFDATA_WIDTH8                           1
#define NFDATA_WIDTH32                  	0
#define TIMER_BITS                              7
#define PSN_IMPLEMENT				1
#endif

