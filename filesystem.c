/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include <stdlib.h>
#include <stdio.h>

#include "include/filesystem.h"		// Headers for the core functionality
#include "include/auxiliary.h"		// Headers for auxiliary functions
#include "include/metadata.h"		// Type and structure declaration of the file system
#include "include/crc.h"			// Headers for the CRC functionality

superblock sblock;

char* i_map;
char* b_map;
inode* inodes;
inode_x* inodes_x;

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize){

	int fd = open(DEVICE_IMAGE, O_RDONLY);
	int size = lseek(fd, 0L, SEEK_END);
	if(deviceSize > size) return -1;//REVIEW

	sblock.magicNum = 1234;//REVIEW: check values
	sblock.numinodes = 50;//REVIEW: check values
	sblock.deviceSize = deviceSize;

	i_map = (char*)calloc(sblock.numinodes, sizeof(char));
	b_map = (char*)calloc(sblock.dataBlockNum, sizeof(char));
	inodes = (inode*)calloc(sblock.numinodes, sizeof(inode));
	inodes_x = (inode_x*)calloc(sblock.numinodes, sizeof(inode_x));

	unmountFS();

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void){
	bread(DEVICE_IMAGE, 1, (char*)&sblock);

	for(int i=0; i<sblock.inodeMapNumBlocks; i++)
		bread(DEVICE_IMAGE, 2+i, (char*)i_map + i*BLOCK_SIZE);

	for(int i=0; i<sblock.dataMapNumBlock; i++)
		bread(DEVICE_IMAGE, 2+i+sblock.inodeMapNumBlocks, (char*)b_map + i*BLOCK_SIZE);

	for(int i=0; i<(sblock.numinodes*sizeof(inode)/BLOCK_SIZE); i++)
		bread(DEVICE_IMAGE, i+sblock.firstInode, (char*)inodes + i*BLOCK_SIZE);

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void){

	for(int i=0; i<sblock.numinodes; i++){
		if(inodes_x[i].opened == 1)
			return -1;
	}

	//sync();REVIEW

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName){
	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, int whence, long offset)
{
	return -1;
}

/*
 * @brief 	Verifies the integrity of the file system metadata.
 * @return 	0 if the file system is correct, -1 if the file system is corrupted, -2 in case of error.
 */
int checkFS(void)
{
	return -2;
}

/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName)
{
	return -2;
}
