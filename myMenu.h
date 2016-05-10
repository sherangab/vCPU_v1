#pragma once

#ifndef MYMENU_H
#define MYMENU_H

#include <stdint.h>

// constants
#define MEMSIZE 16000

// function prototypes
void menu(void);

// declare memory
extern char memory[MEMSIZE];

// declare registers
// Reserved, Debugging, Reserved, IR, Stop, Carry, Zero, Sign
struct Registers
{
	uint32_t gen[16];
	uint32_t mbr;
	uint32_t mar;
	uint16_t ir[2];
	uint8_t ccr;
};

#endif // !MYCPU_H#pragma once
