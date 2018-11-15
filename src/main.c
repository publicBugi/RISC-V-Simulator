#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "IO.h"
#include "interface.h"
#include "RISCVProcessor.h"



struct flags{
	unsigned int Debug:1;
	unsigned int Verbose:1;
	unsigned int Testmode:1;
	unsigned int Output:1;
};


int main(int argc, char** argv) {
	// RISCVSIM <FILE>
	//	optional:
	//			 -d  				(Enables curses debugging)
	//			 -o <OUTPUT FILE> 	(Save final state)
	//			 -t <TEST FILE>		(Test mode)

	if (argc == 1 || argc > 6){			// Arguments given within range?
		printf("Invalid arguments\n");
		exit(1);
	}

	struct flags progFlags;
	int outputFile;
	int resultFile;	
	progFlags.Debug = 0;
	progFlags.Testmode = 0;
	progFlags.Output = 0;
	progFlags.Verbose = 0;

	for (int i = 2; i < argc; i++){		// Simple argument parser (Change to parsing library later)
		switch(argv[i][1]){
			case 'd' :
				progFlags.Debug = 1;
				printf("Enable ncurses based Debugger\n");
				break;
			case 'o' :
				i++;
				outputFile = i;
				progFlags.Output = 1;
				printf("Output file %s \n", argv[i]);
				break;
			case 't' :
				i++;
				resultFile = i;
				progFlags.Testmode = 1;
				printf("Testmode enabled\n");
				break;
			case 'v' :
				progFlags.Verbose = 1;
				printf("Verbose enabled\n");
				break;
			default :
				printf("Invalid argument %s\n", argv[i]);
				exit(1);
				break;
		}	
	}


	// LOAD PROGRAM FILE (Does file exist?)
	int *progr; // Program array (Allocated in loadArray)
	int pc_max = loadArray(&progr, argv[1]) * 4; // Load program file
	int pc = 0;
	int reg[32] = {0};
	reg[2] = 0x7ffffff0; // Stack pointer
	reg[3] = 0x10000000; // Data
	char *mem = malloc(0x7ffffffc);
	struct instr instruction;

	// BOOT CURSES

	// RUN FILE
	while(pc < pc_max && !(pc < 0)){
		instruction = InstructionDecode(*(progr + (pc/4)));
		if (progFlags.Verbose && !progFlags.Debug){
			printf("%#04x : ", pc);
			VerboseInstruction(reg, mem, instruction, &pc);
		}
		SingleCycleStep(reg, mem, instruction, &pc);
		pc += 4;

		if (reg[0] != 0){
			reg[0] = 0;
		}
	}
	
	int *resArray;
	// LOAD RESULT FILE (Does file exist?)
	if(progFlags.Testmode){
		loadArray(&resArray, argv[resultFile]);
	}
	// TEST FILES
	if(progFlags.Verbose){
		printRegister(reg);
		if (progFlags.Testmode){
			printRegister(resArray);
			testResult(reg, resArray);
			free(resArray);
		}
	}

	free(mem);
	free(progr);

	exit(0);
}