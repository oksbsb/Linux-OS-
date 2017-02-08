/*
 * irq.c - support functions for handling interrupt
 * Author:	    gyms guo
 * Version: 1
 *
 */

 #include "types.h"
 #include "lib.h"
 #include "i8259.h"
 #include "irq.h"

/* The jump table for irq handler*/

static do_irq irq_handler[16]=
{
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};


/*   
 *	interrupt_handler
 *	 DESCRIPTION: The common intterupt handling fuction
 *   INPUTS: reg the register on the stack
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void interrupt_handler(stack_register reg){

	uint32_t irq_num=-1*reg.interrupt_num-1; /*Convert the isr number back to the irq number*/

	void (*handler) (stack_register reg);   /*Declear a function pointer*/
	handler=irq_handler[irq_num];      /*Find the irq function*/
	
	disable_irq(irq_num);               /*PIC is fragile disable the irq and then send eoi*/              
	send_eoi(irq_num); 
	if(handler){
		handler(reg);
	}
	
	enable_irq(irq_num);				/*Renable the irq*/

}
/*   
 *	 set_interrupt
 *	 DESCRIPTION: Install the handler and enable the irq
 *   INPUTS: irq_num: the number of irq to be installed and handler: the handler funciton
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void set_interrupt(uint32_t irq_num,do_irq handler){
	cli();
	irq_handler[irq_num]=handler;		/*Install the handler and enable the irq*/
	enable_irq(irq_num);
	sti();
}

/*   
 *	 unset_interrupt
 *	 DESCRIPTION: Uninstall the handler and disable the irq
 *   INPUTS: irq_num: the number of irq to be installed
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

void unset_interrupt(uint32_t irq_num){
	cli();
	irq_handler[irq_num]=0;			    /*Uninstall the handler and disable the irq*/
	disable_irq(irq_num);
	sti();
}




