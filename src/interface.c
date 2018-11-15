#include "interface.h"

void printRegister(int *ptr){
	for (int i = 0; i < 8; i++){
		printf("%#03x : %#010x", i, *(ptr + i));
		printf(" %#03x : %#010x", i+8, *(ptr + i+8));
		printf(" %#03x: %#010x", i+16, *(ptr + i+16));
		printf(" %#03x : %#010x\n", i+24, *(ptr + i+24));		
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