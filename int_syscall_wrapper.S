.text # The text section is used for keeping the actual code

#define ASM     1
#include "x86_desc.h"

.global context_switch
.global halt_ret

invalid:
    movl $-1, %eax

context_switch:

    cli
    movl 4(%esp),%ebx       # ebx contains eip value

    movl $USER_DS, %eax     # load user data segment to the below registers
    movw %ax, %gs 
    movw %ax, %fs  
    movw %ax, %ds 
    movw %ax, %es

    pushl %eax              # push user data segment

    pushl $0x83FFFFC        # push the user space esp

    pushf                   # modify flags
    popl  %eax             
    orl   $0x200, %eax     # altered appropriately

    pushl %eax              # push the altered flags back onto the stack
    pushl $USER_CS          # push the user code segment to the stack
    pushl %ebx              # push the eip to the stack

    iret                    # return from interruption, go to the first line of instruction

halt_ret:             # halt should return here

    leave
    ret
