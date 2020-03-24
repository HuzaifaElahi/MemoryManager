/*
 * memorymanager.c
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memorymanager.h"
#include "pcb.h"
#include "ram.h"

char *ram[40];
int pid=0;

int findVictim(PCB *p){
	srand(time(0));
	int victimIndex = rand()%40;
	printf("%d", victimIndex);
	return 0;

}
//
//int victimExistsInPCB(PCB *p){
//	p->
//	return 0;
//}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	if(victimFrame!=0){
//		PCB victim = findVictimPCB();
	}

	p->pageTable[pageNumber] = frameNumber;
	return 0;

}

int createBackingStorageFile(char** file){
	char filePath[100] = "BackingStore/";
	char fileName[100];
	sprintf(fileName, "%d", pid);
	strcat(filePath, fileName);
	strcat(filePath, ".txt");
	char touch_cmd[100];
	strcpy(touch_cmd, "touch ");
	strcat(touch_cmd, filePath);
	system(touch_cmd);
	*file = strdup(filePath);
	return 0;
}

int copyIntoBackingFile(FILE* fptr1, FILE* fptr2){
    char c = fgetc(fptr1);
    while (!feof(fptr1))
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
    fclose(fptr1);
    fclose(fptr2);
    return 0;
}

int launchPaging(){
	return 0;
}

int launcher(FILE *fptr1){
	char* file;
	int error=0;
	createBackingStorageFile(&file);
	FILE *fptr2 = fopen(file, "w");
	copyIntoBackingFile(fptr1, fptr2);
	FILE *f = fopen(file, "r");
	int totalPages = countTotalPages(f);
	printf("total pages:%d\n", totalPages);

    PCB* pcb = makePCB(0, 0);
    pcb->pages_max = totalPages;

    int enableFindVictim=0;
	int frameNumber = findFrame();
	if(frameNumber == -1)	{
		frameNumber = findVictim(pcb);
		enableFindVictim=1;
	}
	printf("1st frame:%d\n", frameNumber);
	error = loadPage(0, f, frameNumber);
	if(error!=0)
		return error;

	updatePageTable(pcb, 0, frameNumber, enableFindVictim);

	if(totalPages>1){
		frameNumber = findFrame();
		if(frameNumber == -1)	{
			frameNumber = findVictim(pcb);
			enableFindVictim=1;
		}
		error = loadPage(1, f, frameNumber);
		if(error!=0)
			return error;
		printf("%d", pcb->pageTable[1]);
		updatePageTable(pcb, 1, frameNumber, enableFindVictim);
	}
	return 0;
}

int countTotalPages(FILE *f){
	int lines=0;
	int pageSize = 4;
    while(!feof(f)){
        char *line = NULL;
        size_t linecap = 0;
        getline(&line, &linecap, f);
    	lines++;
    }
    int pageCount = lines/pageSize;
    if(lines%pageSize != 0)
    	pageCount++;

    return pageCount;
}

int loadPage(int pageNumber, FILE *f, int frameNumber){
	return 0;
}

int findFrame(){
	for(int i=0; i<37; i+=4){
		if(ram[i]==NULL && ram[i+1]==NULL && ram[i+2]==NULL && ram[i+3]==NULL){
			return i;
		}
	}
	return -1;
}


