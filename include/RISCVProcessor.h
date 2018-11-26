#ifndef __RISCVPROCESSOR_H_
#define __RISCVPROCESSOR_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Decoded instruction struct; Refer to RISC-V Instruction Set Manual for further information.
struct instr{
	unsigned int opcode:7; 	// What function type?
	unsigned int rd:5;		// Result Register
	unsigned int funct3:3;	// Function mod 3
	unsigned int funct7:7;	// Function mod 7
	unsigned int funct2:2;  // Function mod 2
	unsigned int rs1:5;		// Register 1
	unsigned int rs2:5;		// Register 2
	unsigned int rs3:5;		// Register 3
	signed int Iimm;		// Immediate type Immediate
	signed int Simm;		// Store type immediate
	signed int Bimm;		// Branch type immediate
	signed int Uimm;		// Upper type immediate
	signed int Jimm;		// Jump type immediate
};

// Control signals to modify main processor loop
struct ctrlSignals{ // These are not actual control signals as described in RISC-V book.
	int Halt:1;		// Set to stop processor. (EXIT / ERROR)
	int Branch:1; 	// Set if Branch
	int ECALL:1;	// Set if ECALL
	int EBREAK:1;	// Set if EBREAK
};

// Given RISC-V Encoded Machine Code, return a decoded instruction struct containing all relevant information.
struct instr InstructionDecode(int MachineInstr);

// A single processor step; This is the core of the entire processor.
struct ctrlSignals SingleCycleStep(int reg[], unsigned char *mem, struct instr instruction, int *pc);

// Dummy processor; Returns a char array in the style of "0x4 : ADDI x0, x0, x0"
char *VerboseInstruction(struct instr instruction, int pc);

// Helper function; Returns a selected bits. See note in function.
int getBits(int val, int high, int low);


#endif