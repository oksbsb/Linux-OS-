/*
 * kb.c - support funciton for the keyboard 
 * Author:	    gyms guo
 * Version: modified at 12/2/2016  15:50
 *
 */
#include "paging.h"
#include "syscall.h"
#include "pcb.h"
#include "scheduling.h"
#include "i8259.h"
#include "irq.h"
#include "types.h"
#include "lib.h"
#include "kb.h"


static int terminal_number=0;    /*Current active terminal number*/
static int alt_pressed = 0; /*Indicate whether has been pressed*/

static void kb_handler();
/* US Keyboard Layout. Adapted from 0Sdev.net */
static uint8_t keyboard_array[KB_NUMBER][KB_ARRAY_NUM] = { 
		{ 
			// # 0 to # 9
			'\0', 27, '1', '2', '3', '4', '5', '6', '7', '8', 
			// # 10 to # 19
			'9', '0', '-', '=', '\b', /* Backspace */ '\t', /* Tab */'q', 'w', 'e', 'r',
			// # 20 to # 29
			't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */ '\0', /* Ctrl */
			// # 30 to # 39
		  	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
		  	// # 40 to # 49
		 	'\'', '`', '\0', /* Left shift */ '\\', 'z', 'x', 'c', 'v', 'b', 'n',
		 	// # 50 to # 59
		  	'm', ',', '.', '/', '\0', /* Right shift */ '\0', '\0', ' ', '\0', /* CapsLock */ '\0',
		  	// # 60 to # 63
		  	'\0', '\0', '\0', '\0'
		},

		/*symbol table*/
		{ 
			0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b'/*backspace*/, 0/*tab*/,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '{', '}', '\n'/*enter_flag*/, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, ':', '"', '~', 0/*shift*/, '|', 0, 0, 0, 0,
			0, 0, 0, '<', '>', '?', 0, 0, 0, ' '
		},

		/*caps letters table*/
		{ 
			0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 0, 0, 0, 0, 'A', 'S',
			'D', 'F', 'G', 'H', 'J', 'K', 'L', 0, 0, 0, 0, 0, 'Z', 'X', 'C', 'V',
			'B', 'N', 'M' 
		}};



/*   
 *	 set_kb
 *	 DESCRIPTION: initialize the keyboard and install the handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: none
 */

void set_kb(){
	set_interrupt(KB_IRQ_NUM,kb_handler);
}



/*
 * get_terminal_number: return the current terminal_number to somewhere
 * INPUTS: none
 * OUTPUTS: terminal_number
 * RETURN_VAL: terminal_number
 * sides effect: none
 */
int32_t get_terminal_number()
{
	return terminal_number;
}


/*
 * keyboard_console_putc: write character into (cursor_x , cursor_y)
 * INPUT: x,y --- indicate where to print the character , ch --- character which should be printed
 * OUTPUT: character on the console
 * side effect: none
 */
 void keyboard_console_putc(uint16_t x , uint16_t y , char ch)
{
	/*data of a character consists of 2 bytes: | Higher Byte | Lower Byte |
	  Higer Byte contains the ASCII code , Lower Byte contains the color
	  Higer Byte has lower address , Lower Byte has higher address*/

	/*get the offset of (x , y) in video memory*/
	uint16_t cur_offset = (y * WIDTH + x) * DOUBLE;					

	/*get the address of higher byte in video memory*/
	char* high_byte_addr = (char*)(cur_offset + VIDEO);

	/*fill in the character*/
	*high_byte_addr = ch;											

	/*fill in the color*/
	char* low_byte_addr = high_byte_addr + 1;						
	
	*low_byte_addr = WHITE; 
	return;
}

/*
 * keyboard_console_getc: get a character which is located at (x , y)
 * INPUT: x,y --- indicate where to get the character
 * OUTPUT: character from the console
 * side effect: none
 */
 char keyboard_console_getc(uint16_t x , uint16_t y)
{
	char* addr = (char*)(VIDEO + (y * WIDTH + x) * DOUBLE);			/*Get the characatet from the video memory from specific location*/
	char ch = *addr;
	return ch;
}


/*   
 *	 kb_handler
 *	 DESCRIPTION: handlering the keyboard
 *   INPUTS: reg the register on the stack
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

static void kb_handler(){
	char character;
	uint8_t scan_code;
	uint16_t old_cursor_x = 0;/*x position before backspace*/
	uint16_t old_cursor_y = 0;/*y position before backspace*/
	old_cursor_x = termis[terminal_number].cursor_x;
	old_cursor_y = termis[terminal_number].cursor_y;
	cli();     				 		 /*Start the critical section*/
	scan_code = inb(KB_DATA_PORT);   /* Read from the keyboard's data buffer */
	if(scan_code >= KB_ARRAY_NUM)
	{
		/*if caps lock is released*/
		if(scan_code == CAPS_LOCK_OFF)
		{
			termis[terminal_number].caps_on = 0;
			termis[terminal_number].caps_off = 1;
			return;
		}

		/*set flags according to differnet special keys*/
		if(scan_code == ALT_OFF)
		{
			alt_pressed = 0;
			return;
		}

		/*if right ctrl is released*/
		if((scan_code == RIGHT_CTRL_RELEASED_1) || (scan_code == RIGHT_CTRL_RELEASED_2))
		{
			termis[terminal_number].ctrl = 0;
			return;
		}

		/*if caps lock is released*/
		if((scan_code == RIGHT_CTRL_PRESSED_1) || (scan_code == RIGHT_CTRL_PRESSED_2))
		{
			termis[terminal_number].ctrl = 1;
			return;
		}

		/*if left ctrl is released*/
		if((scan_code == LEFT_SHIFT_OFF) || (scan_code == RIGHT_SHIFT_OFF))
		{
			termis[terminal_number].shift = 0;
			return;
		}

		/*if left ctrl is released*/
		if(scan_code == LEFT_CTRL_RELEASED)
		{
			termis[terminal_number].ctrl = 0;
			return;
		}

		/*if left is released*/
		if(scan_code == LEFT_RELEASED)
		{
			termis[terminal_number].left = 0;
			return;
		}
		/*if up is released*/
		if(scan_code == UP_RELEASED)
		{
			termis[terminal_number].up = 0;
			return;
		}
		/*if down is released*/
		if(scan_code == DOWN_RELEASED)
		{
			termis[terminal_number].down = 0;
			return;
		}
		
		/*if right is released*/
		if(scan_code == RIGHT_RELEASED)
		{
			termis[terminal_number].right = 0;
			return;
		}

	}

	else
	{

		/*if left ctrl is pressed*/
		if(scan_code == LEFT_CTRL_ON)
		{
			termis[terminal_number].ctrl = 1;
			return;
		}
		/*if right ctrl is pressed*/
		if((scan_code == RIGHT_CTRL_PRESSED_1) || (scan_code == RIGHT_CTRL_PRESSED_2))
		{
			termis[terminal_number].ctrl = 1;
			return;
		}
		/*if left ctrl is pressed*/
		if((scan_code == LEFT_SHIFT_ON) || (scan_code == RIGHT_SHIFT_ON))
		{
			termis[terminal_number].shift = 1;
			return;
		}
		/*if up is pressed*/
		if(scan_code == UP_PRESSED)
		{   
			termis[terminal_number].up = 1;
		}
		/*if down is pressed*/
		if(scan_code == DOWN_PRESSED)
		{
			termis[terminal_number].down = 1;
		}
		/*if left is pressed*/
		if(scan_code == LEFT_PRESSED)
		{
			termis[terminal_number].left = 1;
		}
		/*if right is pressed*/
		if(scan_code == RIGHT_PRESSED)
		{
			termis[terminal_number].right = 1;
		}
		/*check scan code and set flag*/
		if(scan_code == ALT_ON)
		{
			alt_pressed = 1;
			return;
		}

		if(scan_code == CAPS_LOCK_ON)
		{

			/*if caps lock was not pressed */
			if((termis[terminal_number].caps_on == 0) && (termis[terminal_number].caps_off == 1))
			{
				termis[terminal_number].caps_lock = !termis[terminal_number].caps_lock;
				termis[terminal_number].caps_on = 1;
				termis[terminal_number].caps_off = 0;
			}
			return;
		}

		if(scan_code == F1_ON && alt_pressed == 1)
		{
			cli();
			int old_terminal_number = terminal_number;
			if(terminal_number==0) /*Check to see if this is terminal 1*/
				return;
			/*remap_page paging*/
			remap_page(old_terminal_number, 0);
			/*copy data from video memory into current terminal's buffer*/
			memcpy( (void*)(USER_VIDEO_ADDR+(old_terminal_number+1)*FOUR_KB) , (void*)VIDEO, (uint32_t)FOUR_KB);
			/*copy data from next terminal's buffer into video memory*/
			memcpy((void*)VIDEO , (void*)(USER_VIDEO_ADDR+FOUR_KB) , (uint32_t)FOUR_KB);
			/*set terminal_number*/
			terminal_number = 0;
			/*set cursor*/
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			/*if the new terminal has not ran a shell yet, execute a shell in it*/
			if(termis[terminal_number].run_shell == 0)
			{
				schedule_next_process(terminal_number);
			}
			return;
		}
		if(scan_code == F2_ON && alt_pressed == 1)
		{	
			cli();
			int old_terminal_number = terminal_number;
			if(terminal_number==1) /*Check to see if this is terminal 2*/
				return;
			/*remap_page paging*/
			remap_page(old_terminal_number, 1);
			/*copy data from video memory into current terminal's buffer*/
			memcpy( (void*)(USER_VIDEO_ADDR+(old_terminal_number+1)*FOUR_KB) , (void*)VIDEO, (uint32_t)FOUR_KB);
			/*copy data from next terminal's buffer into video memory*/
			memcpy((void*)VIDEO , (void*)(USER_VIDEO_ADDR+TERMINAL_TWO*FOUR_KB) , (uint32_t)FOUR_KB);
			/*set terminal_number*/
			terminal_number = 1;
			/*set cursor*/
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			/*if the new terminal has not ran a shell yet, execute a shell in it*/
			if(termis[terminal_number].run_shell == 0)
			{
				schedule_next_process(terminal_number);
			}
			return;
		}
		if(scan_code == F3_ON && alt_pressed == 1)
		{
			cli();
			int old_terminal_number = terminal_number;
			if(terminal_number==2) /*Check to see if this is terminal 3*/
				return;
			/*remap_page paging*/
			remap_page(old_terminal_number, 2);
			/*copy data from video memory into current terminal's buffer*/
			memcpy( (void*)(USER_VIDEO_ADDR+(old_terminal_number+1)*FOUR_KB) , (void*)VIDEO, (uint32_t)FOUR_KB);
			/*copy data from next terminal's buffer into video memory*/
			memcpy((void*)VIDEO , (void*)(USER_VIDEO_ADDR+TERMINAL_THREE*FOUR_KB) , (uint32_t)FOUR_KB);
			/*set terminal_number*/
			terminal_number = 2;
			/*set cursor*/
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			/*if the new terminal has not ran a shell yet, execute a shell in it*/
			if(termis[terminal_number].run_shell == 0)
			{

				schedule_next_process(terminal_number);
			}
			return;
		}


		/*set map and move cursor according to flags*/

		if(termis[terminal_number].up)
		{   
			return;
		}
		if(termis[terminal_number].down)
		{
			return;
		}
		/*if left is being pressed*/
		if(termis[terminal_number].left)
		{
			/*find left position of cursor and draw it*/
			keyboard_cursor_revert(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);/*revert the position of cursor to previous status*/
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			return;
		}
		/*if right is being pressed*/
		if(termis[terminal_number].right)
		{
			if((termis[terminal_number].cursor_y * WIDTH + termis[terminal_number].cursor_x) < (old_cursor_y * WIDTH + old_cursor_x))
			{
				/*find right position of cursor and draw it*/
				keyboard_cursor_update(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);/*update the position of cursor*/
				keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			}
			return;

		}


		if(termis[terminal_number].ctrl)
		{
			/*use the first lookup table*/
			termis[terminal_number].map = 0;
		}
		else if(termis[terminal_number].shift)
		{
			if(!termis[terminal_number].caps_lock)
			{
				if((scan_code >= SCAN_CODE_Q && scan_code <= SCAN_CODE_P) || 
				(scan_code >= SCAN_CODE_A && scan_code <= SCAN_CODE_L) || 
				(scan_code >= SCAN_CODE_Z && scan_code <= SCAN_CODE_M))
				{
					/*choose the third table to get captal letter*/
					termis[terminal_number].map = 2;
				}
				else
				{
					/*choose the second table symbol*/
					termis[terminal_number].map = 1;
				}
			}
			else
			{
				if((scan_code >= SCAN_CODE_Q && SCAN_CODE_P) || 
				(scan_code >= SCAN_CODE_A && scan_code <= SCAN_CODE_L) || 
				(scan_code >= SCAN_CODE_Z && scan_code <= SCAN_CODE_M))
				{
					/*choose the first table to get non-captal letter*/
					termis[terminal_number].map = 0;
				}
				else
				{
					/*choose the second table to get symbol*/
					termis[terminal_number].map = 1;
				}
			}
		}
		else if(termis[terminal_number].caps_lock)
		{
			if((scan_code >= SCAN_CODE_Q && scan_code <= SCAN_CODE_P) || 
				(scan_code >= SCAN_CODE_A && scan_code <= SCAN_CODE_L) || 
				(scan_code >= SCAN_CODE_Z && scan_code <= SCAN_CODE_M))
			{
				/*choose the third table to get captal letter*/
				termis[terminal_number].map = 2;
			}
			else
			{
				/*choose the first table to get number*/
				termis[terminal_number].map = 0;
			}
		}
		else
		{
			termis[terminal_number].map = 0;/*use the first table*/
		}
		/*if the scan code indicats a character, read the character*/
		character = keyboard_array[termis[terminal_number].map][scan_code];	
		keyboard_fill_buffer(character);  
	}
	sti();
	return;
}

/*
 * init_termis: set the relevant flags in each terminal to proper value
 * INPUT: terminal_number , indicate which terminal to be initialized
 * OUTPUT: none
 * return val: none
 * side effect: none
 */
void init_termis(uint8_t terminal_number)
{
	int i;
	/////////////////////////// set up the keyboard flags 
	termis[terminal_number].ctrl = 0;/*flag of ctrl*/
	termis[terminal_number].caps_lock = 0;/*flag of capslock*/
	termis[terminal_number].shift = 0;/*flag of shift*/
	termis[terminal_number].caps_on = 0;/*flag of pressed capslock*/
	termis[terminal_number].caps_off = 1;/*flago of released capslock*/
	termis[terminal_number].video_memory = USER_VIDEO_ADDR;
	termis[terminal_number].map = 0;/*indicate which keyboard table is being used*/
	termis[terminal_number].up = 0;/*flag of up key*/
	termis[terminal_number].down = 0;/*flag of down key*/
	termis[terminal_number].left = 0;/*flag of left key*/
	termis[terminal_number].right = 0;/*flag of right key*/
	termis[terminal_number].scroll_index = 0;
	//////////////////////////////////////////////////////
	termis[terminal_number].run_shell = 0; /*Indicate whether there is at least one shell running*/
	termis[terminal_number].terminal_id = terminal_number;	/*The terminal numbers*/
	termis[terminal_number].buffer_flag = 1;	/*Buffer empty flags*/
	termis[terminal_number].enter_flag = 0;/*flag of enter_flag key*/
	termis[terminal_number].cursor_x = 0;/*x position of cursor*/
	termis[terminal_number].cursor_y = 0;/*y position of cursor*/
	termis[terminal_number].buffer_size = 0;/*current buffer size*/	
	
	for(i = 0 ; i < MAX_SIZE ; i ++) /*Clean the corresponding buffer*/
	{
		termis[terminal_number].buffer[i] = '\0';
		termis[terminal_number].storage_buffer[i] = '\0';
	}
}

/*
*open_termial: open a new terminal
*INPUT:none
*OUTPUT:none
*return value: 0
*side effect: none
*/
int32_t open_terminal()
{
	return 0;
}

/*
 *read_termial: read one line from terminal
 *INPUT:buf -- where to store the data , nbytes --- how many character should be read
 *OUTPUT:nbytes of data
 *return value: 0
 *side effect: none
 */
int32_t read_terminal(int fd,void* buf , int32_t nbytes)
{
	/*get the index of process that is running*/
	int i = 0;
	int j = 0;
	int len=0;
	int32_t terminal_number= get_current_process();
	char *buf_new=(char *)buf;
	int length = 0;
	
	/*determine the size of bytes to read*/
	if(nbytes <= MAX_BUFFER_SIZE /* 4KB buffer size*/){
		length = nbytes;
	}
	else{
		length = MAX_BUFFER_SIZE;/* 4KB buffer size */
	}

	/*clear the enter_flag flag before reading*/
	termis[terminal_number].enter_flag=0;
	/*if enter_flag is not pressed*/
	while(termis[terminal_number].enter_flag != 1)
	{
		for(i = 0 ; i < length ; i ++){
			buf_new[i] = '\0';/*NULL*/
		}
		for(j = 0 ; j < length ; j ++)
		{
			buf_new[j] = termis[terminal_number].storage_buffer[j];
		}
	}
	/*if the enter_flag is pressed, add a \0 at the end of buffer*/
	buf_new[MAX_BUFFER_SIZE-1]='\0';
	while((buf_new[len] != '\0'))
		len++;
	/*caculate the length of buffer*/
	/*the line returned should include the line feed character*/
	buf_new[len] = '\n';
	buf_new[MAX_BUFFER_SIZE-1] = '\n';
	termis[terminal_number].enter_flag = 0;
	/*clean the storage buffer*/
	for(j = 0 ; j < MAX_SIZE ; j ++)
	{
		termis[terminal_number].storage_buffer[j] = '\0';/*NULL*/
	}
	return len + 1;
}

/*
 *write_terminal: given the address of a buffer, print the content of buffer on the screen , within nbytes length
 *INPUT:buf -- source , nbytes --- how many character should be printed
 *OUTPUT:none
 *return value: 0
 *side effect: none
 */
int32_t write_terminal(const void *buf , int32_t nbytes)
{
	/*get the current terminal_number and current process number*/
	int32_t terminal_number=get_terminal_number();
	uint32_t current_process=get_current_process();
	char *buf_new = (char*)buf;
	int i , j;
	for(i = 0 ; i < nbytes ; i ++)
	{
		/*get the content of input buf one by one*/
		char character = buf_new[i];

		if(character == '\b')/*backspace*/
		{
			/*if the cursor is not currently at the start of console*/
			if((!(termis[current_process].cursor_x == 0 && termis[current_process].cursor_y == 0)) && (termis[current_process].buffer_flag == 0))
			{
				/*get the last position of cursor and draw it*/
				terminal_revert_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);					/*update position of cursor*/
				if(current_process==terminal_number)
					terminal_set_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);					/*print cursor at new position*/
				terminal_console_putc(termis[current_process].cursor_x , termis[current_process].cursor_y , '\0');		/*print out the character in buffer*/
				continue;
		
			}
			else
			{
				continue;

			}
		}

		else if(character == '\n')/*enter_flag*/
		{
			/*if enter_flag is pressed, start a new line*/
			termis[current_process].cursor_x = WIDTH - 1;
			terminal_update_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);
			if(current_process==terminal_number)
					terminal_set_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);	
			continue;
			
		}

		else if(character == '\t')/*TAB*/
		{
			/*if TAB is pressed, fill next 4 position with blank*/
			for(j = 0 ; j < TAB_SPACE_NUM ; j ++)
			{
				terminal_console_putc(termis[current_process].cursor_x , termis[current_process].cursor_y , ' ');
				terminal_update_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);
				if(current_process==terminal_number)
					terminal_set_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);			/*print cursor at new position*/
			}
			continue;
		
		}
		else
		{
			/*other cases, just update the position of cursor*/
			terminal_console_putc(termis[current_process].cursor_x , termis[current_process].cursor_y , character);
			terminal_update_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);
			if(current_process==terminal_number)
					terminal_set_cursor(termis[current_process].cursor_x , termis[current_process].cursor_y);			/*print cursor at new position*/
		}

	}
	return 0;
}


/*
 *open_termial: close the terminal
 *INPUT:none
 *OUTPUT:none
 *return value: 0
 *side effect: none
 */
int32_t close_terminal()
{	
	/*get the index of process that is running*/
	int32_t process_num=get_current_process();
	int j;
	for(j = 0 ; j < MAX_SIZE ; j ++)
	{
		termis[process_num].storage_buffer[j] = '\0';/*NULL*/
	}
	/*clean the buffer*/
	terminal_clean_buffer();
	return 0;
}

/*
*terminal_console_putc: write character into (cursor_x , cursor_y)
*INPUT: x,y --- indicate where to print the character , ch --- character which should be printed
*OUTPUT: character on the console
*side effect: none
*/
void terminal_console_putc(uint16_t x , uint16_t y , char ch)
{
	/*data of a character consists of 2 bytes: | Higher Byte | Lower Byte |
	  Higer Byte contains the ASCII code , Lower Byte contains the color
	  Higer Byte has lower address , Lower Byte has higher address*/
	uint32_t current_process=get_current_process();
	uint16_t cur_offset = (y * WIDTH + x) * DOUBLE;					/*get the offset of (x , y) in video memory*/

	char* high_byte_addr = (char*)(cur_offset + termis[current_process].video_memory);		/*get the address of higher byte in video memory*/
	*high_byte_addr = ch;											/*fill in the character*/

	char* low_byte_addr = high_byte_addr + 1;	/*fill in the color*/					/*get the address of lower byte in video memory*/
	
	*low_byte_addr = WHITE; 
	
	return;
}

/*
*terminal_console_getc: get a character which is located at (x , y)
*INPUT: x,y --- indicate where to get the character
*OUTPUT: character from the console
*side effect: none
*/
char terminal_console_getc(uint16_t x , uint16_t y)
{
	char* addr = (char*)(USER_VIDEO_ADDR + (y * WIDTH + x) * DOUBLE);  /*Get the characatet from the video memory from specific location*/
	char ch = *addr;	/*Get the character*/
	return ch;
}

/*
*scroll up: scroll the screen up with n rows
*INPUT:n -- how many lines to be scrolled up
*OUTPUT:none
*side effect:none 
*/
void terminal_scroll_up(uint16_t n)
{
	uint32_t current_process=get_current_process();
	uint16_t x, y;
	termis[current_process].scroll_index+=n; 				     /*Update the scroll up index*/
    for (y = 0 ; y < HEIGHT - n ; y ++)
    {
    	for (x = 0 ; x < WIDTH ; x ++)
     	{
     		terminal_console_putc(x , y , terminal_console_getc(x , y + n)); /*Save some characters*/
     	}
    }
    for(y = HEIGHT - n ; y < HEIGHT ; y ++)				/*Other places are filled with zero*/
    {
    	for (x = 0 ; x < WIDTH ; x ++)
     	{
     		terminal_console_putc(x , y , '\0');
     	}
    }
    return;
}

/*
*terminal_set_cursor: set the cursor to position (x , y)
*INPUT: x,y --- position of cursor
*OUTPUT: cursor appears on the console
*side effect: none
*/
void terminal_set_cursor(uint16_t x , uint16_t y)
{
	uint16_t position = y * WIDTH + x;  
	/*#14 register store the higher bits of cursor's position*/
    outb(HIGH_BITS_REG , REG_PORT);  
    outb(((position >> CURSOR_SHIFT_BITS/*right shift 8 bits to get higer 8 bits*/) & BIT_MASK_8) , DATA_PORT);  
    /*#15 register store the lower bits of cursor's position*/
    outb(LOW_BITS_REG , REG_PORT);  
    outb((position & BIT_MASK_8) , DATA_PORT);
    return;
}


/*
*terminal_update_cursor: update cursor's position after print a character on console
*INPUT: x,y --- current position of cursor
*OUTPUT: new position of cursor
*side effect: none
*/
void terminal_update_cursor(uint16_t x , uint16_t y)
{
	uint32_t current_process=get_current_process();
	/*if the cursor is currently at the right down corner of console*/
	if((x == WIDTH - 1) && (y == HEIGHT - 1))
	{
		terminal_scroll_up(1);
		termis[current_process].cursor_x = 0;
		termis[current_process].cursor_y = HEIGHT - 1;
		return;
	}
	/*if the cursor is currently at the end of one line*/
	else if(x == WIDTH - 1)
	{
		termis[current_process].cursor_x = 0;
		termis[current_process].cursor_y ++;
		return;
	}
	/*other cases*/
	else
	{
		termis[current_process].cursor_x ++;
		return;
	}
}


/*
*terminal_revert_cursor:get the previous position of cursor
*INPUT:x,y --- current position of cursor
*OUTPUT:previous position of cursor
*side effect:none
*/
void terminal_revert_cursor(uint16_t x , uint16_t y)
{
	uint32_t current_process=get_current_process();
	/*if the cursor is currently at the beginning of console*/
	if(x == 0 && y == 0)
	{
		return;
	}
	/*if the cursor is currently at the start of one line*/
	else if(x == 0)
	{
		termis[current_process].cursor_x = WIDTH - 1;
		termis[current_process].cursor_y --;
		return;
	}
	/*other cases*/
	else
	{
		termis[current_process].cursor_x --;
		return;
	}
}


/*
 *terminal_clean_buffer: clean the buffer
 *INPUT:none
 *OUTPUT:buffer cleaned
 *side effect:none
*/
void terminal_clean_buffer()
{
	int i; /*For counting purpose*/
	uint32_t current_process=get_current_process();
	/*clean the buffer*/
	for(i = 0 ; i < MAX_BUFFER_SIZE ; i ++)
	{
		termis[current_process].buffer[i] = '\0';/*NULL*/
	}
	/*reset the buffer size and flag*/
	termis[current_process].buffer_size = 0;
	termis[current_process].buffer_flag = 1;
	return;
}


/*
 * scroll up: scroll the screen up with n rows
 * INPUT:n -- how many lines to be scrolled up
 * OUTPUT:none
 * side effect:none 
 */
 void keyboard_scroll_up(uint16_t n)
{
	uint16_t x, y;
	termis[terminal_number].scroll_index+=n; 						/*Update the scroll up index*/
    for (y = 0 ; y < HEIGHT - n ; y ++)
    {
    	for (x = 0 ; x < WIDTH ; x ++)
     	{
     		keyboard_console_putc(x , y , keyboard_console_getc(x , y + n));  /*Save some characters*/
     	}
    }
    for(y = HEIGHT - n ; y < HEIGHT ; y ++)
    {
    	for (x = 0 ; x < WIDTH ; x ++)
     	{
     		keyboard_console_putc(x , y , '\0');						/*Other places are filled with zero*/
     	}
    }
    return;
}

/*
 * keyboard_set_cursor: set the cursor to position (x , y)
 * INPUT: x,y --- position of cursor
 * OUTPUT: cursor appears on the console
 * side effect: none
 */
 void keyboard_set_cursor(uint16_t x , uint16_t y)
{
	uint16_t position = y * WIDTH + x;  
	/*#14 register store the higher bits of cursor's position*/
    outb(HIGH_BITS_REG , REG_PORT);  
    outb(((position >> CURSOR_SHIFT_BITS/*right shift 8 bits to get higer 8 bits*/) & BIT_MASK_8) , DATA_PORT);  
    /*#15 register store the lower bits of cursor's position*/
    outb(LOW_BITS_REG , REG_PORT);  
    outb((position & BIT_MASK_8) , DATA_PORT);
    return;
}

/*
 * clean_screen: clean the screen
 * INPUT: none
 * OUTPUT: screen cleaned
 * side effect: none
 */
void clean_screen()
{
	int x , y;
	for(y = 0 ; y < HEIGHT ; y ++)						/*Put zero in all the video memory*/
	{
		for(x = 0 ; x < WIDTH ; x ++)
		{
			keyboard_console_putc(x , y , ' ');
		}
	}
	termis[terminal_number].cursor_x = 0;					/*Update the cursor*/
	termis[terminal_number].cursor_y = 0;
	keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
	return;
}

/*
 * keyboard_fill_buffer: fill the buffer with new character
 * INPUT: ch --- character need to be filled
 * OUTPUT: none
 * side effect: none
 */
 void keyboard_fill_buffer(char ch)
{
	/*no matter the buffer is full or not, is the new key pressed is backspace, delete a character*/
	if(ch == '\b')/*backspace*/
	{
		if(termis[terminal_number].buffer_size == 0)   /*When the buffer size is zero*/
		{
			keyboard_copy_buffer();
			termis[terminal_number].buffer_flag = 1;
			return;
		}
		else
		{										/*When the buffer size is not zero, detele one character*/
			termis[terminal_number].buffer[termis[terminal_number].buffer_size-1] = '\0';
			termis[terminal_number].buffer_size --;
			termis[terminal_number].buffer_flag = 0;
			write_terminal_from_keyboard(&ch , 1);
			keyboard_copy_buffer();
			return;
		}
		
	}

	/*if enter_flag is pressed*/
	if(ch == '\n')/*enter_flag*/
	{
		keyboard_copy_buffer();							/*Clean the buffer*/
		keyboard_buffer_clean();
		write_terminal_from_keyboard(&ch , 1);
		termis[terminal_number].enter_flag = 1;			/*Set the enter_flag flag to 1*/
		return;
	}

	/*if ctrl is pressed*/
	if(termis[terminal_number].ctrl == 1)
	{
		if(ch == 'l')
		
{			keyboard_buffer_clean();							/*Clean the buffer and the screen*/
			clean_screen();			
			keyboard_copy_buffer();
			return;
		}
	}

	/*if TAB is pressed*/
	if(ch == '\t')/*TAB*/
	{
		termis[terminal_number].buffer_flag = 0;
		int length , i;
		/*Calculate the offset*/
		if(MAX_BUFFER_SIZE - termis[terminal_number].buffer_size < TAB_SPACE_NUM)
		{
			length = MAX_BUFFER_SIZE - termis[terminal_number].buffer_size;
		}
		else
		{
			length = TAB_SPACE_NUM;
		}
		for(i = 0 ; i < length ; i ++)			/*Update the new loction with spaces*/
		{
			termis[terminal_number].buffer[termis[terminal_number].buffer_size] = ' ';
			termis[terminal_number].buffer_size ++;
			keyboard_copy_buffer();
		}
		return;

	}

	/*if the new key is not backspace*/
	/*if the buffer is not full*/
	if(termis[terminal_number].buffer_size < MAX_BUFFER_SIZE)
	{
		/*fill the buffer with new character*/
		termis[terminal_number].buffer[termis[terminal_number].buffer_size] = ch;
		termis[terminal_number].buffer_size ++;
		termis[terminal_number].buffer_flag = 0;
		write_terminal_from_keyboard(&ch , 1);
		keyboard_copy_buffer();
		return;
		/*if ctrl is being pressed*/	
	}

	/*if the buffer is already full*/
	else
	{
		/*do not fill the buffer anymore*/
		return;
	}
}

/*
 * keyboard_buffer_clean: clean the buffer
 * INPUT:none
 * OUTPUT:buffer cleaned
 * side effect:none
 */
 void keyboard_buffer_clean()
{
	int i;
	for(i = 0 ; i < MAX_BUFFER_SIZE ; i ++) /*Clean the buffer*/
	{
		termis[terminal_number].buffer[i] = '\0';/*NULL*/
	}
	termis[terminal_number].buffer_size = 0;  		/*Clean the corresponding flags*/
	termis[terminal_number].buffer_flag = 1;
	return;
}

/*
 * keyboard_copy_buffer: copy data from buffer into storage_buffer
 * INPUT:none
 * OUTPUT:none
 * side effect:none
 */
 void keyboard_copy_buffer()
{
	int i;
	for(i = 0 ; i < MAX_BUFFER_SIZE  ; i ++) /*Copy the content of the buffer to the storage buffer*/
	{
		termis[terminal_number].storage_buffer[i] = termis[terminal_number].buffer[i];
	}
	return;
}

/*
 * keyboard_cursor_update: update cursor's position after print a character on console
 * INPUT: x,y --- current position of cursor
 * OUTPUT: new position of cursor
 * side effect: none
 */
 void keyboard_cursor_update(uint16_t x , uint16_t y)
{
	/*if the cursor is currently at the right down corner of console*/
	if((x == WIDTH - 1) && (y == HEIGHT - 1))
	{
		keyboard_scroll_up(1);
		termis[terminal_number].cursor_x = 0;
		termis[terminal_number].cursor_y = HEIGHT - 1;
		return;
	}
	/*if the cursor is currently at the end of one line*/
	else if(x == WIDTH - 1)
	{
		termis[terminal_number].cursor_x = 0;
		termis[terminal_number].cursor_y ++;
		return;
	}
	/*other cases*/
	else
	{
		termis[terminal_number].cursor_x ++;
		return;
	}
}


/*
 * keyboard_cursor_revert:get the previous position of cursor
 * INPUT:x,y --- current position of cursor
 * OUTPUT:previous position of cursor
 * side effect:none
 */
 void keyboard_cursor_revert(uint16_t x , uint16_t y)
{
	/*if the cursor is currently at the beginning of console*/
	if(x == 0 && y == 0)
	{
		return;
	}
	/*if the cursor is currently at the start of one line*/
	else if(x == 0)
	{
		termis[terminal_number].cursor_x = WIDTH - 1;
		termis[terminal_number].cursor_y --;
		return;
	}
	/*other cases*/
	else
	{
		termis[terminal_number].cursor_x --;
		return;
	}
}


/*
 * write_terminal_from_keyboard: given the address of a buffer, print the content of buffer on the screen , within nbytes length
 * INPUT:buf -- source , nbytes --- how many character should be printed
 * OUTPUT:none
 * return value: 0
 * side effect: none
 */
int32_t write_terminal_from_keyboard(const void *buf , int32_t nbytes)
{
	char *buf_new = (char*)buf;
	int i , j;
	for(i = 0 ; i < nbytes ; i ++)
	{
		char character = buf_new[i];

		if(character == '\b')/*backspace*/
		{
			if((!(termis[terminal_number].cursor_x == 0 && termis[terminal_number].cursor_y == 0)) && (termis[terminal_number].buffer_flag == 0))
			{
				keyboard_cursor_revert(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);					/*update position of cursor*/
				keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);					/*print cursor at new position*/
				keyboard_console_putc(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y , '\0');		/*print out the character in buffer*/
				continue;
			}
			else
			{
				continue;

			}
		}

		else if(character == '\n')/*enter_flag*/
		{
			termis[terminal_number].cursor_x = WIDTH - 1; /*Move the cursor to the right and then update*/
			keyboard_cursor_update(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			continue;
			
		}

		else if(character == '\t')/*TAB*/
		{
			for(j = 0 ; j < TAB_SPACE_NUM ; j ++)
			{
				keyboard_console_putc(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y , ' ');	/*Move for the tab*/
				keyboard_cursor_update(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
				keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);			/*print cursor at new position*/
			}
			continue;
		
		}
		else/*other cases*/
		{

			keyboard_console_putc(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y , character); /*Cursor will follow the character*/
			keyboard_cursor_update(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);
			keyboard_set_cursor(termis[terminal_number].cursor_x , termis[terminal_number].cursor_y);			/*print cursor at new position*/
		}

	}
	return 0;
}

