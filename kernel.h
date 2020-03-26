/*
 * 	kernel.h
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */
#ifndef KERNEL_H
#define KERNEL_H

#include "pcb.h"

typedef struct QUEUE_NODE_t {
    PCB *thisPCB;
    struct QUEUE_NODE_t *next;
}QUEUE_NODE;

extern QUEUE_NODE *head;
extern QUEUE_NODE *tail;

int boot();
int kernel();
int myinit(char* fileName);
void scheduler();
void addToReady(PCB *pcb);

#endif
