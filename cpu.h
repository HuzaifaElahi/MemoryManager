/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#ifndef CPU_H
#define CPU_H

int runCPU(int);

typedef struct CPU_t {
    int IP;
    int offset;
    char IR[1000];
    int quanta;
} CPU;

extern CPU *cpu;

extern const int MAX_OFFSET;

#endif
