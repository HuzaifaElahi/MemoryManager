/*
 * 	pcb.h
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */
#ifndef PCB_H
#define PCB_H

typedef struct PCB_t {
    int PC;
    int start;
    int end;
    int pageTable[10];
    int PC_page;
    int PC_offest;
    int pages_max;
} PCB;

PCB* makePCB(int start, int end);

#endif
