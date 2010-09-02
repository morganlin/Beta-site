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

#ifndef SQ_SDHC0
#error "SDHC Host IP base address is not defined"
#endif

#ifndef SQ_SDHC0
#define SQ_SDHC0	SOCLE_SDHC0
#define SOCLE_SDHC_HOST_0_TEST	0
#else
#define SOCLE_SDHC_HOST_0_TEST	1
#endif

#ifndef SOCLE_SDHC1
#define SOCLE_SDHC1	SQ_SDHC0
#define SOCLE_SDHC_HOST_1_TEST	0
#else
#define SOCLE_SDHC_HOST_1_TEST	1
#endif

#ifndef SQ_INTC_SDHC0
#error "SDHC Host IRQ is not defined"
#endif

#ifndef SOCLE_INTC_SDHC1
#define SOCLE_INTC_SDHC1	SQ_INTC_SDHC0
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

#define SOCLE_SDMMC_SWDMA_PANTHER7_HDMA_TEST 0

#if defined(CONFIG_PC9223)
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0
#define SDHC_NORMAL_TEST	1
#else
#define SOCLE_SDMMC_MUTLIPLE_PARTIAL_READ_TEST 0 
#define SDHC_NORMAL_TEST	1
#endif
