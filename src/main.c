#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define size(x)  (sizeof(x) / sizeof((x)[0]))

static unsigned int *pc;	// Program counter (Pointer to program array)
static int pc_max;
static int reg[32];
static int opcode = 0;
static int rd = 0;
static int funct3 = 0;
static int funct7 = 0;
static int rs1 = 0;
static int rs2 = 0;
static int Iimm = 0;
static int Simm = 0;
static int Bimm = 0;
static int Uimm = 0;
static int Jimm = 0;


void loadProgram(){
	FILE *ptr;	// File pointer
	ptr = fopen("/home/bugi/DTU/Digital/Computer Architecture/Final/shift.bin", "rb"); // Open File stream
	if (ptr != NULL){
		unsigned int buffer;	// Temporary buffer
		fseek(ptr, 0L, SEEK_END);	// Find end of file
		pc_max = ftell(ptr);		// Save size of file (maximum program counter)
		rewind(ptr);				// Go to start of file
		pc = (unsigned int*) malloc(pc_max);	// Allocate memory to
		for (int i = 0; i < pc_max; i++){		// Iterate through program, save to program array.
			fread(&buffer, sizeof(int), 1, ptr); // Fetch instruction from file.
			*pc = buffer;				/// Save instruction to array.
			//printf("%#010x\n", *pc); 	// Print instruction in hexadecimal.
			pc++;						// Increment program counter.
		}
		pc = pc - pc_max;			// Reset program counter.
		fclose(ptr);
	}
	else {
		printf("Unable to open file.");
	}
}






void RISCVExecute(){
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
					printf("Added %d and %d, value %d to register %d\n", Iimm, reg[rs1], reg[rd], rd);
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
					reg[rd] = reg[rs1] << rd;
					printf("Shifted register %d left by %d from %d to %d\n", rd, rd, rs1, reg[rd]);
					break;
				case 0x5 : // Shift right immediate & Aritm
					switch(Iimm){
						case 0x0 :
							reg[rd] = reg[rs1] >> rd;
							printf("Shifted register %d right by %d from %d to %d\n", rd, rd, rs1, reg[rd]);
							break;
						case 0x1 :
							reg[rd] = reg[rs1] >> rd; // WRONG
							printf("Arithmetic Shifted register %d right by %d from %d to %d\n", rd, rd, rs1, reg[rd]);
							break;
						default :
							printf("Funct3 not valid\n");
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
					switch(Iimm){
						case 0x0 : // ADD
							reg[rd] = reg[rs1] + reg[rs2];
							printf("Add register %d (%d) and %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
							break;
						case 0x1 : // SUB
							reg[rd] = reg[rs1] - reg[rs2]; // ??
							printf("Add register %d (%d) and %d (%d) to %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2], rd, reg[rd]);
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
					if (reg[rs1] < unsigned(reg[rs2])){
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
					switch(Iimm){
						case 0x0 :
							reg[rd] = reg[rs1] >> reg[rs2];
							printf("Shifted register %d right by %d from %d to %d\n", rd, rs2, rs1, reg[rd]);
							break;
						case 0x1 :
							reg[rd] = reg[rs1] >> reg[rs2]; //WRONG
							printf("Arithmetic Shifted register %d right by %d from %d to %d\n", rd, rs2, rs1, reg[rd]);
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
						pc = pc + Bimm;
					}
					printf("BEQ %d (%d) = %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x1 : // Branch if not equal
					if (reg[rs1] != reg[rs2]){
						pc = pc + Bimm;
					}
					printf("BNE %d (%d) != %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x4 : // Branch less than
					if (reg[rs1] < reg[rs2]){
						pc = pc + Bimm;
					}
					printf("BLT %d (%d) < %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x5 : // Branch greater equal than
					if (reg[rs1] >= reg[rs2]){
						pc = pc + Bimm;
					}
					printf("BEQ %d (%d) >= %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x6 : // Branch less than unsigned
					if (reg[rs1] < unsigned(reg[rs2])){
						pc = pc + Bimm;
					}
					printf("BEQ %d (%u) < %d (%u)\n", rs1, reg[rs1], rs2, reg[rs2]);
					break;
				case 0x7 : // Branch greater than unsigned
					if (reg[rs1] >= unsigned(reg[rs2])){
						pc = pc + Bimm;
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
			printf("LUI %d to register %d (%d)\n", Iimm, rd, reg[rd]);
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
	int pc_offset = 0;

	while(pc_offset < pc_max){
		// Fetch instruction
		int instruction = *(pc + pc_offset);
		// Decode
		opcode = 0;
		rd = 0;
		funct3 = 0;
		funct7 = 0;
		rs1 = 0;
		rs2 = 0;
		Iimm = 0;
		Simm = 0;
		Bimm = 0;
		Uimm = 0;
		Jimm = 0;

		// Execute
		RISCVExecute();
		// Memory access

		// Write back
		pc++;
		pc_offset++;
	}

}

int main(void) {

	loadProgram();
	SCRISCVProcessor();
	
}
/*void RISCVProcessor(){
	printf("Hello RISC-V World!\n");
	pc = 0;

		int instr = *pcptr + pc;
		int opcode = instr & 0x7f;
		int rd = (instr >> 7) & 0x01f;
		int funct3 = (instr >> 12) & 0x7;
		int rs1 = (instr >> 15) & 0x01f;
		int rs2 = (instr >> 20) & 0x01f;
		int Iimm = (instr >> 20); 	// I Type Immediate
		int Bimm = (instr >> 25); 	// B Type Immediate
		int simm = (instr >> 25) + ((instr >> 7) & 0x01f);	// S Type Immediate
		int rd = Iimm & 0x4;
		int alt = Iimm >> 10;

		
		++pc;
	}

	printf("Program exit\n");
}*/