# RISC-V32IM Simulator
As part of Course 01255 "Computer Architecture And Engineering" at the Technical University of Denmark the final project consists of building a RISC-V simulator. This simulator consists of a commandline interface and a visual debugger built with the ncurses library, currently capable of running RISC-V compiled software supporting the base integer set and the multiplication and divison extension.

## Getting Started
To get started with the simulator on your local machine, simply pull a copy of the repository and compile the project using the provided makefile.

`./RISCVSIM {Path to File}`

`./RISCVSIM {Path to File} -d`

`./RISCVSIM {Path to File} -v -t {Path to resultfile} -o {Path to output file}`

**-d** 
ncurses Visual Debugger enabled
**-t {Path to file}** 
Test result against result, given by filepath
**-o {Path to output file}**
Store result at the given filepath.
**-v**
Verbose enabled: Print out each instruction.

**Examples**
`./RISCVSIM tests/task3/loop.bin`

`./RISCVSIM tests/task3/loop.bin -v -t tests/task3/loop.res -o tests/task3/result_task3/loop.res`

`./RISCVSIM tests/task3/loop.bin -d`

### Prerequisites
The simulator uses the ncurses library for the visual debugger.  The ncurses library is default most linux/unix systems, although I have not been able to test it on any other platform.

## Current Issues

### Redundancy in code
There is a lot of redundancy in the code as features were layered on top of each other. I would like to give it another pass making the code easier to read and maintain.

### Unsigned Division
Unable to pass instruction test: unsigned division. I am certain this is an error in the result file, as the manual explicitly states the return value of x/0 is to be $2^XLEN - 1$.

### Odd Char array behaviour
In the visual debugger, the translation of machine code to assembly results in the first elements in the char arrays to contain noise after the first iteration. Emphasis on the first iteration, as it is written correctly but the moment the loop is done translating machine code the first few elements turn to noise.

For small programs the first element is noise, for larger elements several elements are noise.
```C
int *progr;
	long pc_max = loadArray(&progr, argv[1]);
	char **AssemblyCode = malloc(pc_max);
	for (int i = 0; i < (pc_max); i++){
		AssemblyCode[i] = malloc(50);
		VerboseInstruction(&AssemblyCode[i], InstructionDecode(*(progr + i)), i*4);	
		printf(*(AssemblyCode+i));
		printf("\n");
		printf("%p\n\n", *(AssemblyCode+i)); // No issue
	}
	for (int i = 0; i < pc_max; i++){
		printf(*(AssemblyCode+i));
		printf("\n");
		printf("%p\n\n", *(AssemblyCode+i)); // Issue!
	}
	free(AssemblyCode);
	free(progr);
```

# Plan (Old)
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

Before I hand in I'll make it run entirely off program memory.