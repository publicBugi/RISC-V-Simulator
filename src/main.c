#include <string.h>
#include <stdio.h>

#define size(x)  (sizeof(x) / sizeof((x)[0]))

static int pc;
static int reg[64];
static int progr[3] = {
	// As minimal RISC-V assembler example
	0x00200093, // addi x1 x0 2
	0x00300113, // addi x2 x0 3
	0x002081b3, // add x3 x1 x2
};
size_t progrLength = size(progr);

int main(void) {
	printf("Hello RISC-V World!\n");<

	pc = 0;

	while(pc < progrLength){
		int instr = progr[pc];
		int opcode = instr & 0x7f;
		int rd = (instr >> 7) & 0x01f;
		int funct3 = (instr >> 12) & 0xf;
		int rs1 = (instr >> 15) & 0x01f;
		int imm = (instr >> 20);
		int shamt = imm & 0x4;

		switch(opcode){
			
			//// RV32I
			// Loads
			case 0x3 :
				switch(funct3){
					case 0x0 :// Load Byte
						printf("Loaded %d from %d(%d) to register %d\n", reg[rd], imm, rs1, rd);
						break;
					case 0x1 :// Load Halfword
						printf("Loaded %d from %d(%d) to register %d\n", reg[rd], imm, rs1, rd);
						break;
					case 0x2 :// Load Word
						printf("Loaded %d from %d(%d) to register %d\n", reg[rd], imm, rs1, rd);
						break;
					case 0x4 : // Load Byte Unsigned
						printf("Loaded %d from %d(%d) to register %d\n", reg[rd], imm, rs1, rd);
						break;
					case 0x5 :// Load Half Unsigned
						printf("Loaded %d from %d(%d) to register %d\n", reg[rd], imm, rs1, rd);
						break;
				}
				break;

			// Stores
			case 0x23 :
				switch(funct3){
				case 0x0 : // Store Byte
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], imm, rs1, rd);
					break;
				case 0x1 : // Store Halfword
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], imm, rs1, rd);
					break;
				case 0x2 : // Store Word
					printf("Stored %d to %d(%d) from register %d\n", reg[rd], imm, rs1, rd);
					break;
				}
				break;
			
			// I-Type
			case 0x13 :
				switch(funct3){
					case 0x0 : // Add Immediate
						printf("Added %d and %d, value %d to register %d\n", imm, rs1, reg[rd], rd);
						break;
					case 0x2 : // Śet < Immediate
						printf("Compare %d < %d from register %d, register %d set to %d\n", rs1, imm, rd, reg[rd]);
						break;
					case 0x3 : // Set < Immediate unsigned
						printf("Compare %d < %u from register %d, register %d set to %d\n", rs1, imm, rd, reg[rd]);
						break;
					case 0x4 : // XOR Immediate
						printf("XOR register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x6 : // OR Immediate
						printf("OR register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x7 : // AND Immediate
						printf("And register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x1 : // Shift left Immediate
						printf("Shifted register %d left by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
						break;
					case 0x5 : // Shift right immediate & Aritm
						switch(imm >> 10 == 0){
							case 0x0 :
								printf("Shifted register %d right by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
								break;
							case 0x1 :
								printf("Arithmetic Shifted register %d right by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
								break;
						}
						break;
				}
				break;

			case 0x33 :
				switch(funct3){
					case 0x0 : // ADD
						//printf("Add register %d and %d to %d, from")
						break;
				}
				break;


//*			case 0x13: 	// Add Immediate
//				reg[rd] = reg[rs1] + imm;
//				printf("%d to register %d\n\n",reg[rd], rd);
//				break;
//			default :
//				printf("Opcode: %d not implemented yet\n\n", opcode);
//				break;
		}
		++pc;
	}

	printf("Program exit");
}