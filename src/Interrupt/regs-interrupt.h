#ifndef _regs_interrupt_H_
#define _regs_interrupt_H_

#define	INTC0_SCR(x) ((addr_t)(SQ_INTC0 + 4 * (x)))
#define	INTC0_SVR(x) ((addr_t)(SQ_INTC0 + 0x80	+ 4 * (x)))
#define	INTC0_ISR    ((addr_t)(SQ_INTC0 + 0x104))
#define	INTC0_IPR    ((addr_t)(SQ_INTC0 + 0x108))
#define	INTC0_IMR    ((addr_t)(SQ_INTC0 + 0x10C))
#define	INTC0_IECR   ((addr_t)(SQ_INTC0 + 0x114))
#define	INTC0_ICCR   ((addr_t)(SQ_INTC0 + 0x118))
#define	INTC0_ISCR   ((addr_t)(SQ_INTC0 + 0x11C))
#define	INTC0_TEST   ((addr_t)(SQ_INTC0 + 0x124))

// define Macro	Lib
#define	INT0_SET_TYPE(i,type)		writew((readw(INTC0_SCR(i)) & ~SRCTYPE_MASK) | type , INTC0_SCR(i))
#define	INT0_GET_TYPE(i)		     		readw(INTC0_SCR(i)) & SRCTYPE_MASK
#define	INT0_SET_PRIORITY(i,p)	 	writew((readw(INTC0_SCR(i)) & 0xfffffff8) | (p), INTC0_SCR(i))
#define	INT0_GET_PRIORITY(i)	     		readw(INTC0_SCR(i)) & BIT_MASK(3)

#define	INT0_ENABLE(i)		     		writew(readw(INTC0_IECR) |  (0x1 << i), INTC0_IECR)
#define	INT0_DISABLE(i)		     		writew(readw(INTC0_IECR) & ~(0x1 << i), INTC0_IECR)
#define	INT0_SET_MASK(i)		     	writew(readw(INTC0_IMR)  |  (0x1 << i), INTC0_IMR)
#define	INT0_CLR_MASK(i)		     		writew(readw(INTC0_IMR)  & ~(0x1 << i), INTC0_IMR)

#define	INT0_TRIGGER(i)		     		writew(readw(INTC0_ISCR)| (0x1 << i), INTC0_ISCR)
#define INT0_SWTRIGGER_CLEAR()    	writew(0X0, INTC0_ISCR)
#define	INT0_CLEAR(i)		     		writew(1 << i, INTC0_ICCR)
#define	SET_INTR0_TESTMODE()		writew(1, INTC0_TEST)
#define	CLEAR_INTR0_TESTMODE()		writew(0, INTC0_TEST)

#define	LO_LEVEL		 		0x00000000
#define	HI_LEVEL		 		0x00000040
#define	NEGATIVE_EDGE	 	0x00000080
#define	POSITIVE_EDGE	 	0x000000C0
#define	SRCTYPE_MASK    0x000000C0


#endif  //_regs_interrupt_H_

