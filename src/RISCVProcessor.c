#include "RISCVProcessor.h"

struct instr InstructionDecode(int MachineInstr){
		struct instr instruction;

		instruction.opcode = MachineInstr & 0x7f;
		instruction.rd = (MachineInstr >> 7) & 0x01f;
		instruction.funct3 = (MachineInstr >> 12) & 0x7;
		instruction.funct7 = MachineInstr >> 25;
		instruction.funct2 = (MachineInstr >> 25) & 0x2;
		instruction.rs1 = (MachineInstr >> 15) & 0x1f;
		instruction.rs2 = (MachineInstr >> 20) & 0x1f;
		instruction.rs3 = (MachineInstr >> 27);
		instruction.Iimm = MachineInstr >> 20;
		instruction.Simm = (MachineInstr >> 25) | ((MachineInstr >> 7) & 0x1F);
		instruction.Bimm = ((MachineInstr >> 31) << 12) | (((MachineInstr >> 7) & 0x1) << 11) | (((MachineInstr >> 25) & 0x3f) << 5) | (((MachineInstr >> 8) & 0x3f) << 1);
		instruction.Uimm = (MachineInstr & 0xfffff000);
		instruction.Jimm = (((MachineInstr >> 31) << 20) | (((MachineInstr >> 21) & 0x3ff) << 1) | (((MachineInstr >> 20) & 0x1) << 11) | (((MachineInstr >> 11) & 0xFF) << 12));
		//instruction.Jimm = ((MachineInstr >> 31) << 20) | (((MachineInstr >> 21) & 0x3FF ) << 1)| (((MachineInstr >> 20) 0x01) << 11)|(((MachineInstr >> 11) & 0xFF) << 12);
		return instruction;

}

void SingleCycleStep(int reg[], char *mem, struct instr instruction, int *pc){
	switch(instruction.opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(instruction.funct3){
				case 0x0 :// Load Byte
					reg[instruction.rd] = instruction.Iimm + instruction.rs1;
					printf("LB %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 :// Load Halfword
					reg[instruction.rd] = instruction.Iimm + instruction.rs1;
					reg[instruction.rd] += instruction.Iimm + instruction.rs1 + 1;
					printf("LH %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x2 :// Load Word
					reg[instruction.rd] = instruction.Iimm + instruction.rs1;
					reg[instruction.rd] += instruction.Iimm + instruction.rs1 + 1;
					reg[instruction.rd] += instruction.Iimm + instruction.rs1 + 2;
					reg[instruction.rd] += instruction.Iimm + instruction.rs1 + 3;
					printf("LW %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x4 : // Load Byte Unsigned
					reg[instruction.rd] = (unsigned)(instruction.Iimm + instruction.rs1);
					printf("LBU %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x5 :// Load Half Unsigned
					reg[instruction.rd] = (unsigned)(instruction.Iimm + instruction.rs1);
					reg[instruction.rd] += (unsigned)(instruction.Iimm + instruction.rs1 + 1);
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
					mem[instruction.rs1 + instruction.Simm] = instruction.rs2 & 0x000000ff;
					printf("SB %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x1 : // Store Halfword
					mem[instruction.rs1 + instruction.Simm] = instruction.rs2 & 0x00ff;
					mem[instruction.rs1 + instruction.Simm + 1] = (instruction.rs2 & 0xff00) >> 8;
					printf("SB %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
					break;
				case 0x2 : // Store Word
					mem[instruction.rs1 + instruction.Simm] = instruction.rs2 & 0x000000ff;
					mem[instruction.rs1 + instruction.Simm + 1] = (instruction.rs2 & 0x0000ff00) >> 8;
					mem[instruction.rs1 + instruction.Simm + 2] = (instruction.rs2 & 0x00ff0000) >> 16;
					mem[instruction.rs1 + instruction.Simm + 3] = (instruction.rs2 & 0xff000000) >> 24;
					printf("SB %#04x, %#010x, %#010x\n", instruction.rs1, instruction.rs2, instruction.Simm);
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
					reg[instruction.rd] = reg[instruction.rs1] + instruction.Iimm;
					printf("ADDI %#04x , %#010x , %#010x\n", instruction.rd, reg[instruction.rs1], instruction.Iimm);
					break;
				case 0x2 : // Śet < Immediate
					if(reg[instruction.rs1] < instruction.Iimm){
						reg[instruction.rd] = 1;
					}
					else{
						reg[instruction.rd] = 0;
					}
					printf("SLTI %#04x, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.Iimm);
					break;
				case 0x3 : // Set < Immediate unsigned
					if(reg[instruction.rs1] < (unsigned)instruction.Iimm){
						reg[instruction.rd] = 1;
					}
					else{
						reg[instruction.rd] = 0;
					}
					printf("SLTIU %#04x, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], (unsigned)instruction.Iimm);
					break;
				case 0x4 : // XOR Immediate
					reg[instruction.rd] = reg[instruction.rs1] ^ instruction.Iimm;
					printf("XORI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x6 : // OR Immediate
					reg[instruction.rd] = reg[instruction.rs1] | instruction.Iimm;
					printf("ORI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x7 : // AND Immediate
					reg[instruction.rd] = reg[instruction.rs1] & instruction.Iimm;
					printf("ANDI %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.Iimm);
					break;
				case 0x1 : // Shift left Immediate
					reg[instruction.rd] = reg[instruction.rs1] << instruction.rs2;
					printf("SLLI %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.rs2);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(instruction.funct7){
						case 0x0 :
							reg[instruction.rd] = reg[instruction.rs1] >> instruction.rs2;
							printf("SRLI %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], instruction.rs2);
							break;
						case 0x20 :
							reg[instruction.rd] = reg[instruction.rs1] >> instruction.rs2;
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
							reg[instruction.rd] = reg[instruction.rs1] + reg[instruction.rs2];
							printf("ADD %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], reg[instruction.rs2]);
							break;
						case 0x20 : // SUB
							reg[instruction.rd] = reg[instruction.rs1] - reg[instruction.rs2]; // ??
							printf("SUB %02d, %#010x, %#010x\n", instruction.rd, reg[instruction.rs1], reg[instruction.rs2]);
							break;
						default :
							printf("Funct not implemented\n");
							break;
						}
						break;
				case 0x1 : // SHift left
					reg[instruction.rd] = reg[instruction.rs1] << reg[instruction.rs2];
					printf("SLL %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x2 : // Set <
					if (reg[instruction.rs1] < reg[instruction.rs2]){
						reg[instruction.rd] = 1;
					}
					else {
						reg[instruction.rd] = 0;
					}
					break;
					printf("SLT %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
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
					printf("XOR %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x5 : // Shift right and Right Arithm
					switch(instruction.funct7){
						case 0x0 :
							reg[instruction.rd] = (unsigned)reg[instruction.rs1] >> reg[instruction.rs2];
							printf("SRL %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
						break;
						case 0x20 :
							reg[instruction.rd] = reg[instruction.rs1] >> reg[instruction.rs2]; //WRONG
							printf("SRA %02d, %#010x, %#010x\n", instruction.rd, instruction.rs2, instruction.rs1);
							break;
						default :
							printf("Funct3 not valid\n");
							break;
					}
					break;
				case 0x6 : // OR
					reg[instruction.rd] = reg[instruction.rs1] | reg[instruction.rs2];
					printf("OR %#04x, %#010x, %#010x\n", instruction.rd, instruction.rs1, instruction.rs2);
					break;
				case 0x7 : // AND
					reg[instruction.rd] = reg[instruction.rs1] & reg[instruction.rs2];
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
					if (reg[instruction.rs1] == reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BEQ %d (%d) = %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x1 : // Branch if not equal
					if (reg[instruction.rs1] != reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BNE %d (%d) != %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x4 : // Branch less than
					if (reg[instruction.rs1] < reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BLT %d (%d) < %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x5 : // Branch greater equal than
					if (reg[instruction.rs1] >= reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BEQ %d (%d) >= %d (%d)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x6 : // Branch less than unsigned
					if (reg[instruction.rs1] < (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BEQ %d (%u) < %d (%u)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				case 0x7 : // Branch greater than unsigned
					if (reg[instruction.rs1] >= (unsigned)reg[instruction.rs2]){
						*pc = *pc + instruction.Bimm / 4 - 1;
					}
					printf("BEQ %d (%u) >= %d (%u)\n", instruction.rs1, reg[instruction.rs1], instruction.rs2, reg[instruction.rs2]);
					break;
				default :
					printf("Invalid branch instruction\n");
					break;
			}
			break;
		case 0x6F : // Jump and Link
			reg[instruction.rd] = *pc;
			*pc = *pc + instruction.Jimm / 4;
			printf("JAL %#010x\n", *pc);
			break;
		case 0x67 :
			*pc = *pc + reg[instruction.rd];
			printf("JALR %#010x\n", *pc);
			break;
		case 0x73 : //ECALL / EBREAK
			switch(instruction.Iimm){
				case 0x0 :
					printf("%d", reg[11]);
					switch(reg[11]){
						case 10 :
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
			reg[instruction.rd] = instruction.Uimm;
			printf("LUI %#04x, %#010x\n", instruction.rd, instruction.Uimm);
			break;
		case 0x17 :
			*pc = *pc + instruction.Uimm;
			printf("AUIPC %#010x, %#010x\n", instruction.Uimm, *pc);
			break;
		default : // Unknown
			printf("Opcode not implemented!\n");
			break;
	}
}



/*void RISCVExecute(){
	switch(opcode){	
		//// RV32I
		// Loads
		case 0x3 :
			switch(funct3){
				case 0x0 :// Load Byte
					printf("Loaded %d from %d(%d) to register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x1 :// Load Halfword
					printf("Loaded %d from %d(%d) to register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x2 :// Load Word
					printf("Loaded %d from %d(%d) to register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x4 : // Load Byte Unsigned
					printf("Loaded %d from %d(%d) to register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x5 :// Load Half Unsigned
					printf("Loaded %d from %d(%d) to register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				default :
					printf("Funct3 not implemented!\n");
					break;
			}
			break;
		// Stores
		case 0x23 :
			switch(funct3){
				case 0x0 : // Store Byte
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x1 : // Store Halfword
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				case 0x2 : // Store Word
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], Iimm, rs1, rd);
					break;
				default :
					printf("Funct3 not implemented!\n");
					break;
				}
			break;
		// I-Type
		case 0x13 :
			switch(funct3){
				case 0x0 : // Add Immediate
					reg[rd] = reg[rs1] + Iimm;
					printf("ADDI %#04x , %#010x , %#010x\n", rd, reg[rs1], Iimm);
					break;
				case 0x2 : // Śet < Immediate
					if(reg[rs1] < Iimm){
						reg[rd] = 1;
					}
					else{
						reg[rd] = 0;
					}
					printf("Compare register %d (%d) < %d , register %d set to %d\n", rs1, reg[rs1], Iimm, rd, reg[rd]);
					break;
				case 0x3 : // Set < Immediate unsigned
					if(reg[rs1] < unsigned(Iimm)){
						reg[rd] = 1;
					}
					else{
						reg[rd] = 0;
					}
					printf("Compare register %d (%d) < %u , register %d set to %d\n", rs1, reg[rs1], Iimm, rd, reg[rd]);
					break;
				case 0x4 : // XOR Immediate
					reg[rd] = reg[rs1] ^ Iimm;
					printf("XOR register %d with %d to register %d from %d to %d\n", rs1, Iimm, rd, reg[rs1], reg[rd]);
					break;
				case 0x6 : // OR Immediate
					reg[rd] = reg[rs1] | Iimm;
					printf("OR register %d with %d to register %d from %d to %d\n", rs1, Iimm, rd, reg[rs1], reg[rd]);
					break;
				case 0x7 : // AND Immediate
					reg[rd] = reg[rs1] & Iimm;
					printf("And register %d with %d to register %d from %d to %d\n", rs1, Iimm, rd, reg[rs1], reg[rd]);
					break;
				case 0x1 : // Shift left Immediate
					reg[rd] = reg[rs1] << rs2;
					printf("SLLI x%#02d, %#010x, %#010x\n", rd, reg[rs1], rs2);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(funct7){
						case 0x0 :
							reg[rd] = reg[rs1] >> rs2;
							printf("SRLI x%#02d, %#010x, %#010x\n", rd, reg[rs1], rs2);
							break;
						case 0x20 :
							reg[rd] = reg[rs1] >> rs2;
							printf("SRAI x%#02d, %#010x , %#010x\n", rd, reg[rs1], rs2);
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
			switch(funct3){
				case 0x0 :
					switch(funct7){
						case 0x0 : // ADD
							reg[rd] = reg[rs1] + reg[rs2];
							printf("ADD x%02d, %#010x, %#010x\n", rd, reg[rs1], reg[rs2]);
							break;
						case 0x20 : // SUB
							reg[rd] = reg[rs1] - reg[rs2]; // ??
							printf("SUB x%02d, %#010x, %#010x\n", rd, reg[rs1], reg[rs2]);
							break;
						default :
							printf("Funct not implemented\n");
							break;
						}
						break;
				case 0x1 : // SHift left
					reg[rd] = reg[rs1] << reg[rs2];
					printf("Shifted register %d (%d) by %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				case 0x2 : // Set <
					if (reg[rs1] < reg[rs2]){
						reg[rd] = 1;
					}
					else {
						reg[rd] = 0;
					}
					printf("Compare %d (%d) < %d (%d), register %d set to %d\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				case 0x3 : // Set < Unsigned
					if (reg[rs1] < (unsigned)reg[rs2]){
						reg[rd] = 1;
					}
					else {
						reg[rd] = 0;
					}
					printf("Compare %d (%d) < %d (%u), register %d set to %d\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				case 0x4 : // XOR
					reg[rd] = reg[rs1] ^ reg[rs2];
					printf("XOR register %d (%d) with %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				case 0x5 : // Shift right and Right Arithm
					switch(funct7){
						case 0x0 :
							reg[rd] = (unsigned)reg[rs1] >> reg[rs2];
							printf("Shifted register %d right by %d from %d to %d\n", rd, rs2, rs1, reg[rd]);
							break;
						case 0x20 :
							reg[rd] = reg[rs1] >> reg[rs2]; //WRONG
							printf("SRA", rd, rs2, rs1, reg[rd]);
							break;
						default :
							printf("Funct3 not valid\n");
							break;
					}
					break;
				case 0x6 : // OR
					reg[rd] = reg[rs1] | reg[rs2];
					printf("OR register %d (%d) with %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				case 0x7 : // AND
					reg[rd] = reg[rs1] & reg[rs2];
					printf("AND register %d (%d) with %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
					break;
				default :
					printf("Funct3 not implemented\n");
					break;
			}
			break;
		case 0x63 : // Branch
			switch(funct3){
				case 0x0 : // Branch if Equal
					if (reg[rs1] = reg[rs2]){
						pc_offset = pc_offset + Bimm;
					}
					printf("BEQ %d (%d) = %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x1 : // Branch if not equal
					if (reg[rs1] != reg[rs2]){
						pc_offset = pc_offset + Bimm;
					}
					printf("BNE %d (%d) != %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x4 : // Branch less than
					if (reg[rs1] < reg[rs2]){
						pc_offset = pc_offset + Bimm;
					}
					printf("BLT %d (%d) < %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x5 : // Branch greater equal than
					if (reg[rs1] >= reg[rs2]){
						pc_offset = pc_offset + Bimm;
					}
					printf("BEQ %d (%d) >= %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x6 : // Branch less than unsigned
					if (reg[rs1] < unsigned(reg[rs2])){
						pc_offset = pc_offset + Bimm;
					}
					printf("BEQ %d (%u) < %d (%u)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x7 : // Branch greater than unsigned
					if (reg[rs1] >= unsigned(reg[rs2])){
						pc_offset = pc_offset + Bimm;
					}
					printf("BEQ %d (%u) >= %d (%u)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				default :
					printf("Invalid branch instruction\n");
					break;
			}
			break;
		case 0x6F : // Jump and Link
			printf("Jump and link\n");
			break;
		case 0x67 :
			printf("Jump and link register\n");
			break;
		case 0x73 : //ECALL / EBREAK
			switch(Iimm){
				case 0x0 :
					printf("ECALL\n");
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
			reg[rd] = Uimm;
			printf("LUI %#010x to Reg %#04x (%#010x)\n", Uimm, rd, reg[rd]);
			break;
		case 0x17 :
			printf("AUI to PC %d, %d\n", Iimm, pc);
			break;
		default : // Unknown
			printf("Opcode not implemented!\n");
			break;
		}
	}

void SCRISCVProcessor() // Single cycle RISC-V Processor
{
	

	while(pc_offset < pc_max){
		// Fetch instruction
		int instruction = *(pc + pc_offset);
		// Decode
		opcode = instruction & 0x7f;
		rd = (instruction >> 7) & 0x01f;
		funct3 = (instruction >> 12) & 0x7;
		funct7 = instruction >> 25;
		funct2 = (instruction >> 25) & 0x2;
		rs1 = (instruction >> 15) & 0x1f;
		rs2 = (instruction >> 20) & 0x1f;
		rs3 = (instruction >> 27);
		Iimm = instruction >> 20;
		Simm = (instruction >> 25) | ((instruction >> 7) & 0x1F);
		Bimm = ((instruction >> 31) << 12) | (((instruction >> 7) & 0x1) << 11) | (((instruction >> 25) & 0x3f) << 5) | (((instruction >> 8) & 0xf) << 1); 
		//Bimm = ((instruction >> 31) << 12) | (((instruction >> 7) & 0x1) << 11) | (((instruction >> 25) & 0x3f) << 5) | (((instruction >> 8) & 0x3f) << 1);
		Uimm = (instruction & 0xfffff000);
		Jimm = ((instruction >> 31) << 20) | (((instruction >> (12)) & 0x1FF ) << 1)| (instruction >> 11)|(((instruction >> 11) & 0x7F) << 12) ;
		printf("%#010x\n", Bimm);
		// Execute
		printf("%d : ", pc_offset);
		RISCVExecute();
		// Memory access

		// Write back
		pc_offset++;
	}

}*/