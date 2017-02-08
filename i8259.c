/* i8259.c - Functions to interact with the 8259 interrupt controller
 * author: meng 
 */

#include "i8259.h"
#include "lib.h"

/*******ATTENTION***********/
/* This code adapts linux codes*/

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask=MASK_ALL; /* IRQs 0-7 */
uint8_t slave_mask=MASK_ALL; /* IRQs 8-15 */

/*  Initialize the 8259 PIC 
 *  i8269_init 
 *  DESCRIPTION: initialize PIC by writing ICW1-ICW4
 *  INPUT: NONE
 *  OUTPUT: NONE
 */
void
i8259_init(void)
{

    cli();                                    /*Start of the critical section*/
	  outb(MASK_ALL, MASTER_8259_IMR);   			  /* mask all of 8259A-1*/ 
    outb(MASK_ALL, SLAVE_8259_IMR);    			  /* mask all of 8259A-2 */

    outb(ICW1 , MASTER_8259_CMD);				     /*ICW1 : select 8259A-1 init*/ 

   	/* 
   	 * Since IC4 = 1, ICW4 is required
   	 * Since SNGL = 0, ICW3 is required
     */

    outb(ICW2_MASTER , MASTER_8259_IMR);		  /*ICW2 : map 0x20 - 0x27 to 8259A-1 IR0 - IR7*/
    outb(ICW3_MASTER , MASTER_8259_IMR);      /*ICW3 : indicate that master PIC has a slave on IR2*/
    outb(ICW4 , MASTER_8259_IMR);				      /*ICW4 : indicate that master expects normal EOI*/

    outb(ICW1 , SLAVE_8259_CMD);				      /*ICW1 : select 8259A-2 init*/

   	/*
   	 * Since IC4 = 1, ICW4 is required
   	 * Since SNGL = 0, ICW3 is required
     */

    outb(ICW2_SLAVE , SLAVE_8259_IMR);			  /*ICW2 : map 0x28 - 0x2f to 8259A -2 IR0 - IR7*/
    outb(ICW3_SLAVE , SLAVE_8259_IMR);			  /*ICW3 : since slave is connected with IR2 on master, the value is 2*/
    outb(ICW4 , SLAVE_8259_IMR);				      /*ICW4 : indicate that slave expects normal EOI*/


    outb(master_mask , MASTER_8259_IMR);      /*Restore the mask flag*/
    outb(slave_mask , SLAVE_8259_IMR);

    sti();                                     /*End of the ciritical section*/

}

/* 
 * Enable (unmask) the specified IRQ 
 * enable_irq
 * DESCRIPTION: Enable an interrupt by setting bit mask
 * INPUT: irq_num --- index of interrupt that should be enabled
 * OUTPUT: NONE
 */
void
enable_irq(uint32_t irq_num)
{

	uint8_t mask;
 
  cli();                     
	if(irq_num < MASTER_NUM){            /*For the master irq only*/       
		mask=~(1<<irq_num);
		master_mask&=mask;                 
		outb(master_mask,MASTER_8259_IMR);
	}
	else
	{                                   /*For the slave irq only*/
		irq_num -= MASTER_NUM;
		mask=~(1<<irq_num);
		slave_mask&=mask;
		outb(slave_mask,SLAVE_8259_IMR);
	}
  // sti();
}

/*  Disable (mask) the specified IRQ 
 *  disable_irq
 *  DESCRIPTION: Disable an interrupt by setting bit mask
 *  INPUT: irq_num --- index of interrupt that should be disabled
 *  OUTPUT: NONE
*/
void
disable_irq(uint32_t irq_num)
{

	uint8_t mask;
  cli();

	if(irq_num < MASTER_NUM){          /*For the master irq only*/ 
		mask=(1<<irq_num);
		master_mask|=mask;
		outb(master_mask,MASTER_8259_IMR);
	}
	else
  {
		irq_num -= MASTER_NUM;          /*For the master irq only*/         
		mask=(1<<irq_num);
		slave_mask|=mask;
		outb(slave_mask,SLAVE_8259_IMR);	
	}

  sti();
}

/*  Send end-of-interrupt signal for the specified IRQ 
 *  send_eoi
 * 	DESCRIPTION: send end-of-interrupt signal 
 *  INPUT: irq_num --- which interrupt has been done
 *  OUTPUT: NONE
 */
void
send_eoi(uint32_t irq_num)
{
  cli();
	if(irq_num < MASTER_NUM)
   	{
      outb((irq_num + EOI) , MASTER_8259_CMD);    /*send eoi*/
    }
    else
    {
      irq_num -= MASTER_NUM;
      outb((irq_num + EOI) , SLAVE_8259_CMD);      /*send eoi*/
      outb((SLAVE_PIC_IRQ+EOI),MASTER_8259_CMD);   /*send eoi to the master irq_2*/
    }
  sti();
}




