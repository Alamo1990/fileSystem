/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */
 
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
