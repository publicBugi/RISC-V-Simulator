#include "RISCVProcessor.h"

int getBits(int val, int high, int low){
	if (high == 31){ 
		return ((val << (31 - high)) >> (31 + low - high)); 
	}
	else{ // Dirty fix to prevent C suddenly doing arithmetic shifts where we do not care for sign bit.
		return (((unsigned)val << (31 - high)) >> (31 + low - high));	
	}
}

struct instr InstructionDecode(int MachineInstr){
		struct instr instruction;

		instruction.opcode = getBits(MachineInstr, 6,0);
		instruction.rd = getBits(MachineInstr, 11, 7);
		instruction.rs1 = getBits(MachineInstr, 19, 15);
		instruction.rs2 = getBits(MachineInstr, 24, 20);
		instruction.rs3 = getBits(MachineInstr, 31, 27);
		instruction.funct3 = getBits(MachineInstr, 14, 12);
		instruction.funct2 = getBits(MachineInstr, 26, 25);
		instruction.funct7 = getBits(MachineInstr, 31, 25);
		instruction.Iimm = getBits(MachineInstr ,31, 20);
		instruction.Simm = (getBits(MachineInstr ,31, 25) << 5) | 
							getBits(MachineInstr ,11, 7);	
		instruction.Bimm = ((getBits(MachineInstr,31,31) << 12) | 
							(getBits(MachineInstr, 7, 7) << 11) | 
							(getBits(MachineInstr, 30, 25) << 5) | 
							(getBits(MachineInstr, 11, 8) << 1));
		instruction.Uimm = getBits(MachineInstr, 31,12) << 12;
		instruction.Jimm = (getBits(MachineInstr,31,31) << 20) | 
							(getBits(MachineInstr, 30, 21) << 1) | 
							(getBits(MachineInstr, 20,20) << 11) | 
							(getBits(MachineInstr, 19,12) << 12);

		/*printf("Opcode: %#010x\n", instruction.opcode);
		printf("rd: %#010x\n", instruction.rd);
		printf("funct3: %#010x\n", instruction.funct3);
		printf("funct7: %#010x\n", instruction.funct7);
		printf("funct2: %#010x\n", instruction.funct2);
		printf("rs1: %#010x\n", instruction.rs1);
		printf("rs2: %#010x\n", instruction.rs2);
		printf("rs3: %#010x\n", instruction.rs3);
		printf("Iimm: %010x\n", instruction.Iimm);
		printf("Simm: %#010x\n", instruction.Simm);
		printf("Bimm: %#010x\n", instruction.Bimm);
		printf("Uimm: %#010x\n", instruction.Uimm);
		printf("Jimm: %#010x\n", instruction.Jimm);*/

		return instruction;

		


}

void SingleCycleStep(int reg[], char *mem, struct instr instruction, int *pc){
	switch(instruction.opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(instruction.funct3){
				case 0x0 :// Load Byte
					reg[instruction.rd] = mem[instruction.Iimm + reg[instruction.rs1]];
					break;
				case 0x1 :// Load Halfword
					reg[instruction.rd] = mem[instruction.Iimm + reg[instruction.rs1]] & 0x000000ff;
					reg[instruction.rd] |= mem[instruction.Iimm + reg[instruction.rs1] + 1] << 8;
					break;
				case 0x2 :// Load Word
					reg[instruction.rd] = mem[instruction.Iimm + reg[instruction.rs1]] & 0x000000ff;
					reg[instruction.rd] |= (mem[instruction.Iimm + reg[instruction.rs1] + 1] << 8) &0x0000ff00;
					reg[instruction.rd] |= (mem[instruction.Iimm + reg[instruction.rs1] + 2] << 16) & 0x00ff0000;
					reg[instruction.rd] |= mem[instruction.Iimm + reg[instruction.rs1] + 3] << 24;
					break;
				case 0x4 : // Load Byte Unsigned
					reg[instruction.rd] =mem[(instruction.Iimm + reg[instruction.rs1])] &0xff;
					break;
				case 0x5 :// Load Half Unsigned
					reg[instruction.rd] = mem[(instruction.Iimm + reg[instruction.rs1])] & 0x000000ff;
					reg[instruction.rd] |= (mem[(instruction.Iimm + reg[instruction.rs1] + 1)] << 8) & 0xff00;
					break;
				default :
					break;
			}
			break;
		// Stores
		case 0x23 :
			switch(instruction.funct3){
				case 0x0 : // Store Byte
					mem[reg[instruction.rs1] + instruction.Simm] = reg[instruction.rs2] & 0x000000ff;
					break;
				case 0x1 : // Store Halfword
					mem[reg[instruction.rs1] + instruction.Simm] = reg[instruction.rs2] & 0x000000ff;
					mem[reg[instruction.rs1] + instruction.Simm + 1] = (reg[instruction.rs2] & 0x0000ff00) >> 8;
					break;
				case 0x2 : // Store Word
					mem[reg[instruction.rs1] + instruction.Simm] = reg[instruction.rs2] & 0x000000ff;
					mem[reg[instruction.rs1] + instruction.Simm + 1] = (reg[instruction.rs2] & 0x0000ff00) >> 8;
					mem[reg[instruction.rs1] + instruction.Simm + 2] = (reg[instruction.rs2] & 0x00ff0000) >> 16;
					mem[reg[instruction.rs1] + instruction.Simm + 3] = (reg[instruction.rs2] & 0xff000000) >> 24;
					break;
				default :
					break;
				}
			break;
		// I-Type
		case 0x13 :
			switch(instruction.funct3){
				case 0x0 : // Add Immediate
					reg[instruction.rd] = reg[instruction.rs1] + instruction.Iimm;
					break;
				case 0x2 : // Śet < Immediate
					if(reg[instruction.rs1] < instruction.Iimm){
						reg[instruction.rd] = 1;
					}
					else{
						reg[instruction.rd] = 0;
					}
					break;
				case 0x3 : // Set < Immediate unsigned
					if(reg[instruction.rs1] < (unsigned)instruction.Iimm){
						reg[instruction.rd] = 1;
					}
					else{
						reg[instruction.rd] = 0;
					}
					break;
				case 0x4 : // XOR Immediate
					reg[instruction.rd] = reg[instruction.rs1] ^ instruction.Iimm;
					break;
				case 0x6 : // OR Immediate
					reg[instruction.rd] = reg[instruction.rs1] | instruction.Iimm;
					break;
				case 0x7 : // AND Immediate
					reg[instruction.rd] = reg[instruction.rs1] & instruction.Iimm;
					break;
				case 0x1 : // Shift left Immediate
					reg[instruction.rd] = reg[instruction.rs1] << instruction.rs2;
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							reg[instruction.rd] = reg[instruction.rs1] >> instruction.rs2;
							break;
						case 0x20 :
							reg[instruction.rd] = reg[instruction.rs1] >> instruction.rs2;
							break;
						default :
							break;
					}
					break;
				default :
					break;
			}
			break;
		case 0x33 :
			switch(instruction.funct3){
				case 0x0 :
					switch(instruction.funct7){
						case 0x0 : // ADD
							reg[instruction.rd] = reg[instruction.rs1] + reg[instruction.rs2];
							break;
						case 0x20 : // SUB
							reg[instruction.rd] = reg[instruction.rs1] - reg[instruction.rs2]; // ??
							break;
						default :
							break;
						}
						break;
				case 0x1 : // SHift left
					reg[instruction.rd] = reg[instruction.rs1] << reg[instruction.rs2];
					break;
				case 0x2 : // Set <
					if (reg[instruction.rs1] < reg[instruction.rs2]){
						reg[instruction.rd] = 1;
					}
					else {
						reg[instruction.rd] = 0;
					}
					break;
				case 0x3 : // Set < Unsigned
					if (reg[instruction.rs1] < (unsigned)reg[instruction.rs2]){
						reg[instruction.rd] = 1;
					}
					else {
						reg[instruction.rd] = 0;
					}
					break;
				case 0x4 : // XOR
					reg[instruction.rd] = reg[instruction.rs1] ^ reg[instruction.rs2];
					break;
				case 0x5 : // Shift right and Right Arithm
					switch(instruction.funct7){
						case 0x0 :
							reg[instruction.rd] = (unsigned)reg[instruction.rs1] >> reg[instruction.rs2];
						break;
						case 0x20 :
							reg[instruction.rd] = reg[instruction.rs1] >> reg[instruction.rs2]; //WRONG
							break;
						default :
							break;
					}
					break;
				case 0x6 : // OR
					reg[instruction.rd] = reg[instruction.rs1] | reg[instruction.rs2];
					break;
				case 0x7 : // AND
					reg[instruction.rd] = reg[instruction.rs1] & reg[instruction.rs2];
				break;
				default :
					break;
			}
			break;
		case 0x63 : // Branch
			switch(instruction.funct3){
				case 0x0 : // Branch if Equal
					if (reg[instruction.rs1] == reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				case 0x1 : // Branch if not equal
					if (reg[instruction.rs1] != reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				case 0x4 : // Branch less than
					if (reg[instruction.rs1] < reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				case 0x5 : // Branch greater equal than
					if (reg[instruction.rs1] >= reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				case 0x6 : // Branch less than unsigned
					if (reg[instruction.rs1] < (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				case 0x7 : // Branch greater than unsigned
					if (reg[instruction.rs1] >= (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm - 4;
					}
					break;
				default :
					break;
			}
			break;
		case 0x6F : // Jump and Link
			reg[instruction.rd] = *pc + 4;
			*pc = *pc + instruction.Jimm - 4;
			break;
		case 0x67 :
			reg[instruction.rd] = *pc + 4;
			*pc = (((instruction.Iimm + reg[instruction.rs1]) & 0xfffffffe))-4;
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 :
					switch(reg[0xa]){
						case 0xa :
							*pc = -8;
							break;
						default :
							break;
					}
					break;
				case 0x1 :
					break;
				default :
					break;
			}
			break;
		case 0x37 : // LUI
			reg[instruction.rd] = instruction.Uimm;
			break;
		case 0x17 : // AUIPC
			reg[instruction.rd] = *pc + instruction.Uimm;
			break;
		default : // Unknown
			break;
	}
}

void VerboseInstruction(int reg[], char *mem, struct instr instruction, int *pc){
	switch(instruction.opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(instruction.funct3){
				case 0x0 :// Load Byte
					printf("LB %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 :// Load Halfword
					printf("LH %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 :// Load Word
					printf("LW %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x4 : // Load Byte Unsigned
					printf("LBU %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x5 :// Load Half Unsigned
					printf("LHU %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				default :
					printf("Invalid load function\n");
					break;
			}
			break;
		// Stores
		case 0x23 :
			switch(instruction.funct3){
				case 0x0 : // Store Byte
					printf("SB %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x1 : // Store Halfword
					printf("SH %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x2 : // Store Word
					printf("SW %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				default :
					printf("Invalid store function!\n");
					break;
				}
			break;
		// I-Type
		case 0x13 :
			switch(instruction.funct3){
				case 0x0 : // Add Immediate
					printf("ADDI %#04x , %#010x , %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 : // Śet < Immediate
					printf("SLTI %#04x, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.Iimm);
					break;
				case 0x3 : // Set < Immediate unsigned
					printf("SLTIU %#04x, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], (unsigned)instruction.Iimm);
					break;
				case 0x4 : // XOR Immediate
					printf("XORI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x6 : // OR Immediate
					printf("ORI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x7 : // AND Immediate
					printf("ANDI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 : // Shift left Immediate
					printf("SLLI %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.rs2);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							printf("SRLI %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.rs2);
							break;
						case 0x20 :
							printf("SRAI %02d, %#010x , %#010x\n", instruction.rd, reg[instruction.rs1], instruction.rs2);
							break;
						default :
							printf("Shift not valid\n");
							break;
					}
					break;
				default :
					printf("Funct3 not implemented!\n");
					break;
			}
			break;
		case 0x33 :
			switch(instruction.funct3){
				case 0x0 :
					switch(instruction.funct7){
						case 0x0 : // ADD
							printf("ADD %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], reg[instruction.rs2]);
							break;
						case 0x20 : // SUB
							printf("SUB %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], reg[instruction.rs2]);
							break;
						default :
							printf("Funct not implemented\n");
							break;
						}
						break;
				case 0x1 : // SHift left
					printf("SLL %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x2 : // Set <
					printf("SLT %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x3 : // Set < Unsigned
					printf("SET < UNSIGNED");
					break;
				case 0x4 : // XOR
					printf("XOR %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Shift right and Right Arithm
					switch(instruction.funct7){
						case 0x0 :
							printf("SRL %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
						break;
						case 0x20 :
							printf("SRA %02d, %#010x, %#010x\n", instruction.rd, instruction.rs2, instruction.rs1);
							break;
						default :
							printf("Funct3 not valid\n");
							break;
					}
					break;
				case 0x6 : // OR
					printf("OR %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x7 : // AND
					printf("AND %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
				break;
				default :
					printf("Funct3 not implemented\n");
					break;
			}
			break;
		case 0x63 : // Branch
			switch(instruction.funct3){
				case 0x0 : // Branch if Equal
					printf("BEQ %d (%d) = %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x1 : // Branch if not equal
					printf("BNE %d (%d) != %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x4 : // Branch less than
					printf("BLT %d (%d) < %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x5 : // Branch greater equal than
					printf("BEQ %d (%d) >= %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x6 : // Branch less than unsigned
					printf("BEQ %d (%u) < %d (%u)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x7 : // Branch greater than unsigned					
					printf("BEQ %d (%u) >= %d (%u)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				default :
					printf("Invalid branch instruction\n");
					break;
			}
			break;
		case 0x6F : // Jump and Link
			printf("JAL %#02x, %#010x\n", instruction.rd, instruction.Jimm);
			break;
		case 0x67 :
			printf("JALR %#02x, %#02x, %#010x\n", instruction.rd, instruction.rs1, instruction.Jimm);
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 :
					switch(reg[0xa]){
						case 0xa :
							printf("ECALL EXIT\n");
							break;
						default :
							printf("Unknown ECALL\n");
							break;
					}
					break;
				case 0x1 :
					printf("EBREAK\n");
					break;
				default :
					printf("Error ECALL\n");
					break;
			}
			break;
		case 0x37 :
			printf("LUI %#04x, %#010x\n", instruction.rd, instruction.Uimm);
			break;
		case 0x17 :
			printf("AUIPC %#010x, %#010x\n", instruction.Uimm, *pc);
			break;
		default : // Unknown
			printf("Opcode not implemented!\n");
			break;
	}
}
