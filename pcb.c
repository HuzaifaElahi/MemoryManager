/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#include <stdio.h>
#include <stdlib.h>

#include "pcb.h"

// Create and Return PCB
PCB* makePCB(int pid, int pages_max, int lines_max){
    PCB* newPCB = malloc(sizeof(struct PCB_t));
    newPCB->pid = pid;
    newPCB->lines_max = lines_max;
    newPCB->pages_max = pages_max;
    newPCB->PC_offset = 0;
    newPCB->PC_page = 0;
    for(int i = 0; i < 10; i++) newPCB->pageTable[i] =- 1;

    return newPCB;
}
