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

struct ctrlSignals SingleCycleStep(int reg[], unsigned char *mem, struct instr instruction, int *pc){
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
					mem[reg[instruction.rs1] + instruction.Simm + 1] = (reg[instruction.rs2] >> 8) & 0x000000ff;
					mem[reg[instruction.rs1] + instruction.Simm + 2] = (reg[instruction.rs2] >> 16) & 0x000000ff;
					mem[reg[instruction.rs1] + instruction.Simm + 3] = (reg[instruction.rs2] >> 24) & 0x000000ff;
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
					reg[instruction.rd] = reg[instruction.rs1] << instruction.Iimm;
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							reg[instruction.rd] = (unsigned)reg[instruction.rs1] >> instruction.Iimm;
							break;
						case 0x20 :
							reg[instruction.rd] = reg[instruction.rs1] >> instruction.Iimm;
							break;
						default :
							break;
					}
					break;
				default :
					break;
			}
			break;
		// 
		case 0x33 :
			switch(instruction.funct7 & 0x1){
				case 0x0 :
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

				//// RISC-V EXTENSION M (MULTIPLICATION AND DIVISION)
				case 0x1 :
					switch(instruction.funct3){
						case 0 : // MUL
							reg[instruction.rd] = (int32_t)(((int64_t)reg[instruction.rs1] * (int64_t)reg[instruction.rs2]) & 0xffffffff);
							break;

						case 1 : // MULH
							reg[instruction.rd] = (int32_t)(((int64_t)reg[instruction.rs1] * (int64_t)reg[instruction.rs2]) >> 32);
							break;

						case 2 : // MULHSU
							reg[instruction.rd] = ((int64_t)reg[instruction.rs1]) * ((uint64_t)reg[instruction.rs2] & 0xffffffff) >> 32; // Remove sign extension on unsigned values
							break;

						case 3 : // MULHU
							reg[instruction.rd] = ((uint64_t)reg[instruction.rs1] & 0xffffffff) * ((uint64_t)reg[instruction.rs2] & 0xffffffff) >> 32  ;
							break;

						case 4 : // DIV
							if (reg[instruction.rs2] == 0){ // Division by zero
								reg[instruction.rd] = -1; // Set all bits
							}
							else if (reg[instruction.rs1] == -2147483648 && reg[instruction.rs2] == -1){
								reg[instruction.rd] = -2147483648;	// (2^(32-1))
							}
							else{
								reg[instruction.rd] = reg[instruction.rs1] / reg[instruction.rs2];
							}
							break;

						case 5 : // DIVU
							if (reg[instruction.rs2] == 0){ // Division by zero
								reg[instruction.rd] = 4294967296 - 1; // Set all bits ((2^32) - 1)
								// TESTCASE wants result to be 0x0A Not all bits set ?? Manual says otherwise.
							}
							else{
								reg[instruction.rd] = (unsigned)reg[instruction.rs1] / (unsigned)reg[instruction.rs2];
							}
							break;

						case 6 : // REM
							if (reg[instruction.rs2] == 0){ // Division by zero
								reg[instruction.rd] = reg[instruction.rs1];
							}
							else if ((reg[instruction.rs1] == -2147483648) && (reg[instruction.rs2] == -1)){
								reg[instruction.rd] = 0;
							}
							else {
								reg[instruction.rd] = reg[instruction.rs1] % reg[instruction.rs2];
							}
							break;

						case 7 : // REMU
							if (reg[instruction.rs2] == 0){ // Division by zero
								reg[instruction.rd] = reg[instruction.rs1];
							}
							else{
								reg[instruction.rd] = (unsigned)reg[instruction.rs1] % (unsigned)reg[instruction.rs2];
							}
							break;
					}
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

char* VerboseInstruction(struct instr instruction, int pc){
	char *output_str;
	output_str = malloc(sizeof(char)*50);
	switch(instruction.opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(instruction.funct3){
				case 0x0 :// Load Byte
					snprintf(output_str, 50, "%#04x : LB x%d %d(x%d)", pc ,instruction.rd, instruction.Iimm, instruction.rs1);
					//printf("LB %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 :// Load Halfword
					snprintf(output_str, 50,"%#04x : LH x%d %d(x%d)", pc, instruction.rd, instruction.Iimm, instruction.rs1);
					break;
				case 0x2 :// Load Word
					snprintf(output_str, 50,"%#04x : LW x%d %d(x%d)", pc, instruction.rd, instruction.Iimm, instruction.rs1);
					break;
				case 0x4 : // Load Byte Unsigned
					snprintf(output_str, 50,"%#04x : LBU x%d %u(x%d)", pc, instruction.rd, instruction.Iimm, instruction.rs1);
					break;
				case 0x5 :// Load Half Unsigned
					snprintf(output_str, 50,"%#04x : LHU x%d %u(x%d)", pc, instruction.rd, instruction.Iimm, instruction.rs1);
					break;
				default :
					snprintf(output_str, 50,"%#04x : Invalid load function", pc);
					break;
			}
			break;
		// Stores
		case 0x23 :
			switch(instruction.funct3){
				case 0x0 : // Store Byte
					snprintf(output_str, 50,"%#04x : SB x%d %d(x%d)",pc, instruction.rs1, instruction.Simm, instruction.rs2);
					break;
				case 0x1 : // Store Halfword
					snprintf(output_str, 50,"%#04x : SH x%d %d(x%d)",pc, instruction.rs1, instruction.Simm, instruction.rs2);
					break;
				case 0x2 : // Store Word
					snprintf(output_str, 50,"%#04x : SW x%d %d(x%d)",pc, instruction.rs1, instruction.Simm, instruction.rs2);
					break;
				default :
					snprintf(output_str, 50,"%#04x : Invalid store function" , pc);
					break;
				}
			break;
		// I-Type
		case 0x13 :
			switch(instruction.funct3){
				case 0x0 : // Add Immediate
					snprintf(output_str, 50,"%#04x : ADDI x%d x%d %d",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 : // Śet < Immediate
					snprintf(output_str, 50,"%#04x : SLTI x%d x%d %d",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x3 : // Set < Immediate unsigned
					snprintf(output_str, 50,"%#04x : SLTIU x%d x%d %u", pc, instruction.rd, instruction.rs1, (unsigned)instruction.Iimm);
					break;
				case 0x4 : // XOR Immediate
					snprintf(output_str, 50,"%#04x : XORI x%d x%d %d", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x6 : // OR Immediate
					snprintf(output_str, 50,"%#04x : ORI x%d x%d %d", pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x7 : // AND Immediate
					snprintf(output_str, 50,"%#04x : ANDI x%d x%d %d",pc, instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 : // Shift left Immediate
					snprintf(output_str, 50,"%#04x : SLLI x%d x%d %d",pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							snprintf(output_str, 50,"%#04x : SRLI x%d x%d %d", pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 0x20 :
							snprintf(output_str, 50,"%#04x : SRAI x%d x%d %d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						default :
							snprintf(output_str, 50,"%#04x : Shift not valid",pc);
							break;
					}
					break;
				default :
					snprintf(output_str, 50,"%#04x : 0x13 funct3 invalid",pc);
					break;
			}
			break;
		case 0x33 :
			switch(instruction.funct7 &0x1){
				case 0x0 :
					switch(instruction.funct3){
					case 0x0 :
						switch(instruction.funct7){
							case 0x0 : // ADD
								snprintf(output_str, 50,"%#04x : ADD x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
								break;
							case 0x20 : // SUB
								snprintf(output_str, 50,"%#04x : SUB x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
								break;
							default :
								snprintf(output_str, 50,"%#04x : 0x33 funct7 invalid", pc);
								break;
							}
							break;
					case 0x1 : // SHift left
						snprintf(output_str, 50,"%#04x : SLL x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
					case 0x2 : // Set <
						snprintf(output_str, 50,"%#04x : SLT x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
					case 0x3 : // Set < Unsigned
						snprintf(output_str, 50,"%#04x : SLTU x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
					case 0x4 : // XOR
						snprintf(output_str, 50,"%#04x : XOR x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
					case 0x5 : // Shift right and Right Arithm
						switch(instruction.funct7){
							case 0x0 :
								snprintf(output_str, 50,"%#04x : SRL x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
							case 0x20 :
								snprintf(output_str, 50,"%#04x : SRA x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
								break;
							default :
								snprintf(output_str, 50,"%#04x : 0x33 funct7 invalid", pc);
								break;
						}
						break;
					case 0x6 : // OR
						snprintf(output_str, 50,"%#04x : OR x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
						break;
					case 0x7 : // AND
						snprintf(output_str, 50,"%#04x : AND x%d x%d x%d", pc, instruction.rd, instruction.rs1, instruction.rs2);
					break;
					default :
						snprintf(output_str, 50,"%#04x : 0x33 funct3 invalid", pc);
						break;
				}
				break;

				case 0x1 :
					switch(instruction.funct3){
						case 0 : // MUL
							snprintf(output_str, 50,"%#04x : MUL x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 1 : // MULH
							snprintf(output_str, 50,"%#04x : MULH x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 2 : // MULHSU
							snprintf(output_str, 50,"%#04x : MULHSU x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 3 : // MULHU
							snprintf(output_str, 50,"%#04x : MULHU x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 4 : // DIV
							snprintf(output_str, 50,"%#04x : DIV x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 5 : // DIVU
							snprintf(output_str, 50,"%#04x : DIVU x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 6 : // REM
							snprintf(output_str, 50,"%#04x : REM x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						case 7 : // REMU
							snprintf(output_str, 50,"%#04x : REMU x%d x%d x%d",pc, instruction.rd, instruction.rs1, instruction.rs2);
							break;
						default :
							snprintf(output_str, 50,"%#04x : Invalid MUL/DIV instruction",pc);
							break;
					}
					break;
			}
			break;
		case 0x63 : // Branch
			switch(instruction.funct3){
				case 0x0 : // Branch if Equal
					snprintf(output_str, 50,"%#04x : BEQ x%d x%d %d",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				case 0x1 : // Branch if not equal
					snprintf(output_str, 50,"%#04x : BNE x%d x%d %d",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				case 0x4 : // Branch less than
					snprintf(output_str, 50,"%#04x : BLT x%d x%d %d",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				case 0x5 : // Branch greater equal than
					snprintf(output_str, 50,"%#04x : BGE x%d x%d %d",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				case 0x6 : // Branch less than unsigned
					snprintf(output_str, 50,"%#04x : BLTU x%d x%d %u",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				case 0x7 : // Branch greater than unsigned					
					snprintf(output_str, 50,"%#04x : BGEU x%d x%d %u",pc, instruction.rs1, instruction.rs2, instruction.Bimm);
					break;
				default :
					snprintf(output_str, 50,"%#04x : Invalid branch instruction", pc);
					break;
			}
			break;
		case 0x6F : // Jump and Link
			snprintf(output_str, 50,"%#04x : JAL x%d %d", pc, instruction.rd, instruction.Jimm);
			break;
		case 0x67 : // Jump and link return
			snprintf(output_str, 50,"%#04x : JALR x%d x%d %d", pc, instruction.rd, instruction.rs1, instruction.Jimm);
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 :
					snprintf(output_str, 50,"%#04x : ECALL", pc);
					break;
				case 0x1 :
					snprintf(output_str, 50,"%#04x : EBREAK", pc);
					break;
				default :
					snprintf(output_str, 50,"%#04x : Invalid ECALL",pc);
					break;
			}
			break;
		case 0x37 :
			snprintf(output_str, 50,"%#04x : LUI x%d %d",pc, instruction.rd, instruction.Uimm);
			break;
		case 0x17 :
			snprintf(output_str,  50,"%#04x : AUIPC x%d %d",pc, instruction.rd, instruction.Uimm);
			break;

		default : // Unknown
			snprintf(output_str, 50,"%#04x : Opcode not implemented",pc);
			break;
	}
	return output_str;
}
