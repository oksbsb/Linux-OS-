/* 
 * scheduling.h - scheduling realted functions headers
 * Author:	    shui 
 * Version: 1
 *
 */

#ifndef SCHEDULING_H
#define SCHEDULING_H 

#include "pcb.h"
#include "lib.h"


/*Pit initialize*/
extern void initialize_pit(void);	/// set up the interrupt frequency so that each processed can be scheduled
									// when swtiching terminals, we see them running 
									// even for shell, it is scheduled 
/*The main scheduling functions*/
extern uint32_t schedule_next_process(int terminal_number);	//0 1 2 indicate terminals, and it
									// is used to schedule next process based on given terminals 

/*Set the current process manually*/
extern void set_next_process(int i);	/// set current terminal that is going to be scheduled 
/*Get the current process number*/
extern uint32_t get_current_process();	// get current terminal 

#endif


