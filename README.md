# vCPU_v1
First attempt at simulating a virtual CPU.

This project consisted of simulating a simple CPU through a program written in C. It simulates the virtual CPU at a register level which allows the user to transfer data between registers and between registers and memory. The vCPU has 16 registers that are visible to the user and 16KB of memory.

The file that is loaded must be an ASCII based text file containing 4 character instructions representing 4 hex characters. Newline characters must be represented by 2 bytes (0x0A and 0x0D). The program follows big endian architecture and negative values are represented using 2’s compliment. Debugging provides unassembled instructions per bytecode.
