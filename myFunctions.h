#pragma once

#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

#include <stdint.h>

// define: verify active ir
#define ACTIVE_IR ((myRegisters->ccr >> 5) & 1)

// function prototypes
void flush(void);
void memDump(void * memptr, unsigned offset, unsigned length);
void go(struct Registers * myRegisters, void * memory);
int loadFile(void * memory, unsigned int max);
void modify(void * memory);
void writeFile(void * memory);
void reset(struct Registers * myRegisters);
void showRegisters(struct Registers * myRegisters);
void printBinary16(uint16_t x);
void printBinary8(uint8_t x);
int instructionCycle(struct Registers * myRegisters, void * memory);
void fetch(struct Registers * myRegisters, void * memory);
void debug(struct Registers * myRegisters);
void help(void);

#endif