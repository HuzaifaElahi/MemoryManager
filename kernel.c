/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"
#include "cpu.h"
#include "kernel.h"
#include "shell.h"
#include "memorymanager.h"

const int QUANTA_SIZE = 2;

void resetRAM();
void removeFrame(int index);

QUEUE_NODE *head = NULL, *tail = NULL, *oldhead = NULL;
char *ram[40];
CPU* cpu;

int main(int argc, const char *argv[]){
	int errCode = 0;
	errCode = boot();
    errCode = kernel();
    return errCode;
}

// Boot up the OS by Initializing RAM & Setting Backing Storage Directory
int boot(){

	// Create RAM global array
    resetRAM();
    
    // Clear old backing storage dir
    char rm_cmd[100];
    strcpy(rm_cmd, "rm -r BackingStore");
    system(rm_cmd);

    // Create new dir "BackingStore"
    char mkdir_cmd[100];
    strcpy(mkdir_cmd, "mkdir BackingStore");
    system(mkdir_cmd);
	return 0;
}

// Run the Kernel to launch the Shell
int kernel(){
	return shellUI();
}

// Open & Launch Script
int myinit(char *fileName){
	int errCode =0;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("exec: Script '%s' not found.\n", fileName);
        return 1;
    }
    errCode = launcher(file);
    return errCode;
}

// Add PCB to Ready Queue
void addToReady(PCB *pcb){
    QUEUE_NODE *newPCB = malloc(sizeof(QUEUE_NODE));
    newPCB->thisPCB = pcb;

    // Insert at Head if Queue Empty
    if(head == NULL){
        head = newPCB;
        tail = newPCB;
    }
    // Else Insert at End of Queue
    else{
        tail->next = newPCB;
        tail = newPCB;
        tail->thisPCB = pcb;
    }
}

// Run Scheduler to Execute Instructions based on Ready Queue
void scheduler(){
    cpu = malloc(sizeof(CPU));
    cpu->quanta = QUANTA_SIZE;

    while((head != NULL) && (head != tail->next)){
        int interruptStatusFlag = 0;

        PCB* removeHead = head->thisPCB;
        if(removeHead->pageTable[removeHead->PC_page] == -1) resolvePageFault(removeHead);

        // Copy PC & Offset from PCB into CPU
        cpu->IP = removeHead->pageTable[removeHead->PC_page];	    
        cpu->offset = removeHead->PC_offset;

        oldhead = head;
        int InstructionsToExecute = removeHead->lines_max - ((removeHead->PC_page) * MAX_OFFSET + removeHead->PC_offset);
        
        // More Instructions to Execute than Quanta Avaliable
        if(cpu->quanta < InstructionsToExecute){
        	interruptStatusFlag = runCPU(cpu->quanta);
        	if(interruptStatusFlag == 1){
        		removeHead->PC_offset = 0;
        		(removeHead->PC_page)++;
        		if(resolvePageFault(removeHead) == 0) addToReady(removeHead); // Add PCB back to the end of the ready queue if resolved
        	}
        	else{
        		removeHead->PC_offset = cpu->offset;
        		removeHead->PC = (cpu->IP) * MAX_OFFSET + cpu->offset;
                addToReady(removeHead);		// Add PCB back to the end of the ready queue
        	}
        }
        // Instructions Executed Before Quanta Expires
        else{
        	interruptStatusFlag = runCPU(InstructionsToExecute);
            for(int i = 0; i < 10; i++){
            	if(removeHead->pageTable[i] != -1){
            		int index = (removeHead->pageTable[i]) * MAX_OFFSET;
                    removeFrame(index);
            	}
            }
            deleteBackingStorageFile(removeHead->pid);
            free(oldhead);
        }
        head = head->next;
    }
    head = NULL;
    tail = head;
    resetRAM(); // Reset RAM once scheduler finishes
}

// Reset RAM to NULL enteries
void resetRAM(){
    for(int i = 0; i < 40; i++) ram[i] = NULL;
}

// Set RAM entries values to NULL for Frame starting at index
void removeFrame(int index){
    ram[index] = NULL;
    ram[index + 1] = NULL;
    ram[index + 2] = NULL;
    ram[index + 3] = NULL;
}


