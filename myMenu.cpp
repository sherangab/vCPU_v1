#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "myMenu.h"
#include "myFunctions.h"
#include "myCPU.h"

/**
	Main menu
**/
void menu() {

	int instrReturn;
	char choice;
	int bytesRead;
	unsigned offset, length;

	// initialize memory, declared in myMenu.h
	char memory[MEMSIZE];

	// initialize registers and reset, declared in myMenu.h
	struct Registers myRegisters;
	reset(&myRegisters);

	while (1) {

		// display menu
		fprintf(stdout, "\n\n---------------------------------------------------");
		fprintf(stdout, "\nSheranga Balasuriya: Welcome to my Virtual CPU\nPlease select one of the following");
		fprintf(stdout, "\nd\tdump memory");
		fprintf(stdout, "\ng\tgo");
		fprintf(stdout, "\nl\tload a file into memory");
		fprintf(stdout, "\nm\tmemory modify");
		fprintf(stdout, "\nq\tquit");
		fprintf(stdout, "\nr\tdisplay registers");
		fprintf(stdout, "\nt\ttrace");
		fprintf(stdout, "\nw\twrite file");
		fprintf(stdout, "\nz\treset registers");
		fprintf(stdout, "\nb\tenable/disable debugging");
		fprintf(stdout, "\n? , h\tdisplay help\n");
		fscanf(stdin, " %1c", &choice);
		flush();

		// check choice taken
		switch (choice) {
		case 'd':
		case 'D':
			// get offset from user
			fprintf(stdout, "\nPleae enter the offset in hex: ");
			fscanf(stdin, "%x", &offset);
			flush();

			// get length from user
			fprintf(stdout, "\nPlease enter the length in hex: ");
			fscanf(stdin, "%x", &length);
			flush();

			memDump(memory, offset, length);
			break;
		case 'g':
		case 'G':
			go(&myRegisters,memory);
			break;
		case 'l':
		case 'L':
			bytesRead = loadFile(memory, MEMSIZE);
			if (bytesRead > 0)
				fprintf(stdout, "\n%d(0x%X) bytes read into memory", bytesRead, bytesRead);
			break;
		case 'm':
		case 'M':
			modify(memory);
			break;
		case 'q':
		case 'Q':
			return;
			break;
		case 'r':
		case 'R':
			showRegisters(&myRegisters);
			break;
		case 't':
		case 'T':
			instrReturn = instructionCycle(&myRegisters, memory);
			showRegisters(&myRegisters);
			break;
		case 'w':
		case 'W':
			writeFile(memory);
			break;
		case 'z':
		case 'Z':
			reset(&myRegisters);
			break;
		case 'b':
		case 'B':
			debug(&myRegisters);
			break;
		case '?':
		case 'h':
		case 'H':
			help();
			break;
		default:
			fprintf(stdout, "\nIncorrect entry, try again");
		}
	}
}