#include <platform.h>
#include <irqs.h>

#ifndef SQ_OTG0
#error "OTG IP base address is not defined"
#endif

#ifndef SQ_OTG1
#define SQ_OTG1	SQ_OTG0
#endif

#ifndef SOCLE_OTG2
#define SOCLE_OTG2	SQ_OTG0
#endif

#ifndef SQ_INTC_OTG0
#error "OTG IRQ is not defined"
#endif

#ifndef SQ_INTC_OTG1
#define SQ_INTC_OTG1 SQ_INTC_OTG0
#endif

#ifndef SOCLE_INTC_OTG2
#define SOCLE_INTC_OTG2 SQ_INTC_OTG0
#endif

#if defined(CONFIG_PC9223)
#define OTG_HOST_TEST			0
#define OTG_DEVICE_TEST		1
#define OTG_DULE_ROLE_TEST		0
#define OTG0_DEV_TEST	0
#define OTG1_DEV_TEST	1
#define OTG2_DEV_TEST	0
#else
#define OTG_HOST_TEST			1
#define OTG_DEVICE_TEST		1
#define OTG_DULE_ROLE_TEST		1
#define OTG0_DEV_TEST	1
#define OTG1_DEV_TEST	0
#define OTG2_DEV_TEST	0
#endif

//#define __VUSBHS__
