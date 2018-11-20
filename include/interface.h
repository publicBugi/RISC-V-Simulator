#ifndef __INTERFACE_H_
#define __INTERFACE_H_
#include <stdio.h>
#include <curses.h>

#define size(x)  (sizeof(x) / sizeof((x)[0]))
// NCURSES

void wprintRegisters(int *reg, WINDOW *Window);

void wprintMemory(char *mem , WINDOW *W_Memory, int User_memPoint);

void wprintConsole(char** console, WINDOW *W_Console, int pc);

void wprintMachine(int *prog, WINDOW *W_Machine, int pc, int pc_max);

void wprintAssembly(char** assemblyCode, WINDOW *W_Assembly, int pc, int pc_max);

void printRegister(int *ptr);

void testResult(int *reg1, int *reg2);

#endif