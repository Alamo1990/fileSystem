/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file  metadata.h
 * @brief   Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

 #define DIRECTORY 0
 #define FILE 1

typedef struct {
        unsigned int numinodes; //Number of inodes in the device
        unsigned int firstInode; //Number of the first inode(root inode)
        unsigned int dataBlockNum; //Number of data blocks in the device
        unsigned int firstDataBlock; //Number of the 1st data block
        unsigned int deviceSize; //Total disk space in bytes
        uint16_t crc;
        char padding[BLOCK_SIZE - 5*sizeof(unsigned int)-16]; //Padding to complete a block
}superblock;

typedef struct {
        char name[32]; //File or directory name
        unsigned int size; //Current file size in bytes
        unsigned int directBlock; //direct block number
        uint16_t crc;
        char padding[BLOCK_SIZE-32*sizeof(char)-2*sizeof(unsigned int)-16]; //Padding to complete a block
}inode;

typedef struct {
        int position;
        int opened;
}inode_x;
