/*
 * irq.h - header file for irq.h support fucntions for interrupt handling
 * Author:	    gyms guo
 * Version: 1
 *
 */

#ifndef IRQ_H
#define IRQ_H

#include "types.h"

/*handlering the interrupt*/
extern void interrupt_handler(stack_register reg);
/*install the irq handler*/
extern void set_interrupt(uint32_t irq_num,do_irq handler);
/*unistall the irq hadnler*/
extern void unset_interrupt(uint32_t irq_num);

#endif /* IRQ_H */


