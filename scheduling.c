/* 
 * scheduling.h - scheduling realted functions 
 * Author:	    shui 
 * Version: 1
 *
 */
#include "scheduling.h"
#include "syscall.h"
#include "pcb.h"
#include "paging.h"
#include "x86_desc.h"
#include "kb.h"
#include "i8259.h"
#include "irq.h"

static uint32_t current_terminal=0;	/*indicate which terminal is being scheduled*/

/*All the local funciton, refer to the header for more information*/
static void pit_handler();

/*  Initialize the 8253 PIT 
 *  initialize_pit 
 *  DESCRIPTION: initialize PIT
 *  INPUT: NONE
 *  OUTPUT: NONE
 */
void initialize_pit(void) /*100Hz is once every 10 milliseconds*/
{
	cli();
	/*divisor must be small enough to fit into 16-bits*/
	uint32_t divisor = PIC_INPUT_HZ/PIT_FREQ; /*100HZ*/

 	
 	outb(CW, CW_PORT);
 	outb((uint8_t)(divisor & LSB), COUNTER_PIC);
 	outb((uint8_t)((divisor & MSB) >> 8), COUNTER_PIC);
 	sti();

 	set_interrupt(PIT_IRQ_NUM ,pit_handler); 	/*Install the pit handler*/
}

/*  Initialize the 8253 PIT 
 *  initialize_pit 
 *  DESCRIPTION: initialize PIT
 *  INPUT: frequency to set
 *  OUTPUT: NONE
 */
static void pit_handler(stack_register reg)
{
		/*critical section begin*/
		cli();
		schedule_next_process(-1);	
		sti();	
		/*critical section ends*/
}



/*
 * schedule_next_process: switch to next process in order
 * Input: termis_num --- terminal that is active
 * Output: none
 * Return value: 0 if success , -1 if failed
 * Sides effect: none
 */
uint32_t schedule_next_process(int termis_num)
{	
	/*enable PIT's interrupt*/
	enable_irq(0);							//// test 
	uint32_t curr_esp,curr_ebp;
	int i; /*For counting*/
	/*switch to the next process*/
	uint32_t next_pcb_num ,next_esp, next_ebp;

	asm volatile("movl %%esp, %0"
				 :"=r"(curr_esp)
				);
	asm volatile("movl %%ebp, %0"
				 :"=r"(curr_ebp)		// save the ebp, esp of current process, used for returning back 
				);
	/*find the index of current pcb*/
	int32_t curr_pcb_num = find_terminal_by_pcb(current_terminal);
	/*invalid case*/
	if(curr_pcb_num==0)	// since pcb num is from 1 to 6, 
		return -1;
	/*fint the current pcb*/
	pcb_t* curr_pcb = find_pcb(curr_pcb_num);
	/*invalid pcb*/
	if(curr_pcb==0)		// if process is null 
		return -1;
	
										/// each process has its own stack, 
	curr_pcb -> sched_esp = curr_esp;	/// sched_esp, sched_ebp is used for scheduling between differnt terminals, from terminal 3 to terminal 1
	curr_pcb -> sched_ebp = curr_ebp;	/// esp, ebp is used for halt and tell the child process your parent's esp/ebp
										/// used for halt, halt child process and return to parent ebp/esp
	//set_kesp_kebp(curr_esp, curr_ebp);
	/*open a new terminal if there's no terminal active before*/
	if(termis_num>=0){					//  schedule the three terminals by opening terminals
		termis[termis_num].run_shell = 1;	// first scheduling open terminal 1, then 2, then 3 
		set_next_process(termis_num);			// question here ? 
		enable_irq(0);		
		enable_irq(KB_IRQ_NUM);/*Renable the keyboard interrupt*/ // make alt + F2 working 
		execute((const uint8_t *)"shell\n");
	}

	for(i=0;i<3;i++){
		current_terminal=(current_terminal+1)%3;
		next_pcb_num = find_terminal_by_pcb(current_terminal);	// find next process to schedule from terminal 
		if(next_pcb_num!=0)			// check validity 
			break;
	}
	pcb_t* next_pcb = find_pcb(next_pcb_num);
	/*invalid case*/
	if(next_pcb_num==0 || next_pcb==0)
		return -1;
	
	/*switch paging*/
	int32_t next_pde = next_pcb_num - 1;
	
	if(next_pde>=TASK_NUMBER)	/// check if next_pde is larger than pd, 2D array of paing 
		return -1;				/// pd[TASK_NUMBER][1024]
	
	next_esp=next_pcb -> sched_esp;
	next_ebp=next_pcb -> sched_ebp;	// going to schedule next process, so get its ebp/esp and replace 
									// with current ebp/esp 

	if(next_esp==0 || next_ebp==0)
		return -1;
	/*write esp0 with next esp*/	// system call is a call to kernel and requests OS service. 
								// performed by executing an interrupt, and this causes the kernel to take over 
		// and performed request service, then give control back to applications. mode-switch, system call slower. 
	tss.esp0 = KERNEL_BOTTOM-STACK_SIZE*(next_pcb_num - 1);	// for context switching, 
	tss.ss0 = KERNEL_DS;		// each process has a private kernel stack 
	set_new_task(next_pde,0);				// question here 
	/*store next esp and ebp into eax and ecx*/
	asm("movl %0, %%eax"
		:
		:"m"(next_esp)
		);
	asm("movl %0, %%ecx"
		:
		:"m"(next_ebp)
		);
	/*move the value in eax and ecx into esp and ebp*/
	asm("movl %%eax, %%esp"
		:);
	asm("movl %%ecx, %%ebp"
		:);
	asm ("leave");
	asm ("ret");
	return 0;
}

/*
 * set_curprocess: set the current_terminal to some value
 * Input: i --- next value of current_terminal
 * Output: none
 * Return value: none
 * Sides effect: none
 */
void set_next_process(int i){	
	current_terminal = i;
}

/*
 * get_current_process: get the current_terminal
 * Input: none
 * Output: current process index
 * Return value: current_terminal
 * Sides effect: none
 */
uint32_t get_current_process(){
	return current_terminal;
}

