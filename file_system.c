/*
 * filesystem.c - support functions for filesystem
 * Author:	    gyms  
 * Version: 1
 *
 */
#include "file_system.h"
#include "lib.h"
#include "pcb.h"


uint32_t bb_start;
/* total_entries_num includes the . entry */
#define total_entries_num *(uint8_t *)bb_start
/* non . entries start from second entry */
/* static int current_entry_num = 0;*/

/*
* int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry);
*   Inputs: const uint8_t * fname = filename to read
*			dentry_t * dentry = allocated dentry buffer to place copied dentry in
*   Return Value: -1 if fail; 0 if success.
*	Function: copy dentry of given fname to dentry
*/
int32_t read_dentry_by_name(const uint8_t * fname, dentry_t * dentry)
{
	uint32_t entry_number = *(int *)bb_start;

	int i;
	uint32_t * entry_address;
	uint32_t * file_type;
	uint32_t * inode_number_address;

	for(i = 0; i < entry_number; i++){
		entry_address = (uint32_t *)(bb_start + (i + 1) * ENTRY_SIZE_64B);

		/*Compare the dentry name and the file name given*/
		if(strncmp((int8_t *)entry_address, (int8_t *)fname, MAX_FILE_NAME) == 0){
			strcpy((int8_t*)dentry->file_name, (int8_t*)fname); 
			file_type = (uint32_t *)((int)entry_address + FILE_NAME_32B);
			dentry->file_type = *file_type;

			/* RTC 0, directory 1, regular file 2 */
			if (*file_type == REGULAR_FILE){
				inode_number_address = (uint32_t *)((int)file_type + OFFSET_4B);
				dentry->inode_number = *inode_number_address;
			}
			else{
				/*Whent the file is not regular*/
				dentry->inode_number = 0;
			}
			return 0;
		}
	}
	return -1;
}

/*
* int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry);
*   Inputs: uint32_t index = index of dentry to be read
*			dentry_t * dentry = allocated dentry buffer to place copied dentry in
*   Return Value: -1 if fail; 0 if success.
*	Function: copy dentry of given index to dentry 
*/
int32_t read_dentry_by_index(uint32_t index, dentry_t * dentry)
{
	uint32_t entry_number = *(int *)bb_start;

	/*When the index number is invalid*/
	if(index < 0 || index >= entry_number){
		return -1;
	}
	
	uint32_t * entry_address = (uint32_t *)(bb_start + (index + 1)*ENTRY_SIZE_64B); /*Pointer to the entry*/
	int8_t * fname = (int8_t *)entry_address;
	uint32_t * file_type = (uint32_t *)((int)entry_address + FILE_NAME_32B);	   /*Pointer to the file type*/
	uint32_t * inode_number_address = (uint32_t *)((int)file_type + OFFSET_4B);	   /*Pointer to the inode number*/

	
	memcpy(dentry->file_name, fname, DIR_BUF_SIZE-1);                              /*Get the file name*/
	dentry->file_type = *file_type;										   /*Get the file type*/

	/* RTC 0, directory 1, regular file 2 */
	if (*file_type == REGULAR_FILE){											/*Only regular file has the file type*/
		inode_number_address = (uint32_t *)((int)file_type + OFFSET_4B);
		dentry->inode_number = *inode_number_address;
	}
	else{
		dentry->inode_number = 0;
	}	
	return 0;
}


/*
* 	int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length)
*   Inputs: index of inodes, offset of start point of reading, buf to store read data, length of bytes to be read
*   Return Value: Bytes read, -1 if fail;
*	Function: copy file data into buf
*/
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t * buf, uint32_t length)
{
	uint32_t num_of_inodes = *(int *)(bb_start + OFFSET_4B);
	uint32_t num_of_data_blocks = *(int *)(bb_start + 2*OFFSET_4B);/*2:# of data blocks is the third 4B block in the struct*/
	/* check if input inode # is valid */
	if (inode < 0 || inode > (num_of_inodes - 1)){
		printf(" incorrect inode  %d \n", inode);
		return -1;
	}
		

	uint32_t inode_addr = bb_start + (inode + 1) * BLOCK_4KB;
	uint32_t file_len_in_B = * (uint32_t *)inode_addr;
		
	/*check if offset is already greater than file size */
	if(offset >= file_len_in_B)
		return 0;	/*careful! not -1*/

	/* if length is larger than what is left to copy, just copy to the end of the file*/
	if(length + offset > file_len_in_B){
		length = file_len_in_B - offset;
	}
		
	
	uint32_t quotient = (offset/BLOCK_4KB);    /*Calculate how many data blocks are there*/
	uint32_t remain = offset % BLOCK_4KB;	   /*The remaning offset*/

	
	uint32_t * data_block_ptr = (uint32_t *)(inode_addr + OFFSET_4B+OFFSET_4B*quotient);
	uint32_t data_block_num = *data_block_ptr;
		
	/* check bad data block number */
	if (data_block_num < 0 || data_block_num >= num_of_data_blocks){
		printf("bad data block number %d found!\n", data_block_num);
		return -1;
	}
	uint32_t data_block_addr = bb_start + BLOCK_4KB + num_of_inodes * BLOCK_4KB + data_block_num * BLOCK_4KB;
	uint32_t data_addr = data_block_addr + remain;   /*Get the data block address*/

	uint32_t i = remain;
	uint32_t j;
	for(j = 0; j < length; j++)
	{
		buf[j] = *(uint8_t *)data_addr;
		data_addr++;
		i++;

		if(i > BLOCK_4KB)
		{										   /*Update the new data block number*/
			data_block_ptr++;
			data_block_num = *(data_block_ptr);
			data_block_addr = bb_start + BLOCK_4KB + num_of_inodes * BLOCK_4KB + data_block_num * BLOCK_4KB;
			data_addr = data_block_addr;
			i = 0;
		}
	}

	return length;

}


/*
* 	int32_t open_file();
*   Inputs: None
*   Return Value: returns zero
*	Function: simply returns zero. 
*/
int32_t open_file(){
	return 0;
}

/*
* 	int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
*   Inputs: int32_t fd = file descriptor index 
*			void *buf = buffer passed in, used to stored data copied
* 			int32_t nbytes = number of bytes to copy
*   Return Value: number of bytes read
*	Function: read the file corresponding to the fd passed in, read to the end of file or end of buffer 
*/
/*assume that file type is 2*/
int32_t read_file(int32_t fd, void* buf, int32_t nbytes){
	
	file_t* fd_ptr=(file_t*)fd;						/*Get the file discriptor*/

	int32_t num_read=0;
	int32_t inode_number = (fd_ptr->inode_ptr - bb_start)/BLOCK_4KB - 1; /*Get the inode number*/
	num_read = read_data (inode_number, fd_ptr->file_pos, buf, nbytes); 		/*Read the data into the buffer given*/

	if(num_read > 0)								/*Update the file position*/
	{
		fd_ptr->file_pos += num_read;
	}

	return num_read;

}

/*
* 	int32_t write_file(const void* buf, int32_t nbytes);
*   Inputs: const void* buf = buffer passed in, stores data to write
* 			int32_t nbytes = number of bytes to write
*   Return Value: returns -1
*	Function: files in the file system is read only, therefore this function return -1.
*/
int32_t write_file(const void* buf, int32_t nbytes){
	return -1;				/*We are not supposed to write file*/
}

/*
* 	int32_t close_file();
*   Inputs: None
*   Return Value: returns zero
*	Function: simply returns zero. 
*/
int32_t close_file(){
	return 0;
}


/*
* 	int32_t directory_open();
*   Inputs: None
*   Return Value: returns zero
*	Function: simply returns zero. 
*/
int32_t directory_open(){
	return 0;
}

/*
* 	int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);
*   Inputs: int32_t fd = file descriptor index 
*			void *buf = buffer passed in, used to stored data copied
* 			int32_t nbytes = number of bytes to copy
*   Return Value: length of the file name or 0 once reached the end
*	Function: reads out the files in the directory 
*/
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes){

	file_t* fd_ptr=(file_t*)fd;				/*Get the file descriptor*/
	int current_entry_num = fd_ptr->file_pos; /*Get the current file position*/

	int32_t ret_val=0;
	buf = (int8_t *)buf;
	dentry_t dentry;

	if(current_entry_num == total_entries_num){	/*When there is nothing to read*/
		return 0;
	}

	if(current_entry_num < total_entries_num){
		read_dentry_by_index(current_entry_num, &dentry);	/*Read the dentry by index provided by the file position*/	
		memcpy(buf, dentry.file_name, DIR_BUF_SIZE-1);
		((int8_t *)buf)[DIR_BUF_SIZE-1]='\0';
		fd_ptr->file_pos++;									/*Update the file position and get the return value*/
		ret_val=strlen(buf);
	}

	return ret_val;
}


/*
* 	int32_t directory_write(const void* buf, int32_t nbytes);
*   Inputs: const void* buf = buffer passed in, stores data to write
* 			int32_t nbytes = number of bytes to write
*   Return Value: returns -1
*	Function: files in the file system is read only, therefore this function return -1.
*/
int32_t directory_write(const void* buf, int32_t nbytes){
	return -1;
}

/*
* 	int32_t directory_close();
*   Inputs: None
*   Return Value: returns zero
*	Function: simply returns zero. 
*/
int32_t directory_close(){
	return 0;
}


/*
* 	void loader(uint32_t* destination , const uint8_t * name);
*   Inputs: uint32_t* destination = the starting address of the place to copy the executable to
*    		const uint8_t * name = name of the file
*   Return Value: None
*	Function: copy the executable of file name 'name' to destination
*/
void loader(uint32_t* destination , const uint8_t * name)
{
	dentry_t new_dentry;
	int i=0; /*For counting purpose*/
	uint8_t copy_buf[BLOCK_4KB];
	/*read the dentry according to the file name*/
	read_dentry_by_name(name , &new_dentry);	
	/*caculate the index of the inode in file system*/
	uint32_t inode_index = (new_dentry.inode_number) + 1;
	/*size of data need to copy, in byte*/
	uint32_t file_size = *((uint32_t*)(bb_start + inode_index * BLOCK_4KB));	
	/*if more than 1 data block need to copy*/

	if(file_size > BLOCK_4KB)
	{
		int size = file_size;
		uint8_t* des = (uint8_t*)destination;
		while(size > BLOCK_4KB)
		{
			read_data (new_dentry.inode_number, i*BLOCK_4KB , copy_buf, BLOCK_4KB);
			/*copy 4KB date, ie, a data block*/
			memcpy(des , copy_buf , BLOCK_4KB);
			/*move on next block*/
			size -= BLOCK_4KB; 
			des += BLOCK_4KB;
			i++;
		}
		/*if the remaining data is less than 4KB, copy them then return*/
		read_data (new_dentry.inode_number, i*BLOCK_4KB , copy_buf, size);
		memcpy(des , copy_buf , size);
		return;
	}
	else
	{
		/*if the data is no more than 1 block, copy them in one time*/
		read_data (new_dentry.inode_number, i*BLOCK_4KB , copy_buf, file_size);
		memcpy(destination , copy_buf , file_size);
		return;
	}
}


