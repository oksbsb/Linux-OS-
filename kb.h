/*
 * kb.c - support funciton for the keyboard 
 * Author:	    gyms guo
 * Version: modified at 12/2/2016  15:50
 *
 */

#ifndef KB__H
#define KB__H 

#include "types.h"


/*The terminal struct*/
typedef struct terminal_struct
{
/////////////////// these members are working with keyboard 
	int32_t buffer_size;
	uint8_t ctrl;
	uint8_t caps_lock;
	uint8_t shift;
	uint8_t caps_on;
	uint8_t caps_off;
	uint32_t video_memory;
	uint8_t map;
	uint8_t up;
	uint8_t down;
	uint8_t left;
	uint8_t right;
	uint8_t scroll_index;
//////////////////////////// these members are for terminals 
	uint8_t run_shell;
	uint8_t terminal_id;
	int buffer_flag;
	uint8_t enter_flag;
	uint16_t cursor_x;
	uint16_t cursor_y;
	char buffer[MAX_SIZE];
	char storage_buffer[MAX_SIZE];

}termis_t; 

termis_t termis[TOTAL_TERMINALS]; /*There could be three terminals*/

/*Initialize the terminal according to the terminal number provided*/
extern void init_termis(uint8_t terminal_number);
/*Open the terminal*/
extern int32_t open_terminal();
/*Read the terminal*/
extern int32_t read_terminal(int fd,void* buf , int32_t nbytes);
/*Write the terminal*/
extern int32_t write_terminal(const void* buf , int32_t nbytes);
/*Close the terminal*/
extern int32_t close_terminal();

/*All the helper funtions. See headers for more information*/
extern  void terminal_console_putc(uint16_t x , uint16_t y , char ch);
extern char terminal_console_getc(uint16_t x , uint16_t y);
extern void terminal_scroll_up(uint16_t n);
extern void terminal_set_cursor(uint16_t x , uint16_t y);
extern void terminal_update_cursor(uint16_t cursor_x , uint16_t cursor_y);
extern void terminal_revert_cursor(uint16_t x , uint16_t y);
extern void terminal_clean_buffer();

/*Clean the screen*/
extern void clean_screen();
/*get current terminal_number*/
extern int32_t get_terminal_number();
/*Initialize the keyboard*/
extern void set_kb();
//////////////////// all helper functions for keyboard to set screen
extern void keyboard_fill_buffer(char ch);
extern void keyboard_cursor_update(uint16_t cursor_x , uint16_t cursor_y);
extern void keyboard_buffer_clean();
extern void keyboard_cursor_revert(uint16_t x , uint16_t y);
extern void keyboard_copy_buffer();
extern int32_t write_terminal_from_keyboard(const void *buf , int32_t nbytes);
extern void keyboard_console_putc(uint16_t x , uint16_t y , char ch);
extern char keyboard_console_getc(uint16_t x , uint16_t y);
extern void keyboard_scroll_up(uint16_t n);
extern void keyboard_set_cursor(uint16_t x , uint16_t y);

#endif /* KB__H */


