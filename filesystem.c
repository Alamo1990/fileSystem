/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

	int fd = open(DEVICE_IMAGE, O_RDWR);
	int size = lseek(fd, 0L, SEEK_END);
	// printf("opened dev size: %d",  size/1024);
	if(size > 100*1024 || size < 50*1024) return -1;
	if(deviceSize > size) return -1;//REVIEW 

	sblock.magicNum = 714916;//REVIEW: check values
	sblock.inodeMapNumBlocks = 32;//REVIEW: check values
	sblock.dataMapNumBlock = 128;//REVIEW: check values 
	sblock.numinodes = 64;//REVIEW: check values 
	sblock.firstInode = 162;//REVIEW: check values
	sblock.dataBlockNum = 128;//REVIEW: check values
	sblock.firstDataBlock = 226;//REVIEW: check values
	sblock.deviceSize = deviceSize;

	i_map = (char*)calloc(sblock.numinodes, sizeof(char));
	b_map = (char*)calloc(sblock.dataBlockNum, sizeof(char));
	inodes = (inode*)calloc(sblock.numinodes, sizeof(inode));
	inodes_x = (inode_x*)calloc(sblock.numinodes, sizeof(inode_x));

	unsigned char buffer[BLOCK_SIZE];
	bread(DEVICE_IMAGE, 0, (char*)&buffer);

	sblock.crc = CRC64((const unsigned char*)buffer, BLOCK_SIZE);

	unmountFS();

	return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void){
	if (bread(DEVICE_IMAGE, 1, (char*)&sblock) == -1) return -1;

	int i;
	for(i=0; i<sblock.inodeMapNumBlocks; i++)
		bread(DEVICE_IMAGE, 2+i, (char*)i_map + i*BLOCK_SIZE);

	for(i=0; i<sblock.dataMapNumBlock; i++)
		bread(DEVICE_IMAGE, 2+i+sblock.inodeMapNumBlocks, (char*)b_map + i*BLOCK_SIZE);

	for(i=0; i<(sblock.numinodes*sizeof(inode)/BLOCK_SIZE); i++)
		bread(DEVICE_IMAGE, i+sblock.firstInode, (char*)inodes + i*BLOCK_SIZE);

	if(checkFS()<0) return -1;//F5: the file system must be checked at least on mount

	return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void){

	unsigned char buffer[BLOCK_SIZE];
	bread(DEVICE_IMAGE, 0, (char*)&buffer);

	sblock.crc = CRC64((const unsigned char*)buffer, BLOCK_SIZE);

	int i;
	for(i=0; i<sblock.numinodes; i++)
		if(inodes_x[i].opened == 1) {printf("inode %d is still open\n", i);return -1;}

	// write sblock to disk
	bwrite(DEVICE_IMAGE, 1, (char*)&sblock);

	// To write the i-node map to disk
	for (i=0; i<sblock.inodeMapNumBlocks; i++)
		bwrite(DEVICE_IMAGE, 2+i, (char*)i_map + i*BLOCK_SIZE) ;

	// To write the block map to disk
	for (i=0; i<sblock.dataMapNumBlock; i++)
		bwrite(DEVICE_IMAGE, 2+i+sblock.inodeMapNumBlocks, (char *)b_map + i*BLOCK_SIZE);

	// To write the i-nodes to disk
	for (i=0; i<(sblock.numinodes*sizeof(inode)/BLOCK_SIZE); i++)
		bwrite(DEVICE_IMAGE, i+sblock.firstInode, (char *)inodes + i*BLOCK_SIZE);

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName){
	int b_id, inode_id;

	if(strlen(fileName)>32) return -2;
	if(namei(fileName)!=-1) return -1;//File already exists

	inode_id = ialloc();
	if(inode_id<0) return -2;

	b_id = balloc();
	if(b_id<0){
		ifree(inode_id);
		return -2;
	}

	inodes[inode_id].type = FILE;
	strcpy(inodes[inode_id].name, fileName);
	inodes[inode_id].directBlock = 2+b_id;
	inodes_x[inode_id].position = 0;//seek pointer to beggining as stated on F2 
	inodes_x[inode_id].opened = 1;

	unsigned char buffer[inodes[inode_id].size];

	readFile(inode_id, buffer, 0);

	inodes[inode_id].crc = CRC64((const unsigned char*) buffer, inodes[inode_id].size);

	return 0;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error.
 */
int removeFile(char *fileName){
	int inode_id;

	inode_id = namei(fileName);
	if(inode_id<0) return -1;

	if(bfree(inodes[inode_id].directBlock)<0) return -2;
	memset(&(inodes[inode_id]), 0, sizeof(inode));
	if(ifree(inode_id)<0) return -2;

	return 0;
}

/*
 * @brief	Opens an existing file. 
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName){
	int inode_id;

	if(checkFile(fileName)<0) return -2;//F6

	inode_id = namei(fileName);
	if(inode_id<0){
		return -1;
	} else{
		inodes_x[inode_id].position = 0;
		inodes_x[inode_id].opened = 1;

		return inode_id;
	}
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise. 
 */
int closeFile(int fileDescriptor){
	if(fileDescriptor<0 || inodes_x[fileDescriptor].opened==0) return -1;

	inodes_x[fileDescriptor].position = 0;
	inodes_x[fileDescriptor].opened = 0;

	unsigned char tempBuffer[inodes[fileDescriptor].size];

	lseekFile(fileDescriptor, FS_SEEK_BEGIN, 0);
	readFile(fileDescriptor, tempBuffer, inodes[fileDescriptor].size);

	inodes[fileDescriptor].crc = CRC64((const unsigned char*) tempBuffer, inodes[fileDescriptor].size);

	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes){
	char b[BLOCK_SIZE];
	int b_id;

	if(inodes_x[fileDescriptor].position + numBytes>inodes[fileDescriptor].size)
		numBytes = inodes[fileDescriptor].size - inodes_x[fileDescriptor].position;
	if(numBytes<=0) return -1;

	b_id = bmap(fileDescriptor, inodes_x[fileDescriptor].position);
	bread(DEVICE_IMAGE, b_id, b);
	memmove(buffer, b+inodes_x[fileDescriptor].position, numBytes);
	inodes_x[fileDescriptor].position += numBytes;

	return numBytes;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes){
	char b[BLOCK_SIZE];
	int b_id;

	if(inodes_x[fileDescriptor].position+numBytes>BLOCK_SIZE)
		numBytes = BLOCK_SIZE - inodes_x[fileDescriptor].position;
	if(numBytes<=0) return -1;

	b_id = bmap(fileDescriptor, inodes_x[fileDescriptor].position);

	bread(DEVICE_IMAGE, b_id, b);
	memmove(b+inodes_x[fileDescriptor].position, buffer, numBytes);
	bwrite(DEVICE_IMAGE, b_id, b);

	inodes_x[fileDescriptor].position += numBytes;
	inodes[fileDescriptor].size += numBytes;

	// unsigned char tbuffer[inodes[fileDescriptor].size];
	// 
	// readFile(fileDescriptor, tbuffer, inodes[fileDescriptor].size); 
	//
	// inodes[fileDescriptor].crc = CRC16((const unsigned char*) tbuffer, inodes[fileDescriptor].size);

	return numBytes;
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if success, -1 otherwise.
 */
int lseekFile(int fileDescriptor, int whence, long offset){

	if(fileDescriptor<0) return -1;

	if(whence == FS_SEEK_CUR){
		inodes_x[fileDescriptor].position += offset;
		return 0;
	}
	if(whence == FS_SEEK_BEGIN){
		inodes_x[fileDescriptor].position = offset;
		return 0;
	}
	if(whence == FS_SEEK_END){
		inodes_x[fileDescriptor].position = inodes[fileDescriptor].size - offset;
		return 0;
	}

	return -1;
}

/*
 * @brief 	Verifies the integrity of the file system metadata.
 * @return 	0 if the file system is correct, -1 if the file system is corrupted, -2 in case of error.
 */
int checkFS(void){

	uint64_t check = sblock.crc;
	uint64_t tempcrc;
	sblock.crc = 0;
	unsigned char buffer[BLOCK_SIZE];

	bread(DEVICE_IMAGE, 0, (char*)&buffer);

	tempcrc = CRC64((const unsigned char*) buffer, BLOCK_SIZE);

	if(check == tempcrc){
		sblock.crc = tempcrc;
		return 0;
	}

	return -1;
}

/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName){

	int fd = namei(fileName);

	if(fd<0) return -2;

	uint64_t check = inodes[fd].crc;
	uint64_t tempcrc;
	unsigned char buffer[inodes[fd].size];

	if (inodes[fd].size >= 0){
		lseekFile(fd, FS_SEEK_BEGIN, 0);
		readFile(fd, buffer, inodes[fd].size);
	}

	tempcrc = CRC64((const unsigned char*) buffer, inodes[fd].size);


	if(check == tempcrc){
		inodes[fd].crc = tempcrc;
		return 0;
	}
	return -1;
}


/*
 * @brief 	Tries to fix the corruption of a file in SOME cases where a chunk of 8 bits of data or less is corrupted.
 * @return 	0 if we believe the file is fixed, -1 if we believe the file can't be fixed.
 */
int fixFile(char *fileName){

	int fd = namei(fileName);

	if(fd<0) return -2;

	uint64_t check = inodes[fd].crc;
	uint64_t tempcrc;
	unsigned char buffer[inodes[fd].size];

	if (inodes[fd].size >= 0){
		lseekFile(fd, FS_SEEK_BEGIN, 0);
		readFile(fd, buffer, inodes[fd].size);
	}

	unsigned char actual, temp = 0;
	int position = 0;
	while (position<inodes[fd].size){
		actual = buffer[position];
		temp = 0;
		buffer[position] = temp;
		tempcrc = CRC64((const unsigned char*) buffer, inodes[fd].size);
		if(check == tempcrc){
			inodes[fd].crc = tempcrc;
			return 0;
		}
		temp++;
		while (temp>0){
			buffer[position] = temp;
			tempcrc = CRC64((const unsigned char*) buffer, inodes[fd].size);
			if(check == tempcrc){
				inodes[fd].crc = tempcrc;
				return 0;
			}
		temp++;
		}
		buffer[position] = actual;
		position++;
	}

	return -1;
}


/* AUXILIARY FUNCTIONS */
/*
*@brief  allocates an inode
*@return  the number of the inode alloc'd or -1 in case there is no free inodes
*/
int ialloc(void){
	int i;
	for(i=0; i<sblock.numinodes; i++){
		if(i_map[i] == 0){
			i_map[i] = 1;
			memset(&(inodes[i]), 0, sizeof(inode));
			return i;
		}
	}
	return -1;
}

/*
*@brief  allocates a block
*@return  the number of the block alloc'd or -1 in case there is no free blocks
*/
int balloc(void){
	char b[BLOCK_SIZE];
	int i;
	for(i=0; i<sblock.dataBlockNum; i++){
		if(b_map[i] == 0){
			b_map[i] = 1;
			memset(b, 0, BLOCK_SIZE);
			bwrite(DEVICE_IMAGE, i+sblock.firstDataBlock, b);

			return i;
		}
	}
	return -1;
}

/*
*@brief  frees an inode
*@return  the number of the inode free'd or -1 in case the inode does not exist
*/
int ifree(int inode_id){
	if(inode_id>sblock.numinodes || inode_id<0) return -1;

	i_map[inode_id] = 0;

	return 0;
}

/*
*@brief  frees a block
*@return  the number of the block freed'd or -1 in case the block does not exist
*/
int bfree(int block_id){
	if(block_id>sblock.dataBlockNum || block_id<0) return -1;

	b_map[block_id] = 0;

	return 0;
}

/*
*@brief  searches for an inode by name
*@return  the number of the inode searched or -1 in case it was not found
*/
int namei(char* fname){
	int i;
	for(i=0; i<sblock.numinodes; i++){
		if(!strcmp(inodes[i].name, fname)) return i;
	}
	return -1;
}

/*
*@brief  gets the inode block
*@return  the number of the clock thet is thge inodes direct block or -1 in case the inode does not extist
*/
int bmap(int inode_id, int offset){
	int b[BLOCK_SIZE/4];
	// check if inode_id is valid
	if (inode_id > sblock.numinodes) return -1;
	// check if the offset is in the direct block
	if (offset < BLOCK_SIZE)return inodes[inode_id].directBlock;

	//check if the ofset is in the indirect block
	if (offset < BLOCK_SIZE*BLOCK_SIZE/4) {
		bread(DEVICE_IMAGE, inodes[inode_id].indirectBlock, (char*)b);
		offset = (offset - BLOCK_SIZE)/BLOCK_SIZE;
		return b[offset] ;
	}

	return -1;
}
