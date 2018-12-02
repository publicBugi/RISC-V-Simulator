
Plan :
Read instructions from file 					: Complete
Read results from file							: Complete
Processor Switch statement  					: Complete
	Processor functions							: Complete

User interface (Select test)					: Scrapped (Command based)
Automatic compare of .res and test result 		: Complete
COMPLETE TASK 1									: Complete
COMPLETE TASK 2									: Complete
COMPLETE TASK 3									: Complete
COMPLETE INSTRUCTION TESTS						: Complete

PRIORITY: Curses based Debugger					: Partially Complete
	WINDOW1: Machine code 						: Complete
	WINDOW2: Assembly code 						: Complete (Minor bugs)
	WINDOW3: Console (ECALL Functions) 			: EXIT Implemented
	WINDOW4: Registers 							: Complete
	WINDOW5: Memory 							: Complete
	Function1: Step by step view				: Complete (One way)
	Function2: Select Machine/Assembly line 
			   and autostep to breakpoint 		: Partially
	Function3: More navigation features (QOL)	: Partially
Add MUL, DIV and RISC-V Extension functions. 	: M Extension implemented
Add more ECALL Functions(?)						: Not priority
Tidy code / Helper functions 				   	: Partially


PRIORITY: SAVE RESULT TO BINARY					: COMPLETE

Save file THURSDAY. DONE
Autostep & Navigation SATURDAY
Clean code and fix bugs SUNDAY (CLI and CURSES Commands have to be clean)
If I have time I'd like to implement the important extensions to the processor.

NOTE: Specify Endianness (RISC-V is little Endian / Option to support viewing it in Big endian?)


For the report: I did the final test with the T.A, and got t1 and t14 wrong. I solved them shortly after as:
t1 : My SRLI didn't account to sign, so accidentally got sign extended. Fixed by casting it to unsigned.
t14 : I originally didn't implement memory and store my program into a seperate array. .Data files were not included in memory, quickly fixed by storing my program into memory with a for loop.

Before I hand in I'll make it run entirely off program memory.

Frontpage
Abstract
Introduction
The Processor
The NCurses Debugger
Testing
Conclusion
Bilag: Source code
Bilag: Results from tests/task1 , 2 , 3 and Final tests 1-14.


Test - All passed
Optional instruction test taken from AlMan (?)
	- Division Unsigned not passed; 10 / 0 = 0x0a but I insist it's INT_MAX - 1
	- Random10 error; Some values differ by one, and one value should be zero is ffffff.
		- With the precense of 

```C
int *progr;
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
	free(progr);```