/*
 * 	pcb.h
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
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

#endif
