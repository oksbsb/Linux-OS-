/*
 * isr.c - support functions for handling exception
 * Author:	    gyms
 * Version: 1
 *
 */

 #include "types.h"
 #include "lib.h"
 #include "kb.h"

 /*   
  *	  exception_handler
  *	  DESCRIPTION: handlering the exception
  *   INPUTS: irq_num: the number of irq to be installed
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */

void exception_handler(stack_register reg){
	/*clean_screen();*/
	printf("   No.%d exception detected",reg.interrupt_num );
  	while(1);

}























