/*
 * idt_gate.S - entry fucntion for idt table
 * Author:	    gyms guo
 * Version: 1
 *
 */

#include "syscall.h"

#ifndef IDT_GATE_H
#define IDT_GATE_H

extern void exception_num0(void);
extern void exception_num1(void);
extern void exception_num2(void);
extern void exception_num3(void);
extern void exception_num4(void);
extern void exception_num5(void);
extern void exception_num6(void);
extern void exception_num7(void);
extern void exception_num8(void);
extern void exception_num9(void);
extern void exception_num10(void);
extern void exception_num11(void);
extern void exception_num12(void);
extern void exception_num13(void);
extern void exception_num14(void);
extern void exception_num15(void);
extern void exception_num16(void);
extern void exception_num17(void);
extern void exception_num18(void);
extern void exception_num19(void);
extern void exception_num20(void);
extern void exception_num21(void);
extern void exception_num22(void);
extern void exception_num23(void);
extern void exception_num24(void);
extern void exception_num25(void);
extern void exception_num26(void);
extern void exception_num27(void);
extern void exception_num28(void);
extern void exception_num29(void);
extern void exception_num30(void);
extern void exception_num31(void);

extern void interrupt_num0(void);
extern void interrupt_num1(void);
extern void interrupt_num2(void);
extern void interrupt_num3(void);
extern void interrupt_num4(void);
extern void interrupt_num5(void);
extern void interrupt_num6(void);
extern void interrupt_num7(void);
extern void interrupt_num8(void);
extern void interrupt_num9(void);
extern void interrupt_num10(void);
extern void interrupt_num11(void);
extern void interrupt_num12(void);
extern void interrupt_num13(void);
extern void interrupt_num14(void);
extern void interrupt_num15(void);

extern int system_call(void);

#endif /* IDT_GATE_H */












