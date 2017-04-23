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
		printf("result: %s\nexpected: %s\n", result, expected);
		fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		exit(-1);
	}
	fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
}

void assertGreaterThan(char* testName, int result, int expected){
	if(result < expected) {
		printf("result: %d\nexpected: %d\n", result, expected);
		fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		exit(-1);
	}
	fprintf(stdout, "%s%s%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ", testName," ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
}



int main() {
	char buff[256];bzero(buff, 256);
	int fd1, fd2, fd3;

	assertEquals("incorrect mkFS", mkFS(99999999), -1);

	assertEquals("mkFS", mkFS(DEV_SIZE), 0);

	assertEquals("mountFS", mountFS(), 0);

	assertEquals("createFile1", createFile("test.txt"), 0);
	assertEquals("createFile exists", createFile("test.txt"), -1);
	assertEquals("createFile large name", createFile("thisFileNameIsLongerThan32Characters.txt"), -2);
	assertEquals("createFile2", createFile("differentTest.txt"), 0);
	assertEquals("createFile3", createFile("oneMoreTest.txt"), 0);

	assertEquals("checkFile1", checkFile("test.txt"), 0);

	assertGreaterThan("openFile1", (fd1=openFile("test.txt")), 0);
	assertGreaterThan("openFile2", (fd2=openFile("differentTest.txt")), 0);
	assertGreaterThan("openFile3", (fd3=openFile("oneMoreTest.txt")), 0);
	assertEquals("incorrect openFile", openFile("inexistantFile.txt"), -1);

	assertEquals("writeFile1", writeFile(fd1, "This is a test text to test the functionality of write, lseek and read.", 77), 77);
	assertEquals("incorrect writeFile", writeFile(fd2, "This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.This is a text longer than 2048 characters and should give an error.	This is a text longer than 2048 characters and should give an error.",
	 2055), -1);

	assertEquals("writeFile2", writeFile(fd2, "This is another test text to test the functionality of write, lseek and read.", 78), 78);
	assertEquals("lseekFile2 BEGIN", lseekFile(fd2, FS_SEEK_BEGIN, 20), 0);
	assertEquals("writeFile2 again", writeFile(fd2, "This is another test text to test the functionality of write, lseek and read.", 78), 78);
	assertEquals("lseekFile2 CUR", lseekFile(fd2, FS_SEEK_CUR, -98), 0);
	assertEquals("readFile2 return value", readFile(fd2, buff, 98), 98);
	assertStrEquals("readFile2 buffer value", buff, "This is another testThis is another test text to test the functionality of write, lseek and read.");

	assertEquals("lseekFile1 END", lseekFile(fd1, FS_SEEK_END, 45), 0);
	assertEquals("readFile1 return value", readFile(fd1, buff, 40), 40);
	assertStrEquals("readFile1 buffer value", buff, "functionality of write, lseek and read.");

	assertEquals("closeFile1", closeFile(fd1), 0);
	assertEquals("closeFile2", closeFile(fd2), 0);

	assertEquals("checkFile1 again", checkFile("test.txt"), 0);

	assertEquals("removeFile1", removeFile("test.txt"), 0);

	assertEquals("incorrect removeFile", removeFile("test.txt"), -1);

	assertEquals("unmountFS", unmountFS(), -1);
	assertEquals("closeFile3", closeFile(fd3), 0);
	assertEquals("unmountFS", unmountFS(), 0);

	assertEquals("re-mountFS", mountFS(), 0);

	return 0;
}
