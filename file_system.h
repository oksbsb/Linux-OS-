/*
 * filesystem.h - header for the filesystem functions
 * Author:	    gyms  
 * Version: 1
 *
 */
#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "lib.h"


/*dentry struct definitions*/
typedef struct dentry_t
{
	int8_t file_name[FILE_NAME_LENGTH]; 
	int32_t file_type;
	int32_t inode_number;
	int8_t reserved[DENTRY_RESERVED_LEN]; 
}dentry_t;

/*assume we've got the starting address of the bootblock*/
extern uint32_t bb_start;

/*Help function for the filesystem*/
/*Read the dentry by name*/
extern int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
/*Read the dentry by index*/
extern int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);
/*Read the data based on inode*/
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length);

/*Open the file*/
extern int32_t open_file();
/*Write the file*/
extern int32_t write_file(const void* buf, int32_t nbytes);
/*Close the file*/
extern int32_t close_file();
/*Read the file*/
extern int32_t read_file(int32_t fd, void* buf, int32_t nbytes);

/*Open the directory*/
extern int32_t directory_open();
/*Write the directory*/
extern int32_t directory_write(const void* buf, int32_t nbytes);
/*Close the directory*/
extern int32_t directory_close();
/*Read the directory*/
extern int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);

/*Load the file with the name given to the memory address given*/
extern void loader(uint32_t* destination , const uint8_t * name);

#endif /*_FILESYSTEM_H*/


