#include <platform.h>
#include <irqs.h>

#ifdef CONFIG_SDMMC

#ifndef SOCLE_SDMMC0
#error "SDMMC IP base address is not defined"
#endif


#ifndef SOCLE_SDMMC0
#define SOCLE_SDMMC0       SOCLE_SDMMC0
#define SOCLE_SDHC_HOST_0_TEST	0
#else
#define SOCLE_SDHC_HOST_0_TEST	1
#endif

#ifndef SOCLE_SDMMC1
#define SOCLE_SDMMC1	SOCLE_SDMMC0
#define SOCLE_SDHC_HOST_1_TEST	0
#else
#define SOCLE_SDHC_HOST_1_TEST	1
#endif

#ifndef SOCLE_INTC_SDMMC0
#error "SDMMC IRQ is not defined"
#endif

#ifndef SOCLE_INTC_SDMMC1
#define SOCLE_INTC_SDMMC1	SOCLE_INTC_SDMMC0
#endif

#endif // CONFIG_SDMMC


#ifdef CONFIG_SDHC

#if defined(CONFIG_PC9223)
#define SOCLE_SDHC_HOST_SLOT0		1
#define SOCLE_SDHC_HOST_SLOT1		0
#define SOCLE_SDHC_HOST_SLOT2		0
#define SOCLE_SDHC_HOST_SLOT3		0

#else
#define SOCLE_SDHC_HOST_SLOT0		1
#define SOCLE_SDHC_HOST_SLOT1		1
#define SOCLE_SDHC_HOST_SLOT2		0
#define SOCLE_SDHC_HOST_SLOT3		0
#endif

#ifndef SOCLE_SDHC0
#if defined(CONFIG_CDK)
#define SOCLE_SDHC0	SOCLE_MP1
#else
#error "SDHC Host IP base address is not defined"
#endif
#endif

#ifndef SOCLE_SDHC0
#define SOCLE_SDHC0	SOCLE_SDHC0
#define SOCLE_SDHC_HOST_0_TEST	0
#else
#define SOCLE_SDHC_HOST_0_TEST	1
#endif

#ifndef SOCLE_SDHC1
#define SOCLE_SDHC1	SOCLE_SDHC0
#define SOCLE_SDHC_HOST_1_TEST	0
#else
#define SOCLE_SDHC_HOST_1_TEST	1
#endif

#ifndef SOCLE_INTC_SDHC0
#if defined(CONFIG_CDK)
#define SOCLE_INTC_SDHC0	SOCLE_INTC_MPS1
#else
#error "SDHC Host IRQ is not defined"
#endif
#endif

#ifndef SOCLE_INTC_SDHC1
#define SOCLE_INTC_SDHC1	SOCLE_INTC_SDHC0
#endif

#endif // CONFIG_SDHC


#ifdef CONFIG_A2ADMA
#define SOCLE_SDMMC_SWDMA_A2A_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_A2A_TEST 0
#endif

#ifdef CONFIG_HDMA
#define SOCLE_SDMMC_SWDMA_HDMA_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_HDMA_TEST 0
#endif

#if defined(CONFIG_PANTHER7_HDMA) && (defined(CONFIG_PDK) || defined(CONFIG_PC7210) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK))
#define SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST 0
#endif

#if defined(CONFIG_CDK) || defined(CONFIG_PC9220) || defined(CONFIG_PC9223) || defined(CONFIG_PC9002) || defined(CONFIG_SCDK) || defined(CONFIG_MDK3D) || defined(CONFIG_MDKFHD)
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0
#define SDHC_NORMAL_TEST	1
#else
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0 
#define SDHC_NORMAL_TEST	1
#endif
