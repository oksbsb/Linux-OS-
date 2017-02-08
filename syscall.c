/*
 * syscall.c System call functions
 * Author:	    gyms guo  
 * Version: 1
 *
 */
#include "lib.h"
#include "syscall.h"
#include "file_system.h"
#include "paging.h"
#include "x86_desc.h"
#include "pcb.h"
#include "rtc.h"
#include "kb.h"
#include "scheduling.h"
#include "int_syscall_wrapper.h"

/*All the possible exeutable rigt now*/
 static char parse_array[EXECUTE_FILES][EXECUTE_NAME]=
 { "cat","counter","fish","grep",
 	"hello","ls","pingpong","shell",
 	"sigtest","syserr","testprint"
 };
static int32_t pcb_num[TOTAL_TERMINALS] = {0 , 0 , 0}; /*The pcb num*/


/*
 * 	int32_t halt (uint8_t status)
 *  Inputs: uint8_t status indicate the retval of the execute system call
 *  Return Value: indicate the status of the execute sysctem call
 *	Function: return the status
 */
int32_t halt (uint8_t status){
	cli();/*critical section starts*/
	/*get the index of process that is running*/
	int ind = get_current_process();
	/*get current pcb*/
	pcb_t* curr_pcb = find_pcb(pcb_num[ind]);
	/*close the unused file*/
	int fd_counter;
	for(fd_counter = FD_START ; fd_counter < PCB_FILE_NUMBER ; fd_counter ++)
	{
		/*flag = 0 indicates that the file is unused*/
		if(curr_pcb -> file[fd_counter].flag == 0)
		{
			close(fd_counter);
		} 
	}
	close_terminal();
	/*find parent pcb*/
	pcb_t* parent_pcb = curr_pcb -> parent;
	pcb_ptr[pcb_num[ind]-1]=0;
	/*map the paging to its initial address*/
	restore_page(pcb_num[ind]-1);
	/*if the previous process has no parent, execute the shell*/
	if(parent_pcb==0){
		pcb_num[ind]=0;
		execute((const uint8_t *)"shell\n");
	}
	/*update the pcb_num*/
	pcb_num[ind]=parent_pcb->pid;
	/*write esp0 with next esp*/
	tss.esp0 = KERNEL_BOTTOM-STACK_SIZE*(pcb_num[ind]-1);
	tss.ss0 = KERNEL_DS;
	/*add new page for the new process*/
	set_new_task(pcb_num[ind] - 1,0);

	/*get relevant regs value*/
	int parent_esp = parent_pcb -> esp;
	int parent_ebp = parent_pcb -> ebp;


	/*restore regs value*/
	asm("movl %0, %%eax"
		:
		:"r"((uint32_t)status)
		);
	asm("movl %0, %%esp"
		:
		:"m"(parent_esp)
		);
	asm("movl %0, %%ebp"
		:
		:"m"(parent_ebp)
		);
	/*restore parent's paging*/
	asm ("jmp halt_ret");
	/*never return*/
	return -1;
}
/*
* 	int32_t execute(const uint8_t * command)
*   Inputs: the command for the execute
*   Return Value: indicate the status of the execute sysctem call
*	Function: execute the command
*/
int32_t execute(const uint8_t * command){
	cli();
	int ind=get_current_process();
	int cur_pde=0;
	int i=0;/*For counting purpose*/
	/*test_command buffer*/
	uint8_t test_command[MAX_COMMAND_INPTUT];
	/*Store the temporary pcb number*/ 
	int pcb_num_temp; 
	/*Store the all the registers*/
	uint32_t curr_esp,curr_ebp; 
	/*get the entire command*/
	while((command[i]!='\n')&&(command[i]!='\0')&&(command[i]!=' ')){
		test_command[i]=command[i];
		i++;
	}
	test_command[i]='\0';
	uint8_t buf[ELF_BYTE_NUM]; /*The buf for checking the elf*/
	int32_t parent_pcb_num; /*parennt pcb_num*/

	/*****get argument**********/
	char buffer[EXECUTE_ARG];
	int arg_start = i + 1;
	int j = 0,k=0;/*For counting purpose*/
	for(k=0;k<EXECUTE_ARG;k++){
		buffer[k]='\0';
	}
	/*copy full argument into the buffer*/
	while(command[arg_start] != '\0')
	{
		buffer[j] = command[arg_start];
		arg_start ++;
		j ++;
	}
	buffer[j] = '\0';
	/******Parsing***********/
	for(i=0;i<EXECUTE_FILES;i++){
		if(strncmp((const int8_t* )test_command, (const int8_t*)(parse_array[i]),MAX_COMMAND_INPTUT)==0)
			/*if the command is right, continue*/
			break;
	}
	/*if the command cannot be found, return with error return value*/
	if(i>=EXECUTE_FILES){
		return -1;
	}

	/****exe check****/
	dentry_t dentry;
	read_dentry_by_name(test_command, &dentry);
	int32_t f_inode_num = dentry.inode_number;
	read_data (f_inode_num, 0, buf,ELF_CHECK_NUM);
	uint32_t *buf_new=(uint32_t*)buf;

	if(buf_new[0]!=ELF_MAGIC)
		return -1;

	/****Set up new page****/
	parent_pcb_num=pcb_num[ind];
	pcb_num_temp=find_new_pcb();
	if(pcb_num_temp==-1) 
		/*No valid pcb number*/
	 	return -1;
	pcb_num[ind]=pcb_num_temp;
	cur_pde=pcb_num[ind]-1;
	if(cur_pde>=TASK_NUMBER)
		/*No valid pde number*/
		return -1;
	/*add new page for the program*/
	set_new_task(cur_pde,0);
	/***Load the file*****/
	loader((uint32_t*)FILE_START, test_command);

	/******New PCB*******/
	pcb_t *cur_pcb_ptr=((pcb_t*)((KERNEL_BOTTOM-STACK_SIZE*(pcb_num[ind]-1))&PCB_MASK));
	pcb_ptr[pcb_num[ind]-1]=cur_pcb_ptr;
	new_pcb(cur_pcb_ptr,parent_pcb_num,pcb_num[ind]);
	/***Context switch***/
	pcb_t* new_pcb = find_pcb(pcb_num[ind]);
	/********store arguments*******/
	strcpy(new_pcb -> args , buffer);

	/*write esp0 with next esp*/
	tss.esp0 = KERNEL_BOTTOM-STACK_SIZE*(pcb_num[ind]-1);
	tss.ss0 = KERNEL_DS;

	if(new_pcb->parent!=0){
		/*get address of current pcb*/
		pcb_t* curr_pcb = new_pcb -> parent;
		/*get current value of regs*/
		asm volatile("movl %%esp, %0"
					 :"=r"(curr_esp)
					);
		asm volatile("movl %%ebp, %0"
					 :"=r"(curr_ebp)
					);

		/*store value of current regs into current pcb*/
		curr_pcb -> esp = curr_esp;
		curr_pcb -> ebp = curr_ebp;
	}

	 /***Push artificial IRET context onto the stack***/
	uint32_t entry_point=*((uint32_t*) (FILE_START)+FILE_ENTRY_OFFSET);

	/*assembly linkage*/
	context_switch(entry_point);

	/*never return*/
	return 0;
}

/*
* 	int32_t read(int32_t fd,void *buf,int32_t nbytes);
*   Inputs: int32_t fd = file descriptor index 
*			void *buf = buffer passed in, used to stored data copied
* 			int32_t nbytes = number of bytes to copy
*   Return Value: returns the return value of the specific read function
*	Function: jump to the correct read function and read the file corresponding to the fd passed in
*/
int32_t read(int32_t fd,void *buf,int32_t nbytes){
	int ind=get_current_process();
	/*check weather index is valid*/
	if (fd < 0 || fd >= PCB_FILE_NUMBER)	return -1;
	/*get the pcb*/
	pcb_t * pcb = find_pcb(pcb_num[ind]);
	/*if invalid, return null*/
	if (pcb == NULL) return -1;
	/*if the file is unused, return error return value*/
	if (pcb->file[fd].flag == 0)	return -1;      

	return pcb->file[fd].fop_jump->read_func((int32_t)(&pcb->file[fd]), buf, nbytes);       
}

/*
* 	int32_t write(int32_t fd, const void* buf, int32_t nbytes);
*   Inputs: int32_t fd = file descriptor index 
*			const void* buf = buffer passed in, stores data to write
* 			int32_t nbytes = number of bytes to write
*   Return Value: returns the return value of the specific write function
*	Function: jump to the correct write function and write the number of bytes to the file if allowed.
*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
	int ind=get_current_process();
	/*validity check*/
	if (fd < 0 || fd >= PCB_FILE_NUMBER)	return -1;
	pcb_t * pcb = find_pcb(pcb_num[ind]);
	/*invalid pcb*/
	if (pcb == NULL) return -1;
	/*if file unused*/
	if (pcb->file[fd].flag == 0)	return -1;

	return pcb->file[fd].fop_jump->write_func(buf, nbytes); 

}

/*
* 	int32_t open(const uint8_t* filename);
*   Inputs: const uint8_t* filename= name of file to open
*   Return Value: returns the index fd on success, -1 on failure 
*	Function: find an available file descriptor and store necessary info into the fd structure. 
*             Then jump to the correct open function.
*/
int32_t open(const uint8_t* filename){
	/*get the index of process that is running*/
	int ind=get_current_process();
	pcb_t * pcb = find_pcb(pcb_num[ind]);
	/*invalid pcb*/
	if (pcb == NULL) return -1;

	int fd;
	/*find vacant fd*/
	int i;
	for(i = FD_START; i < PCB_FILE_NUMBER; i++)  /*why start from 2: 0,1 are occupied by stdin and stdout*/
	{
		if (pcb->file[i].flag == NOT_IN_USE)
		{
			
			fd = i;
			dentry_t dentry;
			/*if filename doesn't exist*/
			if(read_dentry_by_name(filename, &dentry) == -1) return -1; 
			/*map the read write open close functions*/
			switch(dentry.file_type){
   				case RTC_FILE: /*RTC*/
 					pcb->file[i].fop_jump = &fops[pcb_num[ind]-1][i];
 					pcb->file[i].fop_jump->read_func=read_rtc;
					pcb->file[i].fop_jump->write_func=write_rtc;
					pcb->file[i].fop_jump->open_func=open_rtc;
					pcb->file[i].fop_jump->close_func=close_rtc;
      				break; 
   				case DIR_FILE: /*directory*/
     			 	pcb->file[i].fop_jump = &fops[pcb_num[ind]-1][i];
 					pcb->file[i].fop_jump->read_func=directory_read;
					pcb->file[i].fop_jump->write_func=directory_write;
					pcb->file[i].fop_jump->open_func=directory_open;
					pcb->file[i].fop_jump->close_func=directory_close;
      				break;
      			case REGULAR_FILE: /*regular file*/
  					pcb->file[i].fop_jump = &fops[pcb_num[ind]-1][i];
 					pcb->file[i].fop_jump->read_func=read_file;
					pcb->file[i].fop_jump->write_func=write_file;
					pcb->file[i].fop_jump->open_func=open_file;
					pcb->file[i].fop_jump->close_func=close_file;
      				break;
   				default : break;
			}

			/*store inode pointer if file type is 2*/
			if(dentry.file_type == REGULAR_FILE) /*regular file 2*/
			{
				pcb->file[i].inode_ptr = bb_start + (dentry.inode_number + 1) * BLOCK_4KB;
			}
			else
			{
				pcb->file[i].inode_ptr = NULL;
			}

			pcb->file[i].file_pos = 0;
			pcb->file[i].flag = IN_USE;

			pcb->file[i].fop_jump->open_func(); /*Call the open functions*/
			return fd;
		}
	}

	/*if no descriptor is free*/
	return -1;
}

/*
* 	int32_t close(int32_t fd);
*   Inputs: int32_t fd = the index of the file descriptor that you want to close
*   Return Value: returns the return value of the corresponding close function
*	Function: set the flag to 0 (unused) and jump to the correct close function. 
*/
int32_t close(int32_t fd){
	/*get the index of process that is running*/
	int ind=get_current_process();
	/*why start from 2: 0,1 are occupied by stdin and stdout*/
	if(fd < FD_START || fd >= PCB_FILE_NUMBER) return -1; 
	/*find the current pcb*/
	pcb_t * pcb = find_pcb(pcb_num[ind]);
	/*invalid pcb*/
	if (pcb == NULL) return -1;
	/*file is unused*/
	if(pcb->file[fd].flag == 0) return -1;

	/*reset flag to zero(un-used)*/
	pcb->file[fd].flag = 0;
	return pcb->file[fd].fop_jump->close_func();
}

/*
 * 	getargs: get the argument of the input string
 *  Inputs: buf --- buffer to store the arguments
 *			nbytes --- length of arguments in bytes
 *	Outputs: none
 *  Return Value: 0 is success, -1 if failed
 *	sides effect: none
 */
int32_t getargs(uint8_t *buf, int32_t nbytes){
	/*get the index of process that is running*/
	int ind=get_current_process();
	/*invalid length to read*/
	if(nbytes == 0)
		return -1;
	else
	{
		/*find the current pcb*/
		pcb_t* current_pcb = find_pcb(pcb_num[ind]);
		/*copy the arguments to buffer*/
		strncpy((int8_t*) buf , (int8_t*)(current_pcb -> args) , nbytes);
		return 0;
	}
}

/*
 * vidmap: map the user video memory with the given screen start address
 * Inputs: screen_start --- address of the screen start address
 * Outputs: none
 * Return value: 0 if success, -1 is failed
 * Sides effect: none
 */
int32_t vidmap (uint8_t ** screen_start){
	if ((uint32_t)screen_start < PROG_START || (uint32_t)screen_start + FOUR_KB >= PROG_START + FOUR_MB)
	{
		/*invalid cases*/
		return -1;
	}
	else
	{
		/*map the user video address to the given address*/
		*screen_start = (uint8_t *)USER_VIDEO_ADDR;
		return 0;
	}
}

/*
 * set_handler: not implement yet
 * Input: signum , handler_address
 * Output: none
 * Return value: -1
 * Sides effect: none
 */
int32_t set_handler(int32_t signum, void * hanlder_address){
	return -1;
}

/*
 * sigreturn: not implement yet
 * Input: none
 * Output: none
 * Return value: -1
 * Sides effect: none
 */
int32_t sigreturn(void){
	return -1;
}

/*
 * find_terminal_by_pcb: find the current pcb number according to the given terminal_number
 * INPUT: terminal_number --- the terminal_number in which the pcb number should be found
 * OUTPUT: correspond pcb number
 * return value: pcb_num
 * sides effect: none
 */
int32_t find_terminal_by_pcb(int terminal_number){
	return pcb_num[terminal_number];
}
