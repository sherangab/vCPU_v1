#pragma once

#ifndef MYCPU_H
#define MYCPU_H

// define current instruction
#define CIR myRegisters->ir[ACTIVE_IR]

// define instructions
#define STP 0xE000
#define BRA 0xC000
#define MOV 0x4000
#define ADD 0x0400
#define LDR 0x2800
#define STR 0x2000
#define AND 0x0000
#define BXX 0x8000
#define SUB 0x0200

// define operands
#define BRA_OP 0xFFF
#define RD 0xF
#define RN 0xF0
#define IMM 0xFF0

// define conditional branches
#define BRA_EQ 0x80
#define BRA_NE 0x81
#define BRA_CS 0x82
#define BRA_CC 0x83
#define BRA_MI 0x84
#define BRA_PL 0x85
#define BRA_HI 0x88
#define BRA_LS 0x89
#define BRA_AL 0x8E

// define other
#define MAX32 0xFFFFFFFF

// function prototypes
int execute(struct Registers * myRegisters, void * memory);
void flags(struct Registers * myRegisters, int reg);
void updatePC(struct Registers * myRegisters, uint8_t x);

#endif // !MYCPU_H