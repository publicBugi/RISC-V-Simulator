#ifndef __RISCVPROCESSOR_H_
#define __RISCVPROCESSOR_H_
#include <stdio.h>
#include <stdlib.h>

struct instr{
	unsigned int opcode:7;
	unsigned int rd:5;
	unsigned int funct3:3;
	unsigned int funct7:7;
	unsigned int funct2:2;
	unsigned int rs1:5;
	unsigned int rs2:5;
	unsigned int rs3:5;
	signed int Iimm;
	signed int Simm;
	signed int Bimm;
	signed int Uimm;
	signed int Jimm;
};

struct ctrlSignals{ // These are not actual control signals as described in RISC-V book.
	int Halt:1;
	int Branch:1; 	// Set if branch
	int ECALL:1;	// Set if ECALL
	int EBREAK:1;	// Set if EBREAK
};

struct instr InstructionDecode(int MachineInstr);

struct ctrlSignals SingleCycleStep(int reg[], char *mem, struct instr instruction, int *pc);

char *VerboseInstruction(struct instr instruction, int pc);

void RISCVExecute(void);

void SCRISCVProcessor(void);

int getBits(int val, int high, int low);


#endif