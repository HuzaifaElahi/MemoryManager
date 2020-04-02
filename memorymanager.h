/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#include "pcb.h"

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

int launcher(FILE *p);
int resolvePageFault(PCB *pcb);
int deleteBackingStorageFile(int pid);

#endif /* MEMORYMANAGER_H_ */
