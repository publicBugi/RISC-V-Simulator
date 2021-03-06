#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "IO.h"
#include "interface.h"
#include "RISCVProcessor.h"

struct flags{
	int Debug:1;	// Enable Curses Debugging
	int Verbose:1;	// Enable Verbose console
	int Testmode:1; // Enable testing of registers
	int Output:1;	// Save registers to file
	int outputFile; // Argc to output file path
	int resultFile; // Argc to result file path
};

int main(int argc, char** argv) {
	// RISCVSIM <FILE>
	//	optional:
	//			 -d  				(Enables curses debugging)
	//			 -o <OUTPUT FILE> 	(Save final state)
	//			 -t <TEST FILE>		(Test mode)
	//			 -v 				(Verbose enabled)

	// Zero initialize program flags.
	struct flags progFlags = {Debug:0, Testmode:0, Output:0, Verbose:0};	

	// I'm not familiar with argument parsers in C: This was quickly written and may not be super safe.
	if (argc == 1){	// Require atleast path to file.
		printf("Invalid arguments\n");
		exit(1);
	}

	// Set options
	for (int i = 2; i < argc; i++){
		switch(argv[i][1]){	// Given '-x', is x == parameter?
			case 'd' :			// Debugger enable
				progFlags.Debug = 1;
				printf("Enable ncurses based Debugger\n");
				break;
			case 'o' :			// Store output file at given filepath
				i++;			// Output path is stored in the next string
				progFlags.outputFile = i; // Store output path value.
				progFlags.Output = 1;
				printf("Output file %s \n", argv[i]);
				break;
			case 't' :			// Test final register versus result file
				i++;			// Result filepath is given in next string
				progFlags.resultFile = i;	// Store result path value
				progFlags.Testmode = 1;
				printf("Testmode enabled\n");
				break;
			case 'v' :			// Enable verbose processor
				progFlags.Verbose = 1;
				printf("Verbose enabled\n");
				break;
			default  :			// Unknown argument : EXIT PROGRAM
				printf("Invalid argument %s\n", argv[i]);
				exit(1);
				break;
		}	
	}	
	
	// LOAD PROGRAM FILE (Does file exist?)
	int *progr; 									// Program array (Allocated in loadArray)
	int pc_max = loadArray(&progr, argv[1]) * 4; 	// Load program file, Store PC_MAX (Size of progr Array)
	int pc = 0;										// Program counter starts at zero.
	int reg[32] = {0};								// Allocate registers (Zero initialized)
	reg[2] = 0x7ffffff0; 							// Stack pointer
	reg[3] = 0x10000000; 							// Data pointer
	unsigned char *mem = malloc(reg[2]);			// Allocate CPU memory on heap
	for (int i = 0; i < pc_max; i++){
		mem[(i*4)] = progr[i] & 0xff;
		mem[(i*4)+1] = (progr[i] >> 8)& 0xff;
		mem[(i*4)+2] = (progr[i] >> 16)& 0xff;
		mem[(i*4)+3] = (progr[i] >> 24)& 0xff;
	}
	struct instr instruction;						// Decoded Instruction struct (Declared in RISCVProcessor.h)
	struct ctrlSignals Control = {	Halt:0, 		// Processor control signals (Declared in RISCVProcessor.h)
									Branch:0, 
									ECALL:0, 
									EBREAK:0};		// Important to initialize HALT to zero; Program may not run otherwise.

	// PRIMARY INIT DONE: Next we run:
	
	// Processor execution:
	// 		- CURSES DEBUGGER: ncurses based visual debugger with step by step and memory view.
				// Does not support Verbose function. (It's already pretty verbose).
	// 		- COMMANDLINE EXECUTION: Run file without debugger:
				// Verbose will print each execution step and registers at the end.
				// IF Testmode is enabled; Verbose will also print result register.
	
	// Testmode by itself will print "Test successful" if all registers match up (Excluding Stack and Return address registers)
	// If testmode is not successful, will print the registers that do not comply.

	// If an output path is given, program result will be saved to given file as a binary file.

	// It's possible to run the program without any options, but nothing will really happen.

	// BOOT CURSES (Commandline in else statement)
	if (progFlags.Debug){
		
		// Curses initialization
		initscr(); 				// Initialize screen
		cbreak();				// Set input mode
		noecho();				// Disable echo
		keypad(stdscr, TRUE);	// Enable additional inputs
		clear();				// Clear terminal

		if (has_colors()){
			//start_color();
		}

		// ALLOCATE WINDOWS
		WINDOW *W_MachineCode = newwin(LINES * 0.8,COLS * 0.4,0,0);
		WINDOW *W_AssemblyCode = newwin(LINES * 0.8, COLS * 0.4,0,COLS*0.4);
		WINDOW *W_Registers = newwin(LINES * 0.3, COLS*0.2, 0, COLS*0.8);
		WINDOW *W_Memory = newwin(LINES*0.7, COLS*0.2, LINES*0.3, COLS*0.8); 
		WINDOW *W_Console = newwin(LINES * 0.2, COLS * 0.8, LINES*0.8, 0);

		// Draw window outlines
		box(W_MachineCode, 0, 0);
		box(W_AssemblyCode, 0, 0);
		box(W_Registers, 0, 0);
		box(W_Memory, 0, 0);
		box(W_Console, 0, 0);
		refresh(); // Refresh (Won't work without?)

		// Allocate user position in memory (Default stack pointer)
		int User_MemPoint = reg[2];
		int User_PC = pc;
		int focus = 0;

		// Generate Assembly code from Machine code
		char **AssemblyCode; // Allocate Array of Char Arrays
		AssemblyCode = malloc((pc_max/4)); // PC_MAX / 4 = Program size in decimal
		for (int i = 0; i < (pc_max/4); i++){
			// Decode each instruction in program, and get verbose instruction.
			// Store each instruction in AssemblyCode array.
			AssemblyCode[i] = VerboseInstruction(InstructionDecode(*(progr + i)), i*4);
			
			// Debugging functions (There's a bug here! First element turns to noise)
			//mvwprintw(W_AssemblyCode, 1+i, 2, *(AssemblyCode+i));
			//wrefresh(W_AssemblyCode);
			//getch();
		}
		
		// Allocate char array for Console. (Should redo this; Not good code)
		int CH_Size = 1;					// Current Console Size
		int CH_Point = 0;					// Position in Console
		char **ConsoleHistory = malloc(CH_Size); // Allocate first entry
		for (int i = 0; i < CH_Size; i++){		 // Allocate char array for first entry
			*(ConsoleHistory+CH_Point) = malloc(5); // Won't function without this (Fix this!)
			//snprintf(*ConsoleHistory, 5, "Test");
		}
		int user_Input = 0;

		while(user_Input != 's'){
			user_Input = 0;
			// Update all windows with their respective registers!
			wprintRegisters(reg, W_Registers);
			wprintMemory(mem, W_Memory, User_MemPoint);
			wprintMachine(progr, W_MachineCode, pc/4, pc_max/4);
			wprintAssembly(AssemblyCode, W_AssemblyCode, pc/4, pc_max/4);

			// Refresh all windows (Won't display new information without update call)
			wrefresh(W_MachineCode);
			wrefresh(W_AssemblyCode);
			wrefresh(W_Registers);
			wrefresh(W_Memory);
			wrefresh(W_Console);

			// Halt until user input
			user_Input = getch();
		}

		// RISC-V Processor loop & CURSES Debugger functions
		while(pc < pc_max && !(pc < 0) && !(Control.Halt)){
			user_Input = 0;
			// Processor loop will exit if:
				// PC counter exceeds PC_MAX (Program size)
				// PC goes below zero (Indicates branch / JUMP error!)
				// Control signals a halt (ECALL EXIT)
			
			// Decode instruction (RISCVProcessor.h)
			instruction = InstructionDecode(*(progr + (pc/4)));
			
			// Execute decoded instruction.
			Control = SingleCycleStep(reg, mem, instruction, &pc);
			
			// Ensure x0 is hardwired zero (Better ways to do this?)
			reg[0] = 0;
			
			// Handle Enviroment Calls.
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

			// Increment PC counter by 1 Byte, unless we branch.
			if (Control.Branch == 0){
				pc += 4;
			}
			
			while(user_Input != 's'){
				
				// Write registers to windows
				wprintRegisters(reg, W_Registers);
				wprintMemory(mem, W_Memory, User_MemPoint);
				wprintMachine(progr, W_MachineCode, (pc - User_PC)/4, pc_max/4);
				wprintAssembly(AssemblyCode, W_AssemblyCode, (pc - User_PC)/4, pc_max/4);
				//wprintConsole(ConsoleHistory, W_Console, pc/4);
				for (int i = 0; i < CH_Size; i++){
					mvwprintw(W_Console, 1 + i, 1, *ConsoleHistory);
				}

				// Refresh all windows
				wrefresh(W_MachineCode);
				wrefresh(W_AssemblyCode);
				wrefresh(W_Registers);
				wrefresh(W_Memory);
				wrefresh(W_Console);
				
				// Halt next step until user input (Expand for autostepping)
				user_Input = 0;
				user_Input = getch();

				switch(user_Input){
					case KEY_LEFT :
						if (focus != 1){
							focus++;
						}
						break;

					case KEY_RIGHT :
						if (focus != 0){
							focus--;
						}
						break;
					case KEY_UP :
						switch(focus){
							case 0 :
								User_MemPoint += 4;
								break;
							case 1 :
								User_PC += 4;
								break;
							default :
								focus = 0;
								break;
						}
						break;
					case KEY_DOWN :
						switch(focus){
							case 0 :
								User_MemPoint -= 4;
								break;
							case 1 :
								User_PC -= 4;
								break;
							default :
								focus = 0;
								break;
						}
						break;
					case '.' :
						User_MemPoint = reg[2] + 80;
						break;
					case ',' :
						User_PC = 0;
						break;
					case 's' :
						User_PC = 0;
						break;
				}

			}

			
		}

		// Halt exit until user input
		getch();
		endwin(); // End Curses
		free(AssemblyCode); // Free Allocated AssemblyCode
		// CURSES Debugger exit
	}
	else { 	// EXECUTE COMMAND LINE
		int cycle_Count = 0;
		while(pc < pc_max && !(pc < 0) && !(Control.Halt)){
			cycle_Count++;
			// Processor loop will exit if:
				// PC counter exceeds PC_MAX (Program size)
				// PC goes below zero (Indicates branch / JUMP error!)
				// Control signals a halt (ECALL EXIT)
			
			// Decode instruction (RISCVProcessor.h)
			instruction = InstructionDecode(*(progr + (pc/4)));

			// Execute decoded instruction.
			Control = SingleCycleStep(reg, mem, instruction, &pc);

			// Ensure x0 is hardwired zero (Better ways to do this?)
			if (reg[0] != 0){
				reg[0] = 0;
			}

			// If Verbose flag is set: Print all instructions!
			if (progFlags.Verbose){
				printf(VerboseInstruction(instruction, pc));
				printf("\n");
			}

			// Handle Enviroment Calls.
			if (Control.ECALL){
				switch(reg[0xa]){
					case 1 : // Print Integer
						printf("%d",reg[0xb]);
						break;
					case 4 : // Print String
						break;

					case 9 : // SBRK (Allocate Heap)
						break;

					case 10 : // Exit
						printf("EXIT\n");
						Control.Halt = 1;
						break;

					case 11 : // Print character
						printf("%c", reg[0xb]);
						break;

					case 17 : // Exit 2
						Control.Halt = 1;
						break;

					default : // Unknown ECALL

						break;
				}
			}

			// Increment PC by 1 Byte, Unless we branch.
			if (Control.Branch == 0){
				pc += 4;
			}
		}
		printf("Cycles run: %d\n", cycle_Count);
	
	}
	// PROCESSOR DONE - Additional functions

	int *resArray; // Pointer to result array.
	// LOAD RESULT FILE
	if(progFlags.Testmode){
		loadArray(&resArray, argv[progFlags.resultFile]);
	}

	// Print results!
	if(progFlags.Verbose){
		printf("\nProcessor Register\n"); 
		printRegister(reg); // Print processor results
		if (progFlags.Testmode){
			printf("\nResult Register\n");
			printRegister(resArray); // Print .res file
		}
	}

	// TEST REGISTERS
	if (progFlags.Testmode){
		testResult(reg, resArray); // Compare files; Print result (Success or Error values)
		free(resArray);			 // Free result array.
	}

	// Save to file given by output path.
	if (progFlags.Output){
		FILE *writeOutput = fopen(argv[progFlags.outputFile], "wb");
		if (writeOutput != NULL){
			fwrite(reg, 4, 32, writeOutput);
			fclose(writeOutput);
		}
		else{
			printf("Error File; Remember filename");
		}

	}

	free(mem); // Free Memory
	free(progr); // Free Program
	exit(0);
}	