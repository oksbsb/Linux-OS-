/*
 * rtc.c - support funciton for RTC 
 * Author:	    Guo
 * Version: 1
 *
 */

#include "irq.h"
#include "lib.h"
#include "rtc.h"
#include "scheduling.h"
static void rtc_handler();




volatile int rtc_interrupt_occured[3] = {0,0,0};

static int test_mode;

 /*   
  *	  set_test_mode
  *	  DESCRIPTION: set the rtc test mode
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */
void set_test_mode(){
	test_mode = 1;
}

 /*   
  *	  clear_test_mode
  *	  DESCRIPTION: clear the rtc test mode
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */
void clear_test_mode(){
	test_mode = 0;
}


/*   
  *	  set_rtc
  *	  DESCRIPTION: initialize the RTC to 2Hz, r3-r0 : 15  rate 
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */
void init_rtc(){
	uint8_t temp = 0;				// store the temp value from RTC register

	///// enable the periodic interrput, changing register B 
	cli();
	outb(RB, CMD_P);		// put RB value in CMD port 
	temp = inb(DATA_P);		// from data port getting temp value 
	outb(RB, CMD_P);		// put RB value in CMD port 
	outb(PIE_BIT|temp, DATA_P);  //// turn on the PIE bit 
	sti();

	/// set up frequnecy to 2 hz, by changing register A 
	outb(RA, CMD_P);
	unsigned char prev = inb(DATA_P);
	outb(RA, CMD_P);
	outb(thirtytwo|(prev & 0xf0), DATA_P);
	
	cli();
	//enable_irq(RTC_PIC);
	//enable_irq(RTC_IRQ_NUMBER);
	set_interrupt(RTC_IRQ_NUMBER ,rtc_handler);
	sti();
}


 /*   
  *	  open_rtc
  *	  DESCRIPTION: open the rtc and initialize the RTC to 2Hz
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */
int32_t open_rtc(const uint8_t* filename){
	init_rtc();
	return 0;
}


 /*   
  *	  read_rtc
  *	  DESCRIPTION: wait till a interrupt has occured
  *   INPUTS: buf: gargabe, nbytes: basically garbage
  *   OUTPUTS: none
  *   RETURN VALUE: always 0 
  *   SIDE EFFECTS: none
  */
int32_t read_rtc(int fd,void* buf, int32_t nbytes){

	// save eax in this function 
	uint32_t current_process = get_current_process();
	uint32_t save_eax;
	asm volatile(
		"movl %%eax, %0"
		:
		:"r" (save_eax)
		);
	
	// block the system when no interrupt has occured
	rtc_interrupt_occured[current_process] = 0;
	while(1){
		if(rtc_interrupt_occured[current_process] == 1)
			break;
	}

	// restore eax
	asm volatile(
		"movl %0, %%eax"
		:
		:"r" (save_eax)
		);

	return 0; 
}


/*   
  *	  write_rtc
  *	  DESCRIPTION: set the rtc frequency to specific value
  *   INPUTS: fd -- ignored
  *			  buf -- points to frequency
  *			  nbytes -- ignored
  *   OUTPUTS: none
  *   RETURN VALUE: 0 success; -1 failure
  *   SIDE EFFECTS: none
  */
int32_t write_rtc( const void* buf, int32_t nbytes){
	
	int rate;

	// validity check 
	if (nbytes != 4)
		return -1;

	// get frequency from buffer
	int frequency = *(uint32_t *)buf;

	switch(frequency){		// change rate depends on the input frequency
		case(2):
			rate = two;		// 2
			break;
		case(4):
			rate = four;	// 4
			break;
		case(8):
			rate = eight;	// 8
			break;
		case(16):
			rate = sixteen;	// 16
			break;
		case(32):
			rate = thirtytwo;// 32
			break;
		case(64):
			rate = sixtyfour;// 64
			break;
		case(128):
			rate = onetwentyeight;// 128
			break;
		case(256):
			rate = twofiftysix;// 256
			break;
		case(512):
			rate = fivetwelve;// 512
			break;
		case(1024):
			rate = tentwentyfour;// 1024
			break;
		default:
			return -1;
	}
	outb(RA, CMD_P);
	unsigned char prev = inb(DATA_P);
	outb(RA, CMD_P);
	outb(rate|(prev & 0xf0), DATA_P);			// adjust frequency 
	return 0;
}


 /*   
  *	  close_rtc
  *	  DESCRIPTION: Do nothing since this no virtualization of the RTC
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: always 0
  *   SIDE EFFECTS: none
  */
int32_t close_rtc(int32_t fd)
{
	// disable rtc
	// disable_irq(RTC_IRQ_NUMBER);
	return 0;
}


 /*   
  *	  do_rtc
  *	  DESCRIPTION: handling the rtc function
  *   INPUTS: none
  *   OUTPUTS: none
  *   RETURN VALUE: none
  *   SIDE EFFECTS: none
  */
void rtc_handler(){
	
	cli();

	outb(RC,CMD_P);   /*Have to read something for the rtc to work*/
	inb(DATA_P);
	//test_interrupts();	// used to run test_interrupts
	// if(test_mode)
	// 	putc('1');
	//send_eoi(RTC_IRQ_NUMBER);
	rtc_interrupt_occured[0] = 1;
	rtc_interrupt_occured[1] = 1;
	rtc_interrupt_occured[2] = 1;
	//update_vidmap();
	sti();
}

