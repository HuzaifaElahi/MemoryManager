/*
 * memorymanager.h
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 */

#include "pcb.h"

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

int launcher(FILE *p);

int countTotalPages(FILE *f);

int loadPage(int pageNumber, FILE *f, int frameNumber);

int findFrame();

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);

int findVictim(PCB *p);

int resolvePageFault(PCB *p);


#endif /* MEMORYMANAGER_H_ */
