/*
 * memorymanager.c
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memorymanager.h"
#include "kernel.h"
#include "pcb.h"
#include "ram.h"

const int PAGE_SIZE = 4;

QUEUE_NODE *head;
QUEUE_NODE *tail;
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

int deleteBackingStorageFile(int pid){
	char* filePath;
	constructFilePathFromPID(&filePath, pid);
    char rm_cmd[100];
    strcpy(rm_cmd, "rm ");
    strcat(rm_cmd, filePath);
    system(rm_cmd);
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
	char lineBuffer[1000];
	while(fgets(lineBuffer, sizeof lineBuffer, fptr1) != NULL) {
		if(lineBuffer[0] == '\0' || lineBuffer[0] == '\n' || lineBuffer[0] == ' ')
			continue;  // skip the rest of the loop and continue
		fputs(lineBuffer, fptr2);
	}
    fclose(fptr1);
    fclose(fptr2);
    return 0;
}

int launchPaging(PCB *pcb, FILE* f, int totalPages){
	int error=0;
    int enableFindVictim=0;
	int frameNumber = findFrame();
	if(frameNumber == -1){
		frameNumber = findVictim(pcb);
		enableFindVictim=1;
	}
	error = loadPage(0, f, frameNumber);
	if(error!=0)
		return error;
	updatePageTable(pcb, 0, frameNumber, enableFindVictim);

	if(totalPages>1){
		frameNumber = findFrame();
		if(frameNumber == -1){
			frameNumber = findVictim(pcb);
			enableFindVictim=1;
		}
		error = loadPage(1, f, frameNumber);
		if(error!=0)
			return error;
		updatePageTable(pcb, 1, frameNumber, enableFindVictim);
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

int countTotalLines(FILE *f){
	fseek(f, 0, SEEK_SET);
	int lines=0;
    while(!feof(f)){
        char *line = NULL;
        size_t linecap = 0;
        getline(&line, &linecap, f);
    	lines++;
    }
    return lines;
}

int launcher(FILE *fptr1){
	char* file;
	int errCode=0;
	createBackingStorageFile(&file);
	FILE *fptr2 = fopen(file, "w");
	copyIntoBackingFile(fptr1, fptr2);
	FILE *f = fopen(file, "r");
	int totalLines = countTotalLines(f);
	int totalPages = countTotalPages(f);
	if(totalPages > 10) {
		deleteBackingStorageFile(generated_pid);
		return -1;
	}
    PCB* pcb = makePCB(generated_pid, totalPages, totalLines);
    errCode= launchPaging(pcb, f, totalPages);
    pcb->PC=pcb->pageTable[0];
    addToReady(pcb);
    generated_pid++;
	return errCode;
}

int countTotalPages(FILE *f){
	int lines = countTotalLines(f);
    int pageCount = lines/PAGE_SIZE;
    if(lines%PAGE_SIZE != 0)
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
	for(int i=0; i<37; i+=PAGE_SIZE){
		if(ram[i]==NULL && ram[i+1]==NULL && ram[i+2]==NULL && ram[i+3]==NULL){
			return i/4;
		}
	}
	return -1;
}

int findVictim(PCB *p){
	srand(time(0));
	int victimIndex = (rand()%40)/PAGE_SIZE;
	while(victimExistsInPCB(p, victimIndex)){
		victimIndex=(victimIndex+1)%10;
	}
	return victimIndex;
}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	if(victimFrame!=0){		//if victim was selected, update other PCBs to not point anymore to the removed victim
		QUEUE_NODE *pointer = head;
		while(pointer!=NULL && pointer!=tail->next){
			PCB *thisPCB = pointer->thisPCB;
			for(int i=0; i<10; i++){
				if((thisPCB->pageTable[i])==frameNumber){
					thisPCB->pageTable[i]=-1;
				}
			}
			pointer=pointer->next;
		}
	}
	p->pageTable[pageNumber] = frameNumber;
	return 0;
}

int resolvePageFault(PCB *pcb){
	int error=0;
	int enableFindVictim=0;
	if(pcb->PC_page>=pcb->pages_max)
		return 1;
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
	pcb->PC = (pcb->pageTable[pcb->PC_page])*PAGE_SIZE+pcb->PC_offset;
	return error;
}


