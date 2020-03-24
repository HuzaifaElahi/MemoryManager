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
//#include "kernel.h"

char *ram[40];
int generated_pid=0;

int constructFilePathFromPID(char** file, int pid){
	char filePath[100] = "BackingStore/";
	char fileName[100];
	sprintf(fileName, "%d", pid);
	strcat(filePath, fileName);
	strcat(filePath, ".txt");
	*file = strdup(filePath);
	return 0;
}

int createBackingStorageFile(char** file){
	char* filePath;
	constructFilePathFromPID(&filePath, generated_pid);
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

int launchPaging(PCB *pcb, FILE* f, int totalPages){
	int error=0;
    int enableFindVictim=0;
	int frameNumber = findFrame();
	if(frameNumber == -1)	{
		frameNumber = findVictim(pcb);
		enableFindVictim=1;
	}
	error = loadPage(0, f, frameNumber);
	if(error!=0)
		return error;
	updatePageTable(pcb, 0, frameNumber, enableFindVictim);
	printf("1st frame:%d\n", pcb->pageTable[0]);

	if(totalPages>1){
		frameNumber = findFrame();
		if(frameNumber == -1)	{
			frameNumber = findVictim(pcb);
			enableFindVictim=1;
		}
		error = loadPage(1, f, frameNumber);
		if(error!=0)
			return error;
		updatePageTable(pcb, 1, frameNumber, enableFindVictim);
		printf("2nd frame:%d\n", pcb->pageTable[1]);
	}
	return error;
}


int victimExistsInPCB(PCB *p, int index){
	size_t length = sizeof(p->pageTable)/sizeof(int);
	for(int i=0; i<length;i++){
		if(p->pageTable[i]==index)
			return 1;
	}
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

    error= launchPaging(pcb, f, totalPages);

    generated_pid++;
	return error;
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
	fseek(f, 0, SEEK_SET);
	int lineTarget = pageNumber*4;
	int lineCount=0;
    int offset=0;
    while(!feof(f)){
        char *line = NULL;
        size_t linecap = 0;
        getline(&line, &linecap, f);
    	if(lineCount>=lineTarget && lineCount<lineTarget+4){
    		ram[frameNumber*4+offset]=strdup(line);
    		offset++;
    	}
    	lineCount++;
    }
	return 0;
}

int findFrame(){
	for(int i=0; i<37; i+=4){
		if(ram[i]==NULL && ram[i+1]==NULL && ram[i+2]==NULL && ram[i+3]==NULL){
			return i/4;
		}
	}
	return -1;
}

int findVictim(PCB *p){
	printf("find victim\n");
	srand(time(0));
	int victimIndex = ((rand()%40)/4)*4;
	printf("random index is:%d\n", victimIndex);
	while(victimExistsInPCB(p, victimIndex)){
		victimIndex=(victimIndex+4)%40;
	}
	printf("final victim Index:%d\n", victimIndex);
	return victimIndex;
}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	if(victimFrame!=0){
		//TODO
		//	PCB victim = findVictimPCB();
	}

	p->pageTable[pageNumber] = frameNumber;
	return 0;
}

int resolvePageFault(PCB *pcb){
	int error=0;
	int enableFindVictim=0;
	(pcb->PC_page)++;
	if(pcb->PC_page>=pcb->pages_max)
		return 0;
	//check if frame exists in ram
	if(pcb->pageTable[pcb->PC_page] == -1){
		int frameNumber = findFrame();
		if(frameNumber == -1)	{
			frameNumber = findVictim(pcb);
			enableFindVictim=1;
		}

		char* filePath;
		constructFilePathFromPID(&filePath, pcb->pid);
		FILE* f=fopen(filePath, "r");
		error = loadPage(pcb->PC_page, f, frameNumber);
		if(error!=0)
			return error;
		updatePageTable(pcb, pcb->PC_page, frameNumber, enableFindVictim);
	}
	pcb->PC = (pcb->pageTable[pcb->PC_page])*4;
	pcb->PC_offset=0;
	return error;
}


