/*
 * idt.c - support functions for set idt entry and install idt
 * Author:	    gyms guo
 * Version: 1
 *
 */

 #include "x86_desc.h"
 #include "types.h"
 #include "idt_gate.h"
 #include "idt.h"
 /*Local fucntion, see headers for more details*/
 static void set_idt_gate(int num, uint32_t handler,uint16_t seg,int size_flag,int dpl);


/*  install_idt
 *	 DESCRIPTION: Install the IDT table
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void install_idt(){
 
	/*Install the IDT entry for exception*/
	set_idt_gate(0, (uint32_t)exception_num0,KERNEL_CS,1,0);
	set_idt_gate(1, (uint32_t)exception_num1,KERNEL_CS,1,0);
	set_idt_gate(2, (uint32_t)exception_num2,KERNEL_CS,1,0);
	set_idt_gate(3, (uint32_t)exception_num3,KERNEL_CS,1,0);
	set_idt_gate(4, (uint32_t)exception_num4,KERNEL_CS,1,0);
	set_idt_gate(5, (uint32_t)exception_num5,KERNEL_CS,1,0);
	set_idt_gate(6, (uint32_t)exception_num6,KERNEL_CS,1,0);
	set_idt_gate(7, (uint32_t)exception_num7,KERNEL_CS,1,0);
	set_idt_gate(8, (uint32_t)exception_num8,KERNEL_CS,1,0);
	set_idt_gate(9, (uint32_t)exception_num9,KERNEL_CS,1,0);
	set_idt_gate(10, (uint32_t)exception_num10,KERNEL_CS,1,0);
	set_idt_gate(11, (uint32_t)exception_num11,KERNEL_CS,1,0);
	set_idt_gate(12, (uint32_t)exception_num12,KERNEL_CS,1,0);
	set_idt_gate(13, (uint32_t)exception_num13,KERNEL_CS,1,0);
	set_idt_gate(14, (uint32_t)exception_num14,KERNEL_CS,1,0);
	set_idt_gate(15, (uint32_t)exception_num15,KERNEL_CS,1,0);
	set_idt_gate(16, (uint32_t)exception_num16,KERNEL_CS,1,0);
	set_idt_gate(17, (uint32_t)exception_num17,KERNEL_CS,1,0);
	set_idt_gate(18, (uint32_t)exception_num18,KERNEL_CS,1,0);
	set_idt_gate(19, (uint32_t)exception_num19,KERNEL_CS,1,0);
	set_idt_gate(20, (uint32_t)exception_num20,KERNEL_CS,1,0);
	set_idt_gate(21, (uint32_t)exception_num21,KERNEL_CS,1,0);
	set_idt_gate(22, (uint32_t)exception_num22,KERNEL_CS,1,0);
	set_idt_gate(23, (uint32_t)exception_num23,KERNEL_CS,1,0);
	set_idt_gate(24, (uint32_t)exception_num24,KERNEL_CS,1,0);
	set_idt_gate(25, (uint32_t)exception_num25,KERNEL_CS,1,0);
	set_idt_gate(26, (uint32_t)exception_num26,KERNEL_CS,1,0);
	set_idt_gate(27, (uint32_t)exception_num27,KERNEL_CS,1,0);
	set_idt_gate(28, (uint32_t)exception_num28,KERNEL_CS,1,0);
	set_idt_gate(29, (uint32_t)exception_num29,KERNEL_CS,1,0);
	set_idt_gate(30, (uint32_t)exception_num30,KERNEL_CS,1,0);
	set_idt_gate(31, (uint32_t)exception_num31,KERNEL_CS,1,0);

	/*****Install the IDT entry for intterrupt*******/
	set_idt_gate(32, (uint32_t)interrupt_num0,KERNEL_CS,1,0);
	set_idt_gate(33, (uint32_t)interrupt_num1,KERNEL_CS,1,0);
	set_idt_gate(34, (uint32_t)interrupt_num2,KERNEL_CS,1,0);
	set_idt_gate(35, (uint32_t)interrupt_num3,KERNEL_CS,1,0);
	set_idt_gate(36, (uint32_t)interrupt_num4,KERNEL_CS,1,0);
	set_idt_gate(37, (uint32_t)interrupt_num5,KERNEL_CS,1,0);
	set_idt_gate(38, (uint32_t)interrupt_num6,KERNEL_CS,1,0);
	set_idt_gate(39, (uint32_t)interrupt_num7,KERNEL_CS,1,0);
	set_idt_gate(40, (uint32_t)interrupt_num8,KERNEL_CS,1,0);
	set_idt_gate(41, (uint32_t)interrupt_num9,KERNEL_CS,1,0);
	set_idt_gate(42, (uint32_t)interrupt_num10,KERNEL_CS,1,0);
	set_idt_gate(43, (uint32_t)interrupt_num11,KERNEL_CS,1,0);
	set_idt_gate(44, (uint32_t)interrupt_num12,KERNEL_CS,1,0);
	set_idt_gate(45, (uint32_t)interrupt_num13,KERNEL_CS,1,0);
	set_idt_gate(46, (uint32_t)interrupt_num14,KERNEL_CS,1,0);
	set_idt_gate(47, (uint32_t)interrupt_num15,KERNEL_CS,1,0);
	/*************system call*************************/
	set_idt_gate(0x80, (uint32_t)system_call,KERNEL_CS,1,USER_MODE);
	/*Load the IDTR register*/
	lidt((uint32_t)(&idt_desc_ptr));
	/*lidt(idt_desc_ptr);*/
}




/*  set_idt_gate
 *	 DESCRIPTION: Set the IDT entry based on the input value
 *   INPUTS: num: the num in the IDT table, hanlder: the base adderss
 *           size_flag: determine the size of the entry
 *           dpl: determine the DPL
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
*/
void set_idt_gate(int num, uint32_t handler,uint16_t seg,int size_flag,int dpl){

  /*Set all the reserved bits and present bits*/
	idt[num].reserved4 = 0x00;
	idt[num].reserved3 = 0;
	idt[num].reserved2 = 1;
	idt[num].reserved1 = 1;
	idt[num].reserved0 = 0;
	idt[num].present = 1;

  /*Set the base address*/
	SET_IDT_ENTRY(idt[num], handler);

  /*Set the remaing flags*/
	idt[num].size=size_flag;
	idt[num].dpl=dpl;
	idt[num].seg_selector=seg;

}



