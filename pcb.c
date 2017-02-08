/*
 * pcb.c - PCB function
 * Author:	    gyms guo
 * Version: 1
 *
 */

#include "pcb.h"
#include "kb.h"
#include "lib.h"
#include "syscall.h"


/*
* 	int32_t read_default(int32_t fd,void *buf,int32_t nbytes)
*   Inputs: int32_t fd = file descriptor index 
*			void *buf = buffer passed in, used to stored data copied
* 			int32_t nbytes = number of bytes to copy
*   Return Value: returns error
*	Function: Dose nothing and return error
*/
int32_t read_default(int32_t fd,void *buf,int32_t nbytes){
	return -1;
}
/*
* 	int32_t write_default(const void* buf, int32_t nbytes)
*   Inputs: 
*			const void* buf = buffer passed in, stores data to write
* 			int32_t nbytes = number of bytes to write
*   Return Value: returns error
*	Function: Dose nothing and return error
*/

int32_t write_default(const void* buf, int32_t nbytes){
	return -1;
}


/*
* 	int32_t open_default()
*   Inputs: NONE
*   Return Value: returns error
*	Function: Dose nothing and return error
*/

int32_t open_default(){
	return -1;
}

/*
* 	int32_t close_default()
*   Inputs: NONE
*   Return Value: returns error
*	Function: Dose nothing and return error
*/

int32_t close_default(){
	return -1;
}

/*
* 	void init_pcb()
*   Inputs: NONE
*   Return Value:  NONE
*	Function: Initialze the pcb pointer array
*/

void init_pcb(){
	int i=0;/*For counting purpose*/
	for(i = 0;i < MAX_PCB_NUM; i++){
		pcb_ptr[i]=NULL;
	}
}


/*
* 	int32_t find_new_pcb()
*   Inputs: NONE
*   Return Value:  -1 no pcb availble
*	Function: Find the valid number for pcb
*/
int32_t find_new_pcb(){
	int i=0; /*For counting purpose*/
	for(i = 0;i < MAX_PCB_NUM; i++){
		if(pcb_ptr[i] == NULL)
			break;
	}
	if(i == MAX_PCB_NUM)
		return -1;
	return i+1;
}


/*
* 	pcb_t* find_pcb(int32_t pcb_num)
*   Inputs: pcb_num
*   Return Value: return NULL if no pcb pointer found
*	Function: Find the valid PCB address
*/

pcb_t* find_pcb(int32_t pcb_num){
	if(pcb_num>MAX_PCB_NUM)
		return NULL; /*no pcb found*/
	if(pcb_num==0)
		return NULL; /*no pcb found*/
	if(pcb_ptr[pcb_num-1]==NULL)
		return NULL; /*no pcb found*/
	return pcb_ptr[pcb_num-1];
}

/*
* 	int32_t new_pcb(pcb_t *cur_pcb_ptr,int32_t parent_pcb_num,int32_t pcb_num)
*   Inputs: pcb_t *cur_pcb_ptr: the current pcb pointer
*	int32_t parent_pcb_num: the parent pcb number
*	int32_t pcb_num: the current pcb number
*   Return Value: always return 0
*	Function: Initialize the pcb struct
*/
int32_t new_pcb(pcb_t *cur_pcb_ptr,int32_t parent_pcb_num,int32_t pcb_num){
	/******Assign the parent pcb pointer******/
	if(parent_pcb_num==0)
		cur_pcb_ptr->parent=0;
	else
		cur_pcb_ptr->parent=find_pcb(parent_pcb_num);
		
	
	/*******File*********/
	int i; /*For counting purpose*/
	for(i=0;i<PCB_FILE_NUMBER;i++){
		cur_pcb_ptr->file[i].flag=NOT_IN_USE;
	}
	

	/*file standord in*/
	cur_pcb_ptr->file[STDIN].inode_ptr=0;
	cur_pcb_ptr->file[STDIN].file_pos=0;
	cur_pcb_ptr->file[STDIN].flag=IN_USE;
	cur_pcb_ptr->file[STDIN].fop_jump=&fops[pcb_num-1][STDIN];
	cur_pcb_ptr->file[STDIN].fop_jump->read_func=read_terminal;
	cur_pcb_ptr->file[STDIN].fop_jump->write_func=write_default;
	cur_pcb_ptr->file[STDIN].fop_jump->open_func=open_terminal;
	cur_pcb_ptr->file[STDIN].fop_jump->close_func=close_default;

	/*file standard out*/
	cur_pcb_ptr->file[STDOUT].fop_jump=&fops[pcb_num-1][STDOUT];
	cur_pcb_ptr->file[STDOUT].fop_jump->read_func=read_default;
	cur_pcb_ptr->file[STDOUT].fop_jump->write_func=write_terminal;
	cur_pcb_ptr->file[STDOUT].fop_jump->open_func=open_terminal;
	cur_pcb_ptr->file[STDOUT].fop_jump->close_func=close_default;
	cur_pcb_ptr->file[STDOUT].inode_ptr=0;
	cur_pcb_ptr->file[STDOUT].file_pos=0;
	cur_pcb_ptr->file[STDOUT].flag=IN_USE;

	/*Register value*/
	cur_pcb_ptr->ebp=0;
	cur_pcb_ptr->esp=0;

	cur_pcb_ptr->sched_esp=0;
	cur_pcb_ptr->sched_ebp=0;

	/*Pid number*/
	cur_pcb_ptr->pid=pcb_num;
	/*retval*/
	cur_pcb_ptr->retval=0;
	/*args*/
	for(i=0;i<EXECUTE_ARG;i++){
		cur_pcb_ptr->args[i]='\0';
	}
	/*terminal number*/
	cur_pcb_ptr->terminal_number = get_terminal_number();
	return 0;

}












