/*
 * memorymanager.h
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

int launcher(FILE *p);

int countTotalPages(FILE *f);

void loadPage(int pageNumber, FILE *f, int frameNumber);

int findFrame();

int findVictim(PCB *p);

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);


#endif /* MEMORYMANAGER_H_ */