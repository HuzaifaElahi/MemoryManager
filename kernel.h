/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#ifndef KERNEL_H
#define KERNEL_H

#include "pcb.h"

typedef struct QUEUE_NODE_t {
    PCB *thisPCB;
    struct QUEUE_NODE_t *next;
} QUEUE_NODE;

extern QUEUE_NODE *head;
extern QUEUE_NODE *tail;

void scheduler();
void addToReady(PCB *pcb);

int boot();
int kernel();
int myinit(char* fileName);

#endif
