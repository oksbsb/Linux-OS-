/* types.h - Defines to use the familiar explicitly-sized types in this
 * OS (uint32_t, int8_t, etc.).  This is necessary because we don't want
 * to include <stdint.h> when building this OS
 * vim:ts=4 noexpandtab
 */

#ifndef _TYPES_H
#define _TYPES_H

#define NULL 0
/*All the magic numbers for paging */ 
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define TASK_NUMBER 10
#define START 0 
#define KERNEL 1
#define PROGRAM 32
#define USER_VIDEO 33
#define VIDEO_MEM 0xB8
#define KERNEL_ADDR 0x400000  
#define USER_VIDEO_ADDR 0x8400000
#define	CR3_VALUE 0x00000000
#define	CR4_VAL 0x00000010
#define CR0_VAL 0x80000001
#define VIDEO 0xB8000  
#define START_PDE 0
#define PTE_SHIFT 12
#define PDE_SHIFT_12 12
#define PDE_SHIFT_22 22
#define PDE_SHIFT_10 10
#define _4KB 4096 
/////////////////////////////////////////
 /*All the magic number for keyboard */ 
#define KB_DATA_PORT 0x60
#define KB_IRQ_NUM	1
#define KB_ARRAY_NUM 128
#define KB_NUMBER 3

#define ALT_ON 0x38
#define ALT_OFF 0xB8

#define F1_ON 0x3B
#define F2_ON 0x3C
#define F3_ON 0x3D

#define CAPS_LOCK_ON 0X3A
#define CAPS_LOCK_OFF 0xBA

#define LEFT_SHIFT_ON 0x2A
#define LEFT_SHIFT_OFF 0xAA

#define	RIGHT_SHIFT_ON 0x36
#define	RIGHT_SHIFT_OFF 0xB6

#define LEFT_CTRL_ON 0x1D
#define LEFT_CTRL_RELEASED 0x9D

#define RIGHT_CTRL_PRESSED_1 0xE0
#define RIGHT_CTRL_PRESSED_2 0x1D
#define RIGHT_CTRL_RELEASED_1 0xE0
#define RIGHT_CTRL_RELEASED_2 0x9D

#define UP_PRESSED 0x48		/* < 128 */
#define UP_RELEASED 0xC8	/* > 128 */


#define DOWN_PRESSED 0x50	/* < 128 */
#define DOWN_RELEASED 0xD0	/* > 128 */


#define LEFT_PRESSED 0x4B	/* < 128 */
#define LEFT_RELEASED 0xCB	/* > 128 */

#define RIGHT_PRESSED 0x4D	/* < 128 */
#define RIGHT_RELEASED 0xCD	/* > 128 */


#define HEIGHT 25
#define WIDTH 80
#define REG_PORT 0x3D4
#define DATA_PORT 0x3D5
#define HIGH_BITS_REG 14
#define LOW_BITS_REG 15
#define BIT_MASK_8 0xFF
#define MAX_BUFFER_SIZE 128
#define DOUBLE 2
#define MAX_SIZE  0x1000
#define COLOR_SWITCH 4
#define WHITE 0x0F
#define FIRST_ROW 0
#define SECOND_ROW 1
#define THIRD_ROW 2
#define FOURTH_ROW 3
#define TAB_SPACE_NUM 4

/*scan codes*/
#define SCAN_CODE_A 0x1E
#define SCAN_CODE_L 0x26
#define SCAN_CODE_Q 0x10
#define SCAN_CODE_P 0x19
#define SCAN_CODE_Z 0x2C
#define SCAN_CODE_M 0x32

#define CURSOR_SHIFT_BITS 8

/////////////////////////////
/*All the magic numbers for RTC */
#define RTC_A  0x0A
#define RTC_B  0x0B
#define RTC_C  0x0C
#define FREQ_NUM 10
#define RTC_NO 1
#define RTC_OCCURED 0
#define RATE_MASK 0x0f
#define ORIG_MASK 0xf0
#define RATE_CONVERTER 15
#define TWO_HERZ 15

#define RTC_CMD  0x70
#define RTC_DATA 0x71

#define PIE_BIT  0x40

#define RTC_IRQ_NUM  8

#define RTC_WRITE 4
////////////////////////////////////////////////

/*magic numbers for PIC */
#define PIC_INPUT_HZ 1193180
#define CW 0x34
#define CW_PORT 0x43
#define COUNTER_PIC 0x40
#define PIT_IRQ_NUM  0
#define LSB 0xFF
#define MSB 0xFF00
#define PIT_FREQ 100
///////////////////////////////////////////////////////
/*All the magic numbers for terminal */
#define TOTAL_TERMINALS 3
#define TERMINAL_ONE 1
#define TERMINAL_TWO 2
#define TERMINAL_THREE 3

/*magic numbers for system call */
#define EXECUTE_FILES 11
#define EXECUTE_NAME 20
#define EXECUTE_ARG 128
#define ELF_CHECK_NUM 4
#define ELF_MAGIC 0x464c457f
#define FILE_START 0x08048000
#define KERNEL_BOTTOM 0x7fffff
#define STACK_SIZE 0x2000
#define PCB_MASK 0xffffe000
#define FILE_ENTRY_OFFSET 6
#define MAX_COMMAND_INPTUT 128
#define ELF_BYTE_NUM 4
#define USER_STACK_BOTTOM 0x083ffffc
#define IF_MASK 0x200

#define IN_USE 1
#define NOT_IN_USE 0
#define FOUR_KB 0x1000
#define FOUR_MB 0x400000
#define EIGHT_MB 0x800000
#define PROG_START 0x8000000
///////////////////////////////////////////
/*magic number for PCB */ 
#define PCB_FILE_NUMBER 8
#define MAX_PCB_NUM 6
#define STDIN 0
#define STDOUT 1
////////////////////////////////////////////
/*magic number for PIC: Ports that each PIC sits on */
#define MASTER_8259_CMD 0x20
#define MASTER_8259_IMR	 0x21
#define SLAVE_8259_CMD  0xA0
#define SLAVE_8259_IMR	 0xA1

#define MASK_ALL 0xff

#define MASTER_NUM 8
#define SLAVE_PIC_IRQ 2

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1    0x11
#define ICW2_MASTER   0x20
#define ICW2_SLAVE    0x28
#define ICW3_MASTER   0x04
#define ICW3_SLAVE    0x02
#define ICW4          0x01

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI             0x60
//////////////////////////////////////////
/*All the magic numbers for file system */
#define INODES_OFFSET 1
#define DATA_BLK_OFFSET 1

#define OFFSET_4B 0x4
#define FILE_NAME_32B 0x20
#define ENTRY_SIZE_64B 0x40
#define BLOCK_4KB 0x1000
#define DIR_BUF_SIZE 33
#define FILE_NAME_LENGTH 32
#define DENTRY_RESERVED_LEN 24
#define RTC_FILE 0
#define DIR_FILE 1
#define REGULAR_FILE 2
#define FD_START 2
#define MAX_FILE_NAME 32

#ifndef ASM

/* Types defined here just like in <stdint.h> */
typedef int int32_t;
typedef unsigned int uint32_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef char int8_t;
typedef unsigned char uint8_t;

/*Register stack struct*/
typedef struct stack_register{

	uint16_t gs,fs,es,ds; /*The segment regisers on the stack*/
	uint32_t edi,esi,ebp,esp,edx,ecx,ebx,eax; /*The gerenal register*/
	uint32_t interrupt_num, error_code;

}stack_register;

/*Interrup handler function pointer*/
typedef void (*do_irq)(stack_register reg);
/*File operation pointers*/
typedef int32_t (*read_op)(int32_t fd,void *buf,int32_t nbytes);
typedef int32_t (*write_op)(const void* buf, int32_t nbytes);
typedef int32_t (*open_op)();
typedef int32_t (*close_op)();

/*File operation table struct*/
typedef struct file_op{
	/*fop*/
	read_op read_func; 
	write_op write_func; 
	open_op open_func;
	close_op close_func;

}file_op;

#endif /* ASM */

#endif /* _TYPES_H */
