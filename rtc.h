/*
 * rtc.h - support funciton for RTC 
 * Author:	    Guo
 * Version: 1
 *
 */

#ifndef RTC_H
#define RTC_H

#include "types.h"

/*All the magic numbers*/
#define RA 0x8A				// stand for register A,B,C 
#define RB 0x8B				///////
#define RC 0x0C

#define CMD_P 0x70			// stand for RTC CMD PORT 
#define DATA_P 0x71 

#define PIE_BIT       0x40
#define NOT_PIE_BIT       0xBF
#define RTC_IRQ_NUMBER  	8 
#define RTC_PIC		  2 
 
#define two     		0x0F
#define four     		0x0E
#define eight    	    0x0D
#define sixteen    		0x0C
#define thirtytwo   	0x0B
#define sixtyfour    	0x0A
#define onetwentyeight  0x09
#define twofiftysix   	0x08
#define fivetwelve      0x07
#define tentwentyfour   0x06


/*initialize the RTC to 2Hz*/
extern void init_rtc();
/*open rtc and set to the default frequency*/
extern int32_t open_rtc();
/*Wait for the rtc ticks*/
extern int32_t read_rtc(int fd,void* buf, int32_t nbytes);
/*Write new frequency to the rtc*/
extern int32_t write_rtc(const void * buf, int32_t nbytes);
/*Close the rtc*/
extern int32_t close_rtc();

#endif /* RTC_H */

