
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

PRIORITY: Curses based Debugger					: Priority
	WINDOW1: Machine code 						: Complete
	WINDOW2: Assembly code 						: Complete (Minor bugs)
	WINDOW3: Console (ECALL Functions) 			: EXIT Implemented
	WINDOW4: Registers 							: Complete
	WINDOW5: Memory 							: Complete
	Function1: Step by step view				: Complete (One way)
	Function2: Select Machine/Assembly line 
			   and autostep to breakpoint 		: Pending
	Function3: More navigation features (QOL)	: Pending
Add MUL, DIV and RISC-V Extension functions. 	: Pending
Add more ECALL Functions(?)						: Not priority
Tidy code / Helper functions 				   	: Pending


PRIORITY: SAVE RESULT TO BINARY					: COMPLETE

Save file THURSDAY. DONE
Autostep & Navigation SATURDAY
Clean code and fix bugs SUNDAY (CLI and CURSES Commands have to be clean)
If I have time I'd like to implement the important extensions to the processor.




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