#ifndef __DEPENDENCY_H_INCLUDED
#define __DEPENDENCY_H_INCLUDED

#include <platform.h>
#include <irqs.h>
#include <dma/dma.h>

#ifdef SOCLE_I2S0
#define SOCLE_I2S_BASE SOCLE_I2S0
#else
#error "I2S IP base address is not defined"
#endif

#ifdef SOCLE_INTC_I2S0 
#if 1
#define SOCLE_I2S_IRQ SOCLE_INTC_I2S0
#else
#define SOCLE_I2S_IRQ SOCLE_INTC_MPS1
#endif
#else
#error "I2S IRQ is not defined"
#endif

//20080123 Ryan Define MSB/LSB to 0
#define SOCLE_I2S_BUS_LSB_TEST 0
#define SOCLE_I2S_BUS_MSB_TEST 0

#ifdef CONFIG_PANTHER7_HDMA
#define SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST 1
#else
#define SOCLE_I2S_SUPPORT_HWDMA_PANTHER7_HDMA_TEST 0
#endif

#ifdef CONFIG_DMAC
#define SOCLE_I2S_SUPPORT_DMAC_TEST_IMPLEMENT 1
#else
#define SOCLE_I2S_SUPPORT_DMAC_TEST_IMPLEMENT 0
#endif

#define TX_DMA_EXT_HDREQ 6
#define RX_DMA_EXT_HDREQ 7

#define SOCLE_I2S_HDMA_AUTO_TEST	0
#define SOCLE_I2S_HWDMA_BURST_INCR8	0
#define SOCLE_I2S_HWDMA_BURST_INCR16	0


#endif
