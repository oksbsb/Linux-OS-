/*
 * pcb.h - header file for PCB
 * Author:	    gyms guo
 * Version: 1
 *
 */
#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "syscall.h"



#ifndef ASM

/*All the file opeartion jump table needed*/
file_op fops[MAX_PCB_NUM][PCB_FILE_NUMBER];

/*File descriptor struct*/
typedef struct file_struct{

	file_op* fop_jump;
	uint32_t inode_ptr; /*Inode pointer*/
	uint32_t file_pos;  /*file position*/
	uint32_t flag;		/*flag,1 means used*/

} file_t;

/*pcb struct*/
typedef struct pcb_struct{

	struct pcb_struct * parent;
	file_t file[PCB_FILE_NUMBER];
	uint32_t esp,ebp;
	uint32_t sched_esp, sched_ebp;
	uint32_t pid;
	uint32_t retval;
	char args[EXECUTE_ARG];
	int32_t terminal_number;
}pcb_t;

/*The pcb pointer array*/
pcb_t* pcb_ptr[MAX_PCB_NUM];

/*Initialze the pcb pointer array*/
extern void init_pcb();
/*find a new pcb number*/
extern int32_t find_new_pcb();
/*find the pcb poiner address based on the pcb number*/
extern pcb_t* find_pcb(int32_t pcb_num);
/*Initialize the new pcb*/
extern int32_t new_pcb(pcb_t *cur_pcb_ptr,int32_t parent_pcb_num, int32_t pcb_num);
/*Read default function*/
int32_t read_default(int32_t fd,void *buf,int32_t nbytes);
/*Write default function*/
int32_t write_default(const void* buf, int32_t nbytes);
/*Open default function*/
int32_t open_default();
/*Close default function*/
int32_t close_default();

#endif /* ASM */

#endif /* PCB_H */






