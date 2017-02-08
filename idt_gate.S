.text

/*****For all the exceptions******/
.global exception_num0
.global exception_num1
.global exception_num2
.global exception_num3
.global exception_num4
.global exception_num5
.global exception_num6
.global exception_num7
.global exception_num8
.global exception_num9
.global exception_num10
.global exception_num11
.global exception_num12
.global exception_num13
.global exception_num14
.global exception_num15
.global exception_num16
.global exception_num17
.global exception_num18
.global exception_num19
.global exception_num20
.global exception_num21
.global exception_num22
.global exception_num23
.global exception_num24
.global exception_num25
.global exception_num26
.global exception_num27
.global exception_num28
.global exception_num29
.global exception_num30
.global exception_num31

/******For all the interrupt********/
.global interrupt_num0
.global interrupt_num1
.global interrupt_num2
.global interrupt_num3
.global interrupt_num4
.global interrupt_num5
.global interrupt_num6
.global interrupt_num7
.global interrupt_num8
.global interrupt_num9
.global interrupt_num10
.global interrupt_num11
.global interrupt_num12
.global interrupt_num13
.global interrupt_num14
.global interrupt_num15

/*************system call*************/
.global system_call

/********Exception(exception_num)************/
exception_num0:
	cli
	pushl $0
	pushl $0
	jmp common_exception_num

exception_num1:
	cli
	pushl $0
	pushl $1
	jmp common_exception_num
	
exception_num2:
	cli
	pushl $0
	pushl $2
	jmp common_exception_num

exception_num3:
	cli
	pushl $0
	pushl $3
	jmp common_exception_num

exception_num4:
	cli
	pushl $0
	pushl $4
	jmp common_exception_num

exception_num5:
	cli
	pushl $0
	pushl $5
	jmp common_exception_num

exception_num6:
	cli
	pushl $0
	pushl $6
	jmp common_exception_num

exception_num7:
	cli
	pushl $0
	pushl $7
	jmp common_exception_num

exception_num8:
	cli
	pushl $8
	jmp common_exception_num

exception_num9:
	cli
	pushl $0
	pushl $9
	jmp common_exception_num
	
exception_num10:
	cli
	pushl $10
	jmp common_exception_num

exception_num11:
	cli
	pushl $11
	jmp common_exception_num

exception_num12:
	cli
	pushl $12
	jmp common_exception_num

exception_num13:
	cli
	pushl $13
	jmp common_exception_num

exception_num14:
	cli
	pushl $14
	jmp common_exception_num

exception_num15:
	cli
	pushl $0
	pushl $15
	jmp common_exception_num

exception_num16:
	cli
	pushl $0
	pushl $16
	jmp common_exception_num

exception_num17:
	cli
	pushl $0
	pushl $17
	jmp common_exception_num

exception_num18:
	cli
	pushl $0
	pushl $18
	jmp common_exception_num

exception_num19:
	cli
	pushl $0
	pushl $19
	jmp common_exception_num

exception_num20:
	cli
	pushl $0
	pushl $20
	jmp common_exception_num

exception_num21:
	cli
	pushl $0
	pushl $21
	jmp common_exception_num

exception_num22:
	cli
	pushl $0
	pushl $22
	jmp common_exception_num

exception_num23:
	cli
	pushl $0
	pushl $23
	jmp common_exception_num

exception_num24:
	cli
	pushl $0
	pushl $24
	jmp common_exception_num

exception_num25:
	cli
	pushl $0
	pushl $25
	jmp common_exception_num

exception_num26:
	cli
	pushl $0
	pushl $26
	jmp common_exception_num

exception_num27:
	cli
	pushl $0
	pushl $27
	jmp common_exception_num

exception_num28:
	cli
	pushl $0
	pushl $28
	jmp common_exception_num

exception_num29:
	cli
	pushl $0
	pushl $29
	jmp common_exception_num

exception_num30:
	cli
	pushl $0
	pushl $30
	jmp common_exception_num

exception_num31:
	cli
	pushl $0
	pushl $31
	jmp common_exception_num

/*****Interrupt(interrupt_num)*********/
interrupt_num0:
	pushl $0   /*Not really neccessay*/
	pushl $-1
	jmp common_interrupt_num
interrupt_num1: 
	pushl $0 
	pushl $-2
	jmp common_interrupt_num
interrupt_num2:
	pushl $0 
	pushl $-3
	jmp common_interrupt_num
interrupt_num3:
	pushl $0 
	pushl $-4
	jmp common_interrupt_num
interrupt_num4:
	pushl $0 
	pushl $-5
	jmp common_interrupt_num
interrupt_num5:
	pushl $0 
	pushl $-6
	jmp common_interrupt_num
interrupt_num6:
	pushl $0 
	pushl $-7
	jmp common_interrupt_num
interrupt_num7:
	pushl $0 
	pushl $-8
	jmp common_interrupt_num
interrupt_num8:
	pushl $0 
	pushl $-9
	jmp common_interrupt_num
interrupt_num9:	
	pushl $0 
	pushl $-10
	jmp common_interrupt_num
interrupt_num10:
	pushl $0 
	pushl $-11
	jmp common_interrupt_num
interrupt_num11:
	pushl $0 
	pushl $-12
	jmp common_interrupt_num
interrupt_num12:
	pushl $0 
	pushl $-13
	jmp common_interrupt_num
interrupt_num13:
	pushl $0 
	pushl $-14
	jmp common_interrupt_num
interrupt_num14:
	pushl $0 
	pushl $-15
	jmp common_interrupt_num
interrupt_num15:
	pushl $0 
	pushl $-16
	jmp common_interrupt_num
# int system_call(void);
#
# Interface: C-style
#    Inputs: %eax, and all the other arguments
#   Outputs: none
# Registers: 
system_call:
	pushal         #Push all the gereneral registers

	pushw %ds      #Push the segment registers
	pushw %es
	pushw %fs
	pushw %gs

	pushl %eax				   # Save the %eax 

	movw $0x18,%ax             # 0x18 corresponds to KD segment
	movw %ax,%ds			   # Modify the segment registers
	movw %ax,%es	
	movw %ax,%fs	
	movw %ax,%gs	

	popl %eax
	pushl %edx
	pushl %ecx
	pushl %ebx

	cmpl  $10,%eax				#check whether the value of eax is valid between 1 to 10 
	jg error
	cmpl $1,%eax
	jl error

	sti
	call *syscall_jump(,%eax,4)	   # Restore all the segament regisers except the CS
	cli
	jmp correct

error:
	movl $-1,%eax 					# invliad eax
correct:
	addl $12,%esp				    # for the three argument 
	popw %gs
	popw %fs
	popw %es
	popw %ds

	movl %eax,error_code			#save the error code
	popal
	movl error_code,%eax
	
	iret
	
# void common_exception_num(void);
#
# Interface: C-style
#    Inputs: none
#   Outputs: none
# Registers: 
common_exception_num:
	pushal         #Push all the gereneral registers

	pushw %ds      #Push the segament registers
	pushw %es
	pushw %fs
	pushw %gs

	movw $0x18,%ax             #0x18 corresponds to Kernel Data segment
	movw %ax,%ds			   #Modify the segment registers
	movw %ax,%es	
	movw %ax,%fs	
	movw %ax,%gs	

	call exception_handler
					           #Restore all the segament regisers except the CS
	popw %gs
	popw %fs
	popw %es
	popw %ds

	popal

	addl $8, %esp			  #pop the arguemnt

	sti
	iret

# void common_interrupt_num(void);
#
# Interface: C-style
#    Inputs: none
#   Outputs: none
# Registers: 
common_interrupt_num:
	pushal         #Push all the gereneral registers

	pushw %ds      #Push the segament registers
	pushw %es
	pushw %fs
	pushw %gs

	movw $0x18,%ax             #0x18 corresponds to Kernel Data segment
	movw %ax,%ds			   #Modify the segment registers
	movw %ax,%es	
	movw %ax,%fs	
	movw %ax,%gs	

	sti
	call interrupt_handler	           #Restore all the segament regisers except the CS
	cli
	popw %gs
	popw %fs
	popw %es
	popw %ds

	popal

	addl $8, %esp				#pop argument
	iret

.align 4
syscall_jump:					#system call jump table
	.long 0
	.long halt
	.long execute
	.long read,write,open,close
	.long getargs
	.long vidmap
	.long set_handler
	.long sigreturn 

error_code:						#store the error code temorarily
	.long 0











	











