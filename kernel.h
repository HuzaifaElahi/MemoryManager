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

struct QUEUE_NODE {
    PCB *thisPCB;
    struct QUEUE_NODE *next;
};

extern struct QUEUE_NODE *head;
extern struct QUEUE_NODE *tail;

int boot();
int kernel();
int myinit(char* fileName);
void scheduler();

#endif
