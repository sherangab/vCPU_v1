#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "myCPU.h"
#include "myMenu.h"
#include "myFunctions.h"

/**
	execution of one instruction
	myRegisters: pointer to register structure
	memory: pointer to memory structure
**/
int execute(struct Registers * myRegisters, void * memory) {
	// TODO check flags

	//printf("\nExecute");

	char value;
	void * memory2;

	// check which instruction
	if (CIR == STP) {
		// stop instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nstop");
		myRegisters->ccr |= 1 << 4;
		return 2;
	}
	else if ((CIR & BRA) == BRA) {
		// branch instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nbranch");
		myRegisters->gen[15] = CIR & BRA_OP;

		// flip ir flag and return
		myRegisters->ccr ^= 1 << 5;
		return 0;
	}
	else if ((CIR & MOV) == MOV) {
		// move instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nmove");
		myRegisters->gen[CIR & RD] = (CIR & IMM) >> 4;
		myRegisters->gen[15] = myRegisters->mar + 2;

		// check and set flags
		flags(myRegisters, CIR & RD);
	}
	else if ((CIR & LDR) == LDR) {
		// load instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nload");
		memory = (char *)memory + myRegisters->gen[((CIR & RN) >> 4)];
		myRegisters->gen[CIR & RD] = 0;

		// load new data from memory
		for (int i = 0; i < 4;i++) {
			value = *((char *)memory);
			myRegisters->gen[CIR & RD] += strtol(&value, NULL, 16);
			memory = (char *)memory + sizeof(char);
			if (i<3)
				myRegisters->gen[CIR & RD] <<= 4;
		}

		// check and set flags
		flags(myRegisters, CIR & RD);
	}
	else if ((CIR & STR) == STR) {
		// store instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("store");

		memory2 = (char *)memory + myRegisters->gen[CIR & RD];
		uint32_t num = myRegisters->gen[((CIR & RN) >> 4)];

		sprintf((char *)memory2, "%X", (num & 0xF0000000)>>28);
		sprintf((char *)memory2 + 1, "%X", (num & 0xF000000)>>24);
		sprintf((char *)memory2 + 2, "%X", (num & 0xF00000) >> 20);
		sprintf((char *)memory2 + 3, "%X", (num & 0xF0000) >> 16);
		sprintf((char *)memory2 + 4, "%X", (num & 0xF000) >> 12);
		sprintf((char *)memory2 + 5, "%X", (num & 0xF00) >> 8);
		sprintf((char *)memory2 + 6, "%X", (num & 0xF0) >> 4);
		sprintf((char *)memory2 + 7, "%X", num & 0xF);
	}
	else if ((CIR & BXX) == BXX) {
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nbranch: %x", CIR >> 8);
		// conditional branch instruction
		if ((CIR >> 8) == BRA_NE) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nbranch not equal");
			if (!(myRegisters->ccr >> 1) & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_CS) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nunsigned higher or same");
			if ((myRegisters->ccr >> 2) & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_CC) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nunsigned lower");
			if (!(myRegisters->ccr >> 2) & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_MI) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nnegative");
			if (myRegisters->ccr & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_PL) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\npositive");
			if ((myRegisters->ccr) & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_HI) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nunsigned higher");
			if (((myRegisters->ccr >> 2) & 1) && (!(myRegisters->ccr >> 1) & 1))
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_LS) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nunsigned lower or same");
			if (!((myRegisters->ccr >> 2) & 1) && ((myRegisters->ccr >> 1) & 1))
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		else if ((CIR >> 8) == BRA_AL) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nalways");
			updatePC(myRegisters, CIR & 0xFF);
		}
		else if ((CIR >> 8) == BRA_EQ) {
			if ((myRegisters->ccr >> 6) && 0x1)
				printf("\nbranch if equal");
			if ((myRegisters->ccr >> 2) & 1)
				updatePC(myRegisters, CIR & 0xFF);
			else
				myRegisters->gen[15] = myRegisters->mar + 2;
		}
		
		myRegisters->ccr ^= 1 << 5;
		return 0;
	}
	else if ((CIR & ADD) == ADD) {
		// add instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nadd");
		// check and set carry flag
		if ((myRegisters->gen[((CIR & RN) >> 4)] == MAX32) ||
			(myRegisters->gen[CIR & RD] > (MAX32 - myRegisters->gen[((CIR & RN) >> 4)])))
			myRegisters->ccr |= 1 << 3;

		// add registers
		myRegisters->gen[CIR & RD] = myRegisters->gen[((CIR & RN) >> 4)] + myRegisters->gen[(CIR & RD)];

		// check and set flags
		flags(myRegisters, CIR & RD);
	}
	else if ((CIR & SUB) == SUB) {
		// sub instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nsub");
		//check and set negative flag
		if((myRegisters->gen[CIR & RD])>myRegisters->gen[((CIR & RN) >> 4)])
			myRegisters->ccr |= 1 << 1;

		// sub registers
		myRegisters->gen[CIR & RD] = myRegisters->gen[(CIR & RD)] - myRegisters->gen[((CIR & RN) >> 4)];

		// check and set flags
		flags(myRegisters, CIR & RD);
	}
	else if ((CIR & AND) == AND) {
		// and instruction
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nand");
		myRegisters->gen[CIR & RD] &= myRegisters->gen[((CIR & RN) >> 4)];

		// check and set flags
		flags(myRegisters, CIR & RD);
	}
	else {
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("General Instruction Error");
		return 1;
	}

	// flip ir flag, update pc, and return
	myRegisters->ccr ^= 1 << 5;
	myRegisters->gen[15] = myRegisters->mar + 2;
	return 0;
}

/**
	Set the flags
	myRegisters: pointer to register structure
	reg: register that was used
**/
void flags(struct Registers * myRegisters, int reg) {
	// check and set zero flag
	if (myRegisters->gen[reg] == 0)
		myRegisters->ccr |= 1 << 1;
	else
		myRegisters->ccr &= ~(1 << 1);
	
	// check and set negative flag
	if ((myRegisters->gen[reg] & 0x80000000) == 0x80000000)
		myRegisters->ccr |= 1 << 0;
	else
		myRegisters->ccr &= ~(1 << 0);
}

/**
	Update PC counter
	myRegisters: pointer to register structure
	x: offset for PC
**/
void updatePC(struct Registers * myRegisters, uint8_t x) {
	if ((myRegisters->ccr >> 6) && 0x1)
		printf("\nrelative address: %X", x);
	// check if negative
	if ((x & 0x80) == 0x80) {
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\nnegative: %X", ~x);
		myRegisters->gen[15] -= (~x & 0xFF);
	}
	else {
		if ((myRegisters->ccr >> 6) && 0x1)
			printf("\npositive");
		myRegisters->gen[15] += x;
	}
}