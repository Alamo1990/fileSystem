/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/filesystem.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"

#define N_BLOCKS	25						// Number of blocks in the device
#define DEV_SIZE 	N_BLOCKS * BLOCK_SIZE	// Device size, in bytes

void assertEquals(char* testName, int result, int expected){
	if(result != expected) {
		printf("result: %d\nexpected: %d\n", result, expected);
		fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		exit(-1);
	}
	fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
}

void assertStrEquals(char* testName, char* result, char* expected){
	if(strcmp(result, expected)) {
		fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		exit(-1);
	}
	fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
}

void assertGreaterThan(char* testName, int result, int expected){
	if(result > expected) {
		fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		exit(-1);
	}
	fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
}



int main() {

	assertEquals("incorrect mkFS", mkFS(99999999), -1);

	assertEquals("mkFS", mkFS(DEV_SIZE), 0);

	assertEquals("mountFS", mountFS(), 0);

	assertEquals("createFile1", createFile("test.txt"), 0);
	assertEquals("createFile exists", createFile("test.txt"), -1);
	assertEquals("createFile large name", createFile("thisFileNameIsLongerThan32Characters.txt"), -2);
	assertEquals("createFile2", createFile("differentTest.txt"), 0);
	assertEquals("createFile3", createFile("oneMoreTest.txt"), 0);

	assertEquals("checkFile1", checkFile("test.txt"), 0);

	int fd1;
	assertGreaterThan("openFile1", (fd1=openFile("test.txt")), 0);

	assertEquals("writeFile1", writeFile(fd1, "This is a test text to test the functionality of write, lseek and read.", 72), 72);

	assertEquals("lseekFile1", lseekFile(fd1, FS_SEEK_END, 40), 0);

	char buff[256];
	assertEquals("readFile1 return value", readFile(fd1, buff, 40), 40);

	assertStrEquals("readFile1 buffer value", buff, "functionality of write, lseek and read.");

	assertEquals("closeFile1", closeFile(fd1), 0);

	assertEquals("closeFile2", closeFile(openFile("differentTest.txt")), 0);

	assertEquals("closeFile3", closeFile(openFile("oneMoreTest.txt")), 0);

	assertEquals("checkFile1 again", checkFile("test.txt"), 0);

	//////////////////////////

	//Corrupting "test.txt"

	char b[BLOCK_SIZE];

	bread(DEVICE_IMAGE, 2, b);

	b[10] = '0';

	bwrite(DEVICE_IMAGE, 2, b);

	//////////////////////////

	assertEquals("checkFile1 corrupted", checkFile("test.txt"), -1);

	assertEquals("fixFile1", fixFile("test.txt"), 0);

	assertEquals("removeFile1", removeFile("test.txt"), 0);

	assertEquals("incorrect removeFile", removeFile("test.txt"), -1);

	assertEquals("checkFS", checkFS(), 0);

	assertEquals("unmountFS", unmountFS(), 0);


	return 0;
}
