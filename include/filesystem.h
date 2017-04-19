/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Interface for the core file system funcionalities.
 * @date	01/03/2017
 *
 * DO NOT MODIFY
 */

#ifndef _USER_H_
#define _USER_H_

#include "blocks_cache.h"	// Headers for block managing (read/write)

#define DEVICE_IMAGE "disk.dat"		// Device name
#define MAX_FILE_SIZE 2048			// Maximum file size, in bytes
#define FS_SEEK_CUR 0
#define FS_SEEK_END 1
#define FS_SEEK_BEGIN 2

/*  FROM HERE IS OURS */

#define DIRECTORY 0
#define FILE 1

typedef struct{
  unsigned int magicNum;  //Magig number of the superblock
  unsigned int inodeMapNumBlocks; //Number of blocks of the inode map
  unsigned int dataMapNumBlock; //Number of blocks of the data map
  unsigned int numinodes; //Number of inodes in the device
  unsigned int firstInode;  //Number of the first inode(root inode)
  unsigned int dataBlockNum;  //Number of data blocks in the device
  unsigned int firstDataBlock;  //Number of the 1st data block
  unsigned int deviceSize;  //Total disk space in bytes
  char padding[BLOCK_SIZE - 8*sizeof(unsigned int)];  //Padding to complete a block
}superblock;
typedef struct{
  unsigned int type; //FILE or DIRECTORY
  char name[256]; //File or directory name
  unsigned int inodesContent[256];  //type==dir.directory inode list
  unsigned int size;  //Current file size in bytes
  unsigned int directBlock; //direct block number
  char padding[BLOCK_SIZE-259*sizeof(unsigned int)-256];  //Padding field to fill a block
}inode;

typedef struct{
  int position;
  int opened;
}inode_x;

/*  TILL HERE OURS  */

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize);
/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void);

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void);

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName);

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName);

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName);

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor);

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes);

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes);

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, int whence, long offset);

/*
 * @brief 	Verifies the integrity of the file system metadata.
 * @return 	0 if the file system is correct, -1 if the file system is corrupted, -2 in case of error.
 */
int checkFS(void);

/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName);

#endif
