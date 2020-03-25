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
	int pid;
	int max_lines;
    int PC;
    int pageTable[10];
    int PC_page;
    int PC_offset;
    int pages_max;
} PCB;

PCB* makePCB();

#endif
