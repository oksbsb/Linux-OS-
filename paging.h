/*
 * paging.h - header for all the paging related fucntions
 * Author:	    gyms  guo
 * Version: 1
 *
 */
#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

 #ifndef ASM

/*See ref-manual-vol-3 p90*/
/*pde struct*/
typedef struct pde
{
		struct 
		{
			uint8_t P : 1;
			uint8_t R_W : 1;
			uint8_t U_S : 1;
			uint8_t PWT : 1;
			uint8_t PCD : 1;
			uint8_t A : 1;
			uint8_t zero : 1;
			uint8_t PS : 1;
			uint8_t G : 1;
			uint8_t avail : 3;
			uint32_t base_address : 20;
		} __attribute__((packed));
}pde; 

/*Pte struct*/
typedef struct pte
{
		struct 
		{
			uint8_t P : 1;
			uint8_t R_W : 1;
			uint8_t U_S : 1;
			uint8_t PWT : 1;
			uint8_t PCD : 1;
			uint8_t A : 1;
			uint8_t D : 1;
			uint8_t PAT : 1;
			uint8_t G : 1;
			uint8_t avail : 3;
			uint32_t base_address : 20;
		} __attribute__((packed));
	
}pte; 

pde pd[TASK_NUMBER][PAGE_DIRECTORY_SIZE] __attribute__((aligned(_4KB)));    /*Page directory for each programs*/
pte pt[TASK_NUMBER][PAGE_TABLE_SIZE] __attribute__((aligned(_4KB)));	     /*Page table for each programs*/
pte vidmem_pt[TASK_NUMBER][PAGE_TABLE_SIZE] __attribute__((aligned(_4KB))); /*Another page table for each programs*/

#endif /*ASM*/

/*Initialze the paging*/
extern void set_paging();
/*Add a new page directory*/
extern void set_new_task(int pcb_num,int flag);
/*remap_page when preforming the terminal switch*/
extern void remap_page(int previous_terminal , int new_termis);
/*Restore the page directory*/
extern void restore_page(int pcb_num);

#endif /* _PAGING_H */

