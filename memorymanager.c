/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memorymanager.h"
#include "ram.h"
#include "kernel.h"
#include "pcb.h"

int countTotalPages(FILE *f);
int countTotalLines(FILE *f);
int loadPage(int pageNumber, FILE *f, int frameNumber);
int findFrame();
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);
int findVictim(PCB *p);
int resolvePageFault(PCB *p);
int launchPaging(PCB *pcb, FILE* f, int totalPages);
int victimExistsInPCB(PCB *p, int index);
int constructFilePathFromPID(char** file, int pid);
int copyIntoBackingFile(FILE* fptr1, FILE* fptr2);
int createBackingStorageFile(char** file);
int isNull(int index);

const int PAGE_SIZE = 4;

QUEUE_NODE *head, *tail;
char *ram[40];
int generated_pid = 0;  // Keep track of PID values

// Launch File
int launcher(FILE *fptr1){
	char* file;
	int errCode = 0;

	// Create Backing Storage File & Copy File Contents Over
	createBackingStorageFile(&file);
	FILE *fptr2 = fopen(file, "w");
	copyIntoBackingFile(fptr1, fptr2);

	// Determine # Total Lines & # Total Pages
	FILE *f = fopen(file, "r");
	int numLines = countTotalLines(f);
	int numPages = countTotalPages(f);

	if(numPages > 10) {
		deleteBackingStorageFile(generated_pid);
		return -5;
	}

	// Make PCB, Launch Paging, Obtain PC from Page Table, Add PCB to Ready Queue
    PCB* pcb = makePCB(generated_pid, numPages, numLines);
    errCode = launchPaging(pcb, f, numPages);
    pcb->PC = pcb->pageTable[0];
    addToReady(pcb);
    generated_pid++;

	return errCode;
}

// Launch Paging given PCB, File ptr and Total Page #
int launchPaging(PCB *pcb, FILE* f, int totalPages){
	int errCode=0, enableFindVictim = 0;  // Flag to enable victim finding
	
	// Find Frame & Victim
	int frameNumber = findFrame();
	if(frameNumber == -1){
		frameNumber = findVictim(pcb);
		enableFindVictim = 1;
	}

	// Load Page & Update Page Table
	errCode = loadPage(0, f, frameNumber);
	if(errCode!=0) return errCode;
	updatePageTable(pcb, 0, frameNumber, enableFindVictim);

	// Re-run above logic if more than 1 Total Pages
	if(totalPages > 1){
		// Find Frame & Victim
		frameNumber = findFrame();
		if(frameNumber == -1)	{
			frameNumber = findVictim(pcb);
			enableFindVictim = 1;
		}

		// Load Page & Update Page Table
		errCode = loadPage(1, f, frameNumber);
		if(errCode!=0) return errCode;
		updatePageTable(pcb, 1, frameNumber, enableFindVictim);
	}
	return errCode;
}

// Load Page
int loadPage(int pageNumber, FILE *f, int frameNumber){
	fseek(f, 0, SEEK_SET);
	int offset = 0, lineCount = 0;
	int lineTarget = pageNumber * PAGE_SIZE;

    while(!feof(f)){
		size_t linecap = 0;
        char *line = NULL;
        getline(&line, &linecap, f);
    	if(lineCount >= lineTarget && lineCount < lineTarget + PAGE_SIZE){
    		ram[frameNumber * PAGE_SIZE + offset]=strdup(line);
    		offset++;
    	}
    	lineCount++;
    }
	return 0;
}

// Checks RAM starting at Index if frame is NULL
int isNull(int index){
	if(ram[index] == NULL &&
	 ram[index + 1] == NULL && 
	 ram[index + 2] == NULL && 
	 ram[index + 3] == NULL) {
		return 1;
	}
	return 0;
}

// Find Avaliable Frame Slot
int findFrame(){
	for(int i=0; i<37; i+= 4) if(isNull(i)) return i/4;
	return -1;
}

// Find Victim Index 
int findVictim(PCB *p){
	srand(time(0));
	int victimIndex = (rand() % 40) / PAGE_SIZE;
	while(victimExistsInPCB(p, victimIndex)) victimIndex = (victimIndex + 1) % 10;
	return victimIndex;
}

// Update Page table
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	if(victimFrame != 0){		// If victim selected, update other PCBs so they don't point to the removed victim
		QUEUE_NODE *pointer = head;
		while((pointer != NULL) && (pointer != tail->next)){
			PCB *thisPCB = pointer->thisPCB;
			for(int i = 0; i < 10; i++) if((thisPCB->pageTable[i]) == frameNumber) thisPCB->pageTable[i] =- 1;
			pointer = pointer->next;
		}
	}
	p->pageTable[pageNumber] = frameNumber;
	return 0;
}

// Resolve Page Fault 
int resolvePageFault(PCB *pcb){
	int error=0;
	int enableFindVictim=0;
	if(pcb->PC_page >= pcb->pages_max) return 1;
	
	// Check if RAM contains Frame
	if(pcb->pageTable[pcb->PC_page] == -1){

		// Find Frame & Victim
		int frameNumber = findFrame();
		if(frameNumber == -1)	{
			frameNumber = findVictim(pcb);
			enableFindVictim = 1;
		}

		// Create File Path & Load Page
		char* filePath;
		constructFilePathFromPID(&filePath, pcb->pid);
		FILE* f = fopen(filePath, "r");
		error = loadPage(pcb->PC_page, f, frameNumber);
		if(error != 0) return error;

		updatePageTable(pcb, pcb->PC_page, frameNumber, enableFindVictim); // Update Page Table if No Error in Loading Page
	}
	pcb->PC = (pcb->pageTable[pcb->PC_page]) * PAGE_SIZE + pcb->PC_offset;

	return error;
}

// Check Victim Exists in Given PCB
int victimExistsInPCB(PCB *p, int index){
	size_t length = sizeof(p->pageTable) / sizeof(int);
	for(int i = 0; i < length; i++)	if(p->pageTable[i] == index) return 1;
	return 0;
}

// Count Total Lines in given File
int countTotalLines(FILE *f){
	int lines = 0;
	fseek(f, 0, SEEK_SET);
    while(!feof(f)){
		size_t linecap = 0;
        char *line = NULL;
        getline(&line, &linecap, f);
    	lines++;
    }
    return lines;
}

// Count Total Pages in given File
int countTotalPages(FILE *f){
	int lines = countTotalLines(f);
    int pageCount = lines / PAGE_SIZE;
    if(lines % PAGE_SIZE != 0) pageCount++;

    return pageCount;
}

// Create File Path from a given PID Value
int constructFilePathFromPID(char** file, int pid){
	// Create File path & Set File Pointer
	char fileName[100];
	char filePath[100] = "BackingStore/";
	sprintf(fileName, "%d", pid);
	strcat(filePath, fileName);
	strcat(filePath, ".txt");
	*file = strdup(filePath);
	return 0;
}

// Delete Backing Storage File from given PID Value
int deleteBackingStorageFile(int pid){
	// Create & Run rm Command
	char* filePath;
	char rm_cmd[100];
	constructFilePathFromPID(&filePath, pid);
    strcpy(rm_cmd, "rm ");
    strcat(rm_cmd, filePath);
    system(rm_cmd);
    return 0;
}

// Create Backing Storage File for given File Pointer
int createBackingStorageFile(char** file){
	// Crate & Run touch Command 
	char* filePath;
	char touch_cmd[100];
	constructFilePathFromPID(&filePath, generated_pid);
	strcpy(touch_cmd, "touch ");
	strcat(touch_cmd, filePath);
	system(touch_cmd);

	// Set File Pointer
	*file = strdup(filePath);
	return 0;
}

// Copy File Contents
int copyIntoBackingFile(FILE* fptr1, FILE* fptr2){
	// Fetch characters from File 1 until end of file & Insert in File 2
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


