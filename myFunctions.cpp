#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myMenu.h"
#include "myFunctions.h"
#include "myCPU.h"

/**
	flush the input buffer
**/
void flush() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/**
	dump the contents of memory
	memptr: pointer to memory
	offset: number of bytes offset to start
	length: number of bytes to show
**/
void memDump(void * memptr, unsigned offset, unsigned length) {
	fprintf(stdout, "\nMemory Dump\n");

	// check if offset too large
	if ((offset > MEMSIZE) || (offset<0)) {
		fprintf(stdout, "\nOffset error, printing from beginnig.");
		offset = 0;
	}

	// check if length too large
	if (((offset + length) > MEMSIZE) || (length < 1)) {
		fprintf(stdout, "\nLength too large, dumping all memory starting from offset.");
		length = MEMSIZE - offset;
	}

	int i = offset;
	char data;
	while (i < length) {

		// print line of hex code
		fprintf(stdout, "\n%d\t", i);
		for (int i2 = 0;i2 < 16;i2++) {
			if ((i + i2) == length)
				break;

			// retrieve character
			data = *((char *)memptr + i + i2);

			// print character
			fprintf(stdout, "%.2X ", data);
		}

		// print line of ASCII code
		fprintf(stdout, "\n\t");
		for (int i2 = 0;i2 < 16;i2++) {
			if ((i + i2) == length)
				break;

			// retrieve character and verify if printable
			data = *((char *)memptr + i + i2);
			if ((data < 33) || (data > 126))
				data = '.';

			// print character
			fprintf(stdout, " %c ", data);
		}

		i += 16;
	}
}

/**
	Execute instructions in memory until stop
	myRegisters: pointer to register structure
	memory: pointer to memory
**/
void go(struct Registers * myRegisters, void * memory) {
	fprintf(stdout, "\nRun");
	while (instructionCycle(myRegisters, memory) != 2);
}

/** 
	Load contents of file into memory
	memory: pointer to memory
	max: maximum bytes to read
	returns: number of bytes read
**/
int loadFile(void * memory, unsigned int max) {
	FILE *fp;
	char fileName[21];
	long fileSize;
	size_t result;

	fprintf(stdout,"\nLoad");
	
	//prompt for filename
	fprintf(stdout, "\nPlease enter file name (20 character max): ");
	fscanf(stdin, "%20s", fileName);
	flush();
	
	//open file and read to memory
	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		fprintf(stdout, "Error opening file %s", fileName);
		return -1;
	}
	else {
		//determine file size
		fseek(fp, 0L, SEEK_END);
		fileSize = ftell(fp);
		if (fileSize > max)
			fprintf(stdout, "File is %ld bytes long, only %d(x%X) bytes will be read",fileSize,max-1,max-1);
		fseek(fp, 0L, SEEK_SET);
		//read contents of file into memory
		result = fread(memory, sizeof(char), max - 1, fp);
		fclose(fp);
		return result;
	}
}

/**
	Modify contents of memory
	memory: pointer to memory
**/
void modify(void * memory) {
	unsigned address;
	char data;
	char update[3] = { '\0' };
	fprintf(stdout, "\nMemory Modify\n");
	
	// prompt user for starting address
	do {
		fprintf(stdout, "\nPlease enter a starting address in hex(0-%x): ", MEMSIZE);
		fscanf(stdin, "%x", &address);
		flush();
	} while ((address < 0) || (address > MEMSIZE));

	fprintf(stdout, "\nEnter Hex value to update (0-FF), 2E to exit, any other value to move to next address");
	fprintf(stdout, "\naddr\tvalue\tupdated\n");

	// show specified address
	while (1) {

		// reset input string
		update[0] = '\0';
		update[1] = '\0';
		
		// verify if memory is printable
		data = *((char *)memory);
		if ((data < 33) || (data > 126))
			data = '.';

		// prompt user for updated value
		fprintf(stdout, "%.2X\t%c\t", address, data);
		fscanf(stdin, "%2s", &update);
		flush();

		// check what user has entered
		if ( (isxdigit(update[0])) && (isxdigit(update[1])) ) {
			*(char *)memory = strtol(update,NULL,16);
			memory = (char *)memory + sizeof(char);
			address++;
		}
		else if (strcmp(update,".")==0) {
			return;
		}
		else {
			memory = (char *)memory + sizeof(char);
			address++;
		}
	}
}

/**
	Write the contents of memory to a file
	memory: pointer to memory
**/
void writeFile(void * memory) {
	FILE *fp;
	char fileName[21];
	int nBytes = 0;
	size_t result;

	fprintf(stdout,"\nWrite");

	//prompt for filename
	fprintf(stdout, "\nPlease enter file name (20 character max): ");
	fscanf(stdin, "%20s", fileName);
	flush();

	//prompt for number of bytes to write
	do {
		fprintf(stdout, "Please enter number of bytes to write (less than %d): ",MEMSIZE);
		fscanf(stdin, "%d", &nBytes);
		flush();
	} while (nBytes>MEMSIZE);
	
	//open file and write memory
	fp = fopen(fileName, "wb");
	if (fp == NULL) {
		fprintf(stdout, "Error opening file %s", fileName);
		return;
	}
	else {
		result = fwrite(memory, sizeof(char), nBytes, fp);
		fclose(fp);
	}

	//check if successfully written
	if (result > 0)
		fprintf(stdout, "%zu Bytes successfully written to %s", result, fileName);
	else
		fprintf(stdout, "Error writing to file");
}

/**
	Reset registers
	myRegisters: pointer to register structure
**/
void reset(struct Registers * myRegisters) {
	fprintf(stdout, "\nResetting Registers");
	for (int i = 0;i < 16;i++)
		myRegisters->gen[i] = 0;
	myRegisters->mbr = 0;
	myRegisters->mar = 0;
	myRegisters->ir[0] = 0;
	myRegisters->ir[1] = 0;
	myRegisters->ccr = 0;
}

/**
	Show registers
	myRegisters: pointer to register structure
**/
void showRegisters(struct Registers * myRegisters) {
	fprintf(stdout, "\nDisplaying Registers");

	// display general registers
	for (int i = 0;i < 16;i += 2) {
		fprintf(stdout, "\nR%d\t%.8X\tR%d\t%.8X", i, myRegisters->gen[i], i + 1, myRegisters->gen[i + 1]);
	}

	// display rest of registers
	fprintf(stdout, "\nMBR\t%.8X\tMAR\t%.8X",myRegisters->mbr,myRegisters->mar);

	fprintf(stdout, "\nIR0\t");
	printBinary16(myRegisters->ir[0]);

	fprintf(stdout, "\tIR1\t");
	printBinary16(myRegisters->ir[1]);

	// TODO split ccr flag prints
	fprintf(stdout, "\nCCR\t");
	printBinary8(myRegisters->ccr);
}

// TODO combine binary prints
/**
	Print Binary represenation of 16 bit Int
	x: int to be printed
**/
void printBinary16(uint16_t x) {
	for (int i = 0; i < 16;i++) {
		if (x & 0x8000)
			fprintf(stdout, "1");
		else
			fprintf(stdout, "0");
		x = x << 1;
	}
}

/**
	Print Binary represenation of 8 bit Int
	x: int to be printed
**/
void printBinary8(uint8_t x) {
	for (int i = 0; i < 8;i++) {
		if (x & 0x80)
			fprintf(stdout, "1");
		else
			fprintf(stdout, "0");
		x = x << 1;
	}
}

/**
	Instruction Cycle
		Consists of one fetch and execute command
	myRegisters: pointer to register structure
	memory: pointer to memory structure
**/
int instructionCycle(struct Registers * myRegisters, void * memory) {
	fetch(myRegisters, memory);
	return execute(myRegisters, memory);
}

/**
	Fetch new instruction from memory
	myRegisters: pointer to register structure
	memory: pointer to memory structure
**/
void fetch(struct Registers * myRegisters, void * memory) {
	
	char value;

	// reset mbr and active ir
	myRegisters->mbr = 0;
	myRegisters->ir[ACTIVE_IR] = 0;

	// testing
	//myRegisters->gen[0] = 0xA;
	//myRegisters->gen[1] = 0xC;

	// load new memory address
	myRegisters->mar = myRegisters->gen[15];
	memory = (char *)memory + (myRegisters->mar * sizeof(char));

	// load 4 bytes into mbr from memory at mar
	for (int i = 0; i < 4;i++, myRegisters->mar++) {
		value = *((char *)memory);
		myRegisters->mbr += strtol(&value, NULL, 16);
		memory = (char *)memory + sizeof(char);
		if (i<3)
			myRegisters->mbr = myRegisters->mbr << 4;
	}

	// load instruction into active ir
	myRegisters->ir[ACTIVE_IR] = myRegisters->mbr;
}

/**
	Enable/disable debugging features
	myRegisters: pointer to register structure
**/
void debug(struct Registers * myRegisters) {
	if ((myRegisters->ccr >> 6) && 0x1)
		fprintf(stdout, "debugging disabled");
	else
		fprintf(stdout, "debugging enabled");
	myRegisters->ccr ^= 1 << 6;
}

/**
	Display help
**/
void help() {
	fprintf(stdout,"\nHelp");
	fprintf(stdout, "\nPlease choice from one of options.");
	fprintf(stdout, "\nDump Memory: dump user defined area of memory");
	fprintf(stdout, "\nRun: run program starting at memory 0x0");
	fprintf(stdout, "\nLoad: load user defined file");
	fprintf(stdout, "\nModify: directly modify user specified area of memory");
	fprintf(stdout, "\nQuit: end virtual CPU program");
	fprintf(stdout, "\nDisplay: show registers");
	fprintf(stdout, "\nTrace: execute one instruction at a time starting at 0x0");
	fprintf(stdout, "\nWrite: write contents of memory to file");
	fprintf(stdout, "\nReset: reset all the registers to zero");
	fprintf(stdout, "\nDebugging: enable showing of debugging information");
	fprintf(stdout, "\nHelp: shows this description");
	// TODO add more help
}