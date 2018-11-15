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

struct instr InstructionDecode(int MachineInstr);

void SingleCycleStep(int reg[], char *mem, struct instr instruction, int *pc);

void VerboseInstruction(int reg[], char *mem, struct instr instruction, int *pc);

void RISCVExecute(void);

void SCRISCVProcessor(void);

int getBits(int val, int high, int low);


#endif