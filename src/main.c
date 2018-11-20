#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "IO.h"
#include "interface.h"
#include "RISCVProcessor.h"



struct flags{
	int Debug:1;
	int Verbose:1;
	int Testmode:1;
	int Output:1;
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
	/*int *progr;
	long pc_max = loadArray(&progr, argv[1]);
	char **AssemblyCode = malloc(pc_max);
	for (int i = 0; i < (pc_max); i++){
		AssemblyCode[i] = malloc(50);
		VerboseInstruction(&AssemblyCode[i], InstructionDecode(*(progr + i)), i*4);
		
		printf(*(AssemblyCode+i));
		printf("\n");
		printf("%p\n\n", *(AssemblyCode+i));
	}
	for (int i = 0; i < pc_max; i++){
		printf(*(AssemblyCode+i));
		printf("\n");
		printf("%p\n\n", *(AssemblyCode+i));
	}
	free(AssemblyCode);
	free(progr);*/
	
	// LOAD PROGRAM FILE (Does file exist?)
	int *progr; // Program array (Allocated in loadArray)
	int pc_max = loadArray(&progr, argv[1]) * 4; // Load program file
	int pc = 0;
	int reg[32] = {0};
	reg[2] = 0x7ffffff0; // Stack pointer
	reg[3] = 0x10000000; // Data
	char *mem = malloc(0x7ffffffc);
	struct instr instruction;
	struct ctrlSignals Control;

	// BOOT CURSES
	if (progFlags.Debug){
		initscr(); 				// Initialize screen
		cbreak();				// Set input mode
		noecho();				// Disable echo
		keypad(stdscr, TRUE);	// Enable additional inputs
		clear();				// Clear terminal

		// ALLOCATE WINDOWS
		WINDOW *W_MachineCode = newwin(LINES * 0.8,COLS * 0.4,0,0);
		WINDOW *W_AssemblyCode = newwin(LINES * 0.8, COLS * 0.4,0,COLS*0.4);
		WINDOW *W_Registers = newwin(LINES * 0.3, COLS*0.2, 0, COLS*0.8);
		WINDOW *W_Memory = newwin(LINES*0.7, COLS*0.2, LINES*0.3, COLS*0.8); 
		WINDOW *W_Console = newwin(LINES * 0.2, COLS * 0.8, LINES*0.8, 0);

		box(W_MachineCode, 0, 0);
		box(W_AssemblyCode, 0, 0);
		box(W_Registers, 0, 0);
		box(W_Memory, 0, 0);
		box(W_Console, 0, 0);
		refresh();

		int User_MemPoint = reg[2];
		

		char **AssemblyCode;
		AssemblyCode = malloc((pc_max/4));
		for (int i = 0; i < (pc_max/sizeof(int)); i++){
			VerboseInstruction(&(*(AssemblyCode+i)),InstructionDecode(*(progr + i)), i*4);
			//mvwprintw(W_AssemblyCode, 1+i, 1, *(AssemblyCode+i));
			//wrefresh(W_AssemblyCode);
			//getch();
		}
		
		int CH_Size = 1;
		int CH_Point = 0;
		int CH_USPoint = 0;
		char **ConsoleHistory = malloc(CH_Size);
		for (int i = 0; i < CH_Size; i++){
			*(ConsoleHistory+CH_Point) = malloc(5);
			//snprintf(*ConsoleHistory, 5, "Test");
		}
		for (int i = 0; i < CH_Size; i++){
			mvwprintw(W_Console, 1 + i, 1, *ConsoleHistory);
		}
		
		wprintRegisters(reg, W_Registers);
		wprintMemory(mem, W_Memory, User_MemPoint);
		wprintMachine(progr, W_MachineCode, pc/4, pc_max/4);
		wprintAssembly(AssemblyCode, W_AssemblyCode, pc/4, pc_max/4);
		
		wrefresh(W_MachineCode);
		wrefresh(W_AssemblyCode);
		wrefresh(W_Registers);
		wrefresh(W_Memory);
		wrefresh(W_Console);

		getch();

		while(pc < pc_max && !(pc < 0) && !(Control.Halt)){
			
			instruction = InstructionDecode(*(progr + (pc/4)));
			
			Control = SingleCycleStep(reg, mem, instruction, &pc);
			
			if (reg[0] != 0){
				reg[0] = 0;
			}


			if (Control.ECALL){
				switch(reg[0xa]){
					case 1 : // Print Integer

						break;
					case 4 : // Print String
						break;

					case 9 : // SBRK (Allocate Heap)
						break;

					case 10 : // Exit
						CH_Point++;
						if (CH_Size < CH_Point){
							CH_Size++;
							ConsoleHistory = realloc(ConsoleHistory, CH_Size);
						}
						*(ConsoleHistory+CH_Point) = malloc(5);
						snprintf(*ConsoleHistory, 5, "EXIT");
						Control.Halt = 1;
						break;

					case 11 : // Print character
						break;

					case 17 : // Exit 2
						CH_Point++;
						if (CH_Size < CH_Point){
							CH_Size++;
							ConsoleHistory = realloc(ConsoleHistory, CH_Size);
						}
						*(ConsoleHistory+CH_Point) = malloc(6);
						snprintf(*ConsoleHistory, 6, "EXIT2");
						Control.Halt = 1;
						break;

					default : // Unknown ECALL

						break;
				}
			}

			if (Control.EBREAK){

			}

			if (Control.Branch == 0){
				pc += 4;
			}
			
			
			User_MemPoint = reg[2] + 80;
			
			wprintRegisters(reg, W_Registers);
			wprintMemory(mem, W_Memory, User_MemPoint);
			wprintMachine(progr, W_MachineCode, pc/4, pc_max/4);
			wprintAssembly(AssemblyCode, W_AssemblyCode, pc/4, pc_max/4);
			//wprintConsole(ConsoleHistory, W_Console, pc/4);
			for (int i = 0; i < CH_Size; i++){
				mvwprintw(W_Console, 1 + i, 1, *ConsoleHistory);
			}

			wrefresh(W_MachineCode);
			wrefresh(W_AssemblyCode);
			wrefresh(W_Registers);
			wrefresh(W_Memory);
			wrefresh(W_Console);
			
			getch();
		}

		getch();
		endwin();
		free(AssemblyCode);
		
	}
	else {
			// RUN FILE
		while(pc < pc_max && !(pc < 0) && !(Control.Halt)){
			instruction = InstructionDecode(*(progr + (pc/4)));
			Control = SingleCycleStep(reg, mem, instruction, &pc);

			if (reg[0] != 0){
				reg[0] = 0;
			}

			if (Control.ECALL){
				switch(reg[0xa]){
					case 1 : // Print Integer

						break;
					case 4 : // Print String
						break;

					case 9 : // SBRK (Allocate Heap)
						break;

					case 10 : // Exit
						Control.Halt = 1;
						break;

					case 11 : // Print character
						break;

					case 17 : // Exit 2
						break;

					default : // Unknown ECALL

						break;
				}
			}

			if (Control.Branch == 0){
				pc += 4;
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
	}
	free(mem);
	free(progr);
	exit(0);
}	