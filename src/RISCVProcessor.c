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

		return instruction;

		


}

struct ctrlSignals SingleCycleStep(int reg[], char *mem, struct instr instruction, int *pc){
	struct ctrlSignals Control;
	Control.Branch = 0;
	Control.ECALL = 0;
	Control.EBREAK = 0;
	Control.Halt = 0;
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
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				case 0x1 : // Branch if not equal
					if (reg[instruction.rs1] != reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				case 0x4 : // Branch less than
					if (reg[instruction.rs1] < reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				case 0x5 : // Branch greater equal than
					if (reg[instruction.rs1] >= reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				case 0x6 : // Branch less than unsigned
					if (reg[instruction.rs1] < (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				case 0x7 : // Branch greater than unsigned
					if (reg[instruction.rs1] >= (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm;
						Control.Branch = 1;
					}
					break;
				default :
					break;
			}
			break;
		case 0x6F : // JAL
			reg[instruction.rd] = *pc + 4;
			*pc = *pc + instruction.Jimm;
			Control.Branch = 1;
			break;
		case 0x67 : // JALR
			reg[instruction.rd] = *pc + 4;
			*pc = (((instruction.Iimm + reg[instruction.rs1]) & 0xfffffffe));
			Control.Branch = 1;
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 : // ECALL
					Control.ECALL = 1;
					break;
				case 0x1 : // 
					Control.EBREAK = 1;
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
	return Control;
}

void VerboseInstruction(char **output_str, struct instr instruction, int pc){
	*output_str = malloc(50);
	switch(instruction.opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(instruction.funct3){
				case 0x0 :// Load Byte
					snprintf(*output_str, 50, "%#04x : LB %#06x, %#06x, %#010x", pc ,instruction.rd, instruction.rs1, instruction.Iimm);
					//printf("LB %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 :// Load Halfword
					snprintf(*output_str, 50,"%#04x : LH %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 :// Load Word
					snprintf(*output_str, 50,"%#04x : LW %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x4 : // Load Byte Unsigned
					snprintf(*output_str, 50,"%#04x : LBU %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x5 :// Load Half Unsigned
					snprintf(*output_str, 50,"%#04x : LHU %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				default :
					snprintf(*output_str, 50,"%#04x : Invalid load function", pc);
					break;
			}
			break;
		// Stores
		case 0x23 :
			switch(instruction.funct3){
				case 0x0 : // Store Byte
					snprintf(*output_str, 50,"%#04x : SB %#06x, %#06x, %#010x",pc, instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x1 : // Store Halfword
					snprintf(*output_str, 50,"%#04x : SH %#06x, %#06x, %#010x",pc, instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x2 : // Store Word
					snprintf(*output_str, 50,"%#04x : SW %#06x, %#06x, %#010x",pc, instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				default :
					snprintf(*output_str, 50,"%#04x : Invalid store function!" , pc);
					break;
				}
			break;
		// I-Type
		case 0x13 :
			switch(instruction.funct3){
				case 0x0 : // Add Immediate
					snprintf(*output_str, 50,"%#04x : ADDI %#06x , %#06x , %#010x",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 : // Śet < Immediate
					snprintf(*output_str, 50,"%#04x : SLTI %#06x, %#06x, %#010x",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x3 : // Set < Immediate unsigned
					snprintf(*output_str, 50,"%#04x : SLTIU %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, (unsigned)instruction.Iimm);
					break;
				case 0x4 : // XOR Immediate
					snprintf(*output_str, 50,"%#04x : XORI %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x6 : // OR Immediate
					snprintf(*output_str, 50,"%#04x : ORI %#06x, %#06x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x7 : // AND Immediate
					snprintf(*output_str, 50,"%#04x : ANDI %#06x, %#06x, %#010x",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 : // Shift left Immediate
					snprintf(*output_str, 50,"%#04x : SLLI %02d, %#010x, %#010x",pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							snprintf(*output_str, 50,"%#04x : SRLI %02d, %#010x, %#010x", pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 0x20 :
							snprintf(*output_str, 50,"%#04x : SRAI %02d, %#010x , %#010x",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						default :
							snprintf(*output_str, 50,"%#04x : Shift not valid",pc);
							break;
					}
					break;
				default :
					snprintf(*output_str, 50,"%#04x : Funct3 not implemented!",pc);
					break;
			}
			break;
		case 0x33 :
			switch(instruction.funct3){
				case 0x0 :
					switch(instruction.funct7){
						case 0x0 : // ADD
							snprintf(*output_str, 50,"%#04x : ADD %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 0x20 : // SUB
							snprintf(*output_str, 50,"%#04x : SUB %#06x, %#06x, %#06x",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						default :
							snprintf(*output_str, 50,"%#04x : Funct not implemented", pc);
							break;
						}
						break;
				case 0x1 : // SHift left
					snprintf(*output_str, 50,"%#04x : SLL %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x2 : // Set <
					snprintf(*output_str, 50,"%#04x : SLT %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x3 : // Set < Unsigned
					snprintf(*output_str, 50,"%#04x : SET < UNSIGNED", pc);
					break;
				case 0x4 : // XOR
					snprintf(*output_str, 50,"%#04x : XOR %#06x, %#06x, %#06x",pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Shift right and Right Arithm
					switch(instruction.funct7){
						case 0x0 :
							snprintf(*output_str, 50,"%#04x : SRL %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
						case 0x20 :
							snprintf(*output_str, 50,"%#04x : SRA %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs2, instruction.rs1);
							break;
						default :
							snprintf(*output_str, 50,"%#04x : Funct3 not valid", pc);
							break;
					}
					break;
				case 0x6 : // OR
					snprintf(*output_str, 50,"%#04x : OR %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x7 : // AND
					snprintf(*output_str, 50,"%#04x : AND %#06x, %#06x, %#06x", pc, instruction.rd, instruction.rs1, instruction.rs2);
				break;
				default :
					snprintf(*output_str, 50,"%#04x : Funct3 not implemented", pc);
					break;
			}
			break;
		case 0x63 : // Branch
			switch(instruction.funct3){
				case 0x0 : // Branch if Equal
					snprintf(*output_str, 50,"%#04x : BEQ %d = %d",pc,instruction.rs1, instruction.rs2);
					break;
				case 0x1 : // Branch if not equal
					snprintf(*output_str, 50,"%#04x : BNE %d != %d",pc,instruction.rs1, instruction.rs2);
					break;
				case 0x4 : // Branch less than
					snprintf(*output_str, 50,"%#04x : BLT %d < %d",pc,instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Branch greater equal than
					snprintf(*output_str, 50,"%#04x : BGE %d >= %d",pc,instruction.rs1, instruction.rs2);
					break;
				case 0x6 : // Branch less than unsigned
					snprintf(*output_str, 50,"%#04x : BLTU %d < %d",pc,instruction.rs1, instruction.rs2);
					break;
				case 0x7 : // Branch greater than unsigned					
					snprintf(*output_str, 50,"%#04x : BGEU %d >= %d",pc,instruction.rs1, instruction.rs2);
					break;
				default :
					snprintf(*output_str, 50,"%#04x : Invalid branch instruction", pc);
					break;
			}
			break;
		case 0x6F : // Jump and Link
			snprintf(*output_str, 50,"%#04x : JAL %#02x, %#010x", pc, instruction.rd, instruction.Jimm);
			break;
		case 0x67 :
			snprintf(*output_str, 50,"%#04x : JALR %#02x, %#02x, %#010x", pc, instruction.rd, instruction.rs1, instruction.Jimm);
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 :
					snprintf(*output_str, 50,"%#04x : ECALL", pc);
					break;
				case 0x1 :
					snprintf(*output_str, 50,"%#04x : EBREAK", pc);
					break;
				default :
					snprintf(*output_str, 50,"%#04x : Error ECALL",pc);
					break;
			}
			break;
		case 0x37 :
			snprintf(*output_str, 50,"%#04x : LUI %#06x, %#010x",pc, instruction.rd, instruction.Uimm);
			break;
		case 0x17 :
			snprintf(*output_str,  50,"%#04x : AUIPC %#010x, %#010x",pc, instruction.Uimm, pc);
			break;
		default : // Unknown
			snprintf(*output_str, 50,"%#04x : Opcode not implemented!",pc);
			break;
	}
}
