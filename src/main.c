#include <stdio.h>
#include <string.h>


#define size(x)  (sizeof(x) / sizeof((x)[0]))

static int pc;
static int reg[64];

unsigned int buffer;

FILE *ptr;

int main(void) {

	ptr = fopen("/home/bugi/DTU/Digital/Computer Architecture/Final/shift.bin", "rb");
	fseek(ptr, 0L, SEEK_END);
	int pc_max = ftell(ptr);
	rewind(ptr);
	int progr[pc_max];
	size_t progrLength = size(progr);
	if (ptr == NULL){
		printf("Error! Unable to open file\n");
		return(1);
	}
	for (int i = 0; i < pc_max; i++){
		fread(&buffer, sizeof(int), 1, ptr);
		progr[i] = buffer;
		printf("%#010x\n", buffer);
	}

	printf("Hello RISC-V World!\n");
	pc = 0;

	while(pc < progrLength){


		int instr = progr[pc];
		int opcode = instr & 0x7f;
		int rd = (instr >> 7) & 0x01f;
		int funct3 = (instr >> 12) & 0x7;
		int rs1 = (instr >> 15) & 0x01f;
		int rs2 = (instr >> 20) & 0x01f;
		int imm1 = rd;
		int imm2 = 1;
		int imm3 = 1;
		int imm = (instr >> 20);
		int shamt = imm & 0x4;
		int alt = imm >> 10;

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
					default :
						printf("Funct3 not implemented!\n");
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
				default :
					printf("Funct3 not implemented!\n");
					break;
				}
				break;
			
			// I-Type
			case 0x13 :
				switch(funct3){
					case 0x0 : // Add Immediate
						reg[rd] = reg[rs1] + imm;
						printf("Added %d and %d, value %d to register %d\n", imm, reg[rs1], reg[rd], rd);
						break;
					case 0x2 : // Śet < Immediate
						if(reg[rs1] < imm){
							reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						printf("Compare register %d (%d) < %d , register %d set to %d\n", rs1, reg[rs1], imm, rd, reg[rd]);
						break;
					case 0x3 : // Set < Immediate unsigned
						if(reg[rs1] < unsigned(imm)){
							reg[rd] = 1;
						}
						else{
							reg[rd] = 0;
						}
						printf("Compare register %d (%d) < %u , register %d set to %d\n", rs1, reg[rs1], imm, rd, reg[rd]);
						break;
					case 0x4 : // XOR Immediate
						reg[rd] = reg[rs1] ^ imm;
						printf("XOR register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x6 : // OR Immediate
						reg[rd] = reg[rs1] | imm;
						printf("OR register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x7 : // AND Immediate
						reg[rd] = reg[rs1] & imm;
						printf("And register %d with %d to register %d from %d to %d\n", rs1, imm, rd, reg[rs1], reg[rd]);
						break;
					case 0x1 : // Shift left Immediate
						reg[rd] = reg[rs1] << shamt;
						printf("Shifted register %d left by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
						break;
					case 0x5 : // Shift right immediate & Aritm
						switch(alt){
							case 0x0 :
								reg[rd] = reg[rs1] >> shamt;
								printf("Shifted register %d right by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
								break;
							case 0x1 :
								reg[rd] = reg[rs1] >> shamt; // WRONG
								printf("Arithmetic Shifted register %d right by %d from %d to %d\n", rd, shamt, rs1, reg[rd]);
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
						switch(alt){
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
						switch(alt){
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
						if (reg[rs1 = reg[rs2]]){
							pc = imm;
						}
						printf("BEQ %d (%d) = %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
						break;
					case 0x1 : // Branch if not equal
						printf("BNE %d (%d) != %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
						break;
					case 0x4 : // Branch less than
						printf("BLT %d (%d) < %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
						break;
					case 0x5 : // Branch greater equal than
						printf("BEQ %d (%d) >= %d (%d)\n", rs1, reg[rs1], rs2, reg[rs2]);
						break;
					case 0x6 : // Branch less than unsigned
						printf("BEQ %d (%u) < %d (%u)\n", rs1, reg[rs1], rs2, reg[rs2]);
						break;
					case 0x7 : // Branch greater than unsigned
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
				switch(imm){
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
				printf("LUI %d to register %d (%d)\n", imm, rd, reg[rd]);
				break;
			case 0x17 :
				printf("AUI to PC %d, %d\n", imm, pc);
				break;

			default : // Unknown
				printf("Opcode not implemented!\n");
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

	printf("Program exit\n");
}