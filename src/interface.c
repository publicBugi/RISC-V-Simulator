#include "interface.h"

void wprintRegisters(int *reg, WINDOW *Window){
	for (int i = 0; i < 16; i++){
		mvwprintw(Window, i+1 , 3 ,"x%02d : %#010x", i, reg[i]);
		mvwprintw(Window, i+1 , 22 ,"x%02d : %#010x", i+16, reg[i+16]);
	}
}

void wprintMemory(unsigned char *mem , WINDOW *W_Memory, int User_memPoint){
	mvwprintw(W_Memory, 1, 2, "  Address");
	mvwprintw(W_Memory, 1, 17, "+0");
	mvwprintw(W_Memory, 1, 24, "+1");
	mvwprintw(W_Memory, 1, 31, "+2");
	mvwprintw(W_Memory, 1, 38, "+3");
	for (int i = 0; i < 39; i++){
		if((User_memPoint - (i*4) < 0)){
			mvwprintw(W_Memory, i+2 , 3 ,"-----------:");
			mvwprintw(W_Memory, i+2 , 17 ,"----");
			mvwprintw(W_Memory, i+2 , 24 ,"----");
			mvwprintw(W_Memory, i+2 , 31 ,"----");
			mvwprintw(W_Memory, i+2 , 38 ,"----");
		}
		else{
			mvwprintw(W_Memory, i+2 , 3 ,"%010x :", (User_memPoint-(i*4)));
			mvwprintw(W_Memory, i+2 , 17 ,"%.2x", (unsigned int)mem[User_memPoint-(i*4)+0]);
			mvwprintw(W_Memory, i+2 , 24 ,"%.2x", (unsigned int)mem[User_memPoint-(i*4)+1]);
			mvwprintw(W_Memory, i+2 , 31 ,"%.2x", (unsigned int)mem[User_memPoint-(i*4)+2]);
			mvwprintw(W_Memory, i+2 , 38 ,"%.2x", (unsigned int)mem[User_memPoint-(i*4)+3]);
		}
	}
}

void wprintConsole(char** console, WINDOW *W_Console, int pc){
	for (int i = 0; i < 10; i++){
		mvwprintw(W_Console, i+1, 2, console[pc + i]);
	}
}

void wprintMachine(int *prog, WINDOW *W_Machine, int pc, int pc_max){
	for (int i = 0; i < 46; i++){
		if((pc+i) < 0 || (pc+i) >= pc_max){
			mvwprintw(W_Machine, i+1, 2, "%-50s","---- : ----------");
		}
		else{
			mvwprintw(W_Machine, i+1, 2, "%#04x : %#010x", ((pc+i)*4), prog[pc+i]);
		}
	}
}

void wprintAssembly(char** assemblyCode, WINDOW *W_Assembly, int pc, int pc_max){
	for (int i = 0; i < 46; i++){
		if ((pc+i) < 0 || (pc+i) >= pc_max){
			mvwprintw(W_Assembly, i+1, 2, "%-50s","---- : ----------");
		}
		else {
			mvwprintw(W_Assembly, i+1, 2, "%-50s",*(assemblyCode+i+pc));
		}
	}
}


void printRegister(int *ptr){
	for (int i = 0; i < 8; i++){
		printf("x%02d : %#010x", i, *(ptr + i));
		printf(" x%02d : %#010x", i+8, *(ptr + i+8));
		printf(" x%02d: %#010x", i+16, *(ptr + i+16));
		printf(" x%02d : %#010x\n", i+24, *(ptr + i+24));		
	}
}

void testResult(int *reg1, int *reg2){
	int success = 1;
	for (int i = 4; i < 32; i++){
		if (*(reg1 + i) != *(reg2 + i)){
			printf("Test failure : %d\n", i);
			success = 0;
		}
	}
	if (success){
		printf("Test sucess!\n");
	}
}