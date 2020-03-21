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
} PCB;

PCB* makePCB(int start, int end);

#endif
