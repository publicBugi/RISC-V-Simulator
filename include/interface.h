#ifndef __INTERFACE_H_
#define __INTERFACE_H_
#include <stdio.h>

#define size(x)  (sizeof(x) / sizeof((x)[0]))
// NCURSES

void printRegister(int *ptr);

void testResult(int *reg1, int *reg2);

#endif