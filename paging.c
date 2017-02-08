/*
 * paging.c - paging related fucntions
 * Author:	    gyms  guo
 * Version: 1
 *
 */
#include "paging.h"
#include "syscall.h"
#include "pcb.h"
#include "lib.h"

/**All the local functions, Refer to the header for more information***/

static void set_register();
static void map_page();

/*  
 *  set_paging
 *	DESCRIPTION: start paing and distribute memory location for different uses
 *  INPUT: None
 *  OUTPUT: None
 */
void set_paging()
{
	int i , j;  /*The index to the page directory and programs*/
	for(j = 0 ; j < TASK_NUMBER ; j ++)
	{
		for(i = 0 ; i < PAGE_DIRECTORY_SIZE ; i++)
		{
			/*initilize each pde in each program*/
			pd[j][i].P = 0;
			pd[j][i].R_W = 1;
			pd[j][i].U_S = 0;	// only kernel page can access 
			pd[j][i].PCD = 1;
			pd[j][i].base_address = 0;	/*set bits in pde to proper value*/

		}
	}
	for(i = 0 ; i < TASK_NUMBER ; i ++)
	{
		for(j = 0 ; j < PAGE_TABLE_SIZE ; j++)
		{
			/*initialize each pte in each program*/
			pt[i][j].P = 0;
			pt[i][j].R_W = 1;
			pt[i][j].U_S = 0;
			pt[i][j].PCD = 1;
			pt[i][j].base_address = i;	/*set bits in pde to proper value*/

		}
	}
	map_page();
	set_register(START_PDE);
}


/*  
 *  set_new_task
 *	DESCRIPTION: Update the page the page directory for a new process
 *  INPUT: pcb_num: indicate the process number, flag: indicate wheter to remap_page to the video memory
 *  OUTPUT: None
 */

void set_new_task(int pcb_num,int flag){
	if(flag==1){
		vidmem_pt[pcb_num][0].base_address = VIDEO >> PTE_SHIFT; /*remap_page only when flag indicates so*/
	}
	cli();				/*Critical sections*/
	set_register(pcb_num);	/*Set the registers*/
}


/*
 *	map_page
 *  DESCRIPTION: map_page the memeory location in different sizes with different entry
 *	INPUT: NONE
 *	OUTPUT: NONE
 *  RETURN VALUE: NONE
 *  SIDE EFFECT: NONE
 */
static void map_page()
{
	int i;
	for(i = 0 ; i < TASK_NUMBER ; i ++)
	{
		vidmem_pt[i][0].P = 1;		/*present*/
		vidmem_pt[i][0].R_W = 1;	/*read & write*/
		vidmem_pt[i][0].U_S = 1;	/*user level*/
		vidmem_pt[i][0].base_address = VIDEO >> PTE_SHIFT;/*fill the address*/

		vidmem_pt[i][1].P = 1;		/*present*/
		vidmem_pt[i][1].R_W = 1;	/*read & write*/
		vidmem_pt[i][1].U_S = 1;	/*user level*/
		vidmem_pt[i][1].base_address = (VIDEO+FOUR_KB)>> PTE_SHIFT;

		vidmem_pt[i][2].P = 1;		/*present*/
		vidmem_pt[i][2].R_W = 1;	/*read & write*/
		vidmem_pt[i][2].U_S = 1;	/*user level*/
		vidmem_pt[i][2].base_address = (VIDEO+TERMINAL_TWO *FOUR_KB) >> PTE_SHIFT;

		vidmem_pt[i][3].P = 1;		/*present*/
		vidmem_pt[i][3].R_W = 1;	/*read & write*/
		vidmem_pt[i][3].U_S = 1;	/*user level*/
		vidmem_pt[i][3].base_address = (VIDEO+TERMINAL_THREE*FOUR_KB) >> PTE_SHIFT;

		pd[i][START].P = 1;		/*present*/
		pd[i][START].R_W = 1;		/*read & write*/
		pd[i][START].U_S = 0;		/*user level*/
		pd[i][START].PS = 0;		/*4KB page*/
		pd[i][START].G = 1;
		pd[i][START].base_address = (((uint32_t)(&pt[i])) >> PDE_SHIFT_12);	/*fill the address*/
									//// pd[i][0]--- 0 to 4 MB, means the physical address is same as virtual address
									///// the mapping is same translation 
		pt[i][VIDEO_MEM].P = 1;	/*present*/
		pt[i][VIDEO_MEM].R_W = 1;	/*read & write*/
		pt[i][VIDEO_MEM].U_S = 0;	/*user level*/
		pt[i][VIDEO_MEM].G = 1;
		pt[i][VIDEO_MEM].base_address = VIDEO >> PTE_SHIFT;/*fill the address*/

		pd[i][KERNEL].P = 1;		/*present*/
		pd[i][KERNEL].R_W = 0;		/*read only*/
		pd[i][KERNEL].U_S = 0;		/*supervisor*/
		pd[i][KERNEL].PS = 1;		/*4MB page*/
		pd[i][KERNEL].G = 1;		/*Global*/
		pd[i][KERNEL].base_address = ((KERNEL_ADDR >> PDE_SHIFT_22) << PDE_SHIFT_10);/*fill the address*/

		pd[i][PROGRAM].P = 1;		/*present*/
		pd[i][PROGRAM].R_W = 1;		/*read & write*/
		pd[i][PROGRAM].U_S = 1;		/*user level*/
		pd[i][PROGRAM].PS = 1;		/*4MB page*/
		pd[i][PROGRAM].base_address = (((EIGHT_MB + i * FOUR_MB) >> PDE_SHIFT_22) << PDE_SHIFT_10);/*fill the address*/


		pd[i][USER_VIDEO].P = 1;		/*present*/
		pd[i][USER_VIDEO].R_W = 1;		/*read & write*/
		pd[i][USER_VIDEO].U_S = 1;		/*user level*/
		pd[i][USER_VIDEO].PS = 0;		/*4KB page*/
		pd[i][USER_VIDEO].base_address = ((uint32_t)&vidmem_pt[i]) >> PDE_SHIFT_12; 
		
	}

}


/*  
 *	set_register
 *  DESCRIPTION: set CR0, CR3, CR4 to proper value to start paging
 *	INPUT:int i: determine which page directory to use
 *	OUTPUT:NONE
 *  RETURN VALUE: NONE
 *  SIDE EFFECT: NONE
 */
static void set_register(int i)
{
	
	uint32_t cr0, cr4;
	asm volatile("movl %0, %%cr3"
				  :
				  :"r"(pd[i])
				  : "memory" );		/*CR3 contains the base address of pde table*/ 


	asm volatile("movl %%cr4, %0"
				  :"=a"(cr4));		/*CR4 Contains flags that enable several architectural extensions*/
	
	cr4=cr4|CR4_VAL;
	
	asm volatile("movl %0, %%cr4"
				 :
				 :"r"(CR4_VAL)
				 :"memory");

	asm volatile("movl %%cr0, %0"
				 :"=a"(cr0));	/*CR0 Contains system control flags*/ 
	
	cr0=cr0|CR0_VAL;
	
	asm volatile("movl %0, %%cr0"
				 :
				 :"r"(CR0_VAL)
				 : "memory" );
}

/*
 * remap_page: remap_page the user video memory to video memory or buffer in physical memory
 * Inputs: old termis --- index of terminal that should be mapped to buffer
 *  	   new_termis --- index of terminal that should be mapped to video memory
 * Outputs: none
 * Return value: none
 * Sides effect: none
 */
void remap_page(int previous_terminal , int new_termis)
{
	cli();
	int i;
	for(i = 0 ; i < MAX_PCB_NUM  ;i ++)
	{
		if(pcb_ptr[i] == NULL)	
			continue;
		if(pcb_ptr[i] -> terminal_number == previous_terminal)	/*Processes of the old terminal will be mapped to the buffer address*/
			vidmem_pt[i][0].base_address = (VIDEO + (previous_terminal+1) * FOUR_KB) >> PTE_SHIFT;
		if(pcb_ptr[i] -> terminal_number == new_termis)	/*Processes of the new terminal will be mapped to the video memory*/
			vidmem_pt[i][0].base_address = VIDEO >> PTE_SHIFT;
	}


	// Flush the tlb
	asm volatile("movl %%cr3 , %%eax":);			
	asm volatile("movl %%eax , %%cr3":);
	return;
}

/*
 * restore_page: remap_page the user video memory to video memory in physical memory
 * Input: pcb_num --- index of process
 * Output: none
 * Return value: none
 * Sides effect: none
 */
void restore_page(int pcb_num){
	vidmem_pt[pcb_num][0].base_address = VIDEO >> PTE_SHIFT; /*Restore the page to the default value*/
} 


