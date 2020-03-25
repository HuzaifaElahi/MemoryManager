/*
 * 	kernel.c
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kernel.h"
#include "shell.h"
#include "ram.h"
#include "cpu.h"
#include "memorymanager.h"


struct QUEUE_NODE *oldhead = NULL;
struct QUEUE_NODE *head = NULL;
struct QUEUE_NODE *tail = NULL;

void addToReady(PCB *pcb);

CPU* cpu;
char *ram[40];
int main(int argc, const char *argv[])
{
	int error=0;
	boot();
	char* const path = "script1.txt";
	FILE *p = fopen(path, "r");
	launcher(p);
    error = kernel();
    return error;
}

int boot(){
	//create RAM global array of size 40 strings(not malloced). initializes array cells to null.
	//clears old backing storage directory ,create new directory. Directory name is "BackingStore".
    for(int i=0; i<40 ; i++){
        ram[i] = NULL;
    }
    char rm_cmd[100];
    strcpy(rm_cmd, "rm -r BackingStore");
    system(rm_cmd);

    char mkdir_cmd[100];
    strcpy(mkdir_cmd, "mkdir BackingStore");
    system(mkdir_cmd);
	return 0;
}

int kernel(){
	return shellUI();
}

int myinit(char *fileName){
	int errCode =0;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("exec: Script '%s' not found.\n", fileName);
        return 1;
    }
//    errCode = addToRAM(file, &start, &end);
//    if(errCode != 0)	return errCode;
    PCB* thisPCB = makePCB();
    addToReady(thisPCB);
    return errCode;
}

void addToReady(PCB *pcb){
    struct QUEUE_NODE *newPCB = malloc(sizeof(struct QUEUE_NODE));
    newPCB->thisPCB = pcb;
    if(head == NULL){
        head = newPCB;
        tail = newPCB;
    }else{
        tail->next = newPCB;
        tail = newPCB;
        tail->thisPCB = pcb;
    }
}


void scheduler(){
    cpu = malloc(sizeof(CPU));
    cpu->quanta = 2;

    while(head != NULL && head!=tail->next){
        PCB* removeHead = head->thisPCB;
        cpu->IP = removeHead->pageTable[removeHead->PC_page];	    // Copy PC from PCB into IP of CPU
        cpu->offset = removeHead->PC_offset;
        oldhead = head;
        int InstructionsToExecute = (removeHead->max_lines - (removeHead->PC_page)*4+removeHead->PC_offset) + 1;
        int interruptStatusFlag=0;
        if(cpu->quanta < InstructionsToExecute){
        	interruptStatusFlag = runCPU(cpu->quanta);
        	if(interruptStatusFlag==1){
        		if(resolvePageFault(removeHead)==0)
        			addToReady(removeHead);
        	}
        	else{
        		removeHead->PC_offset = cpu->offset;
        		removeHead->PC = (cpu->IP)*4 + cpu->offset;
                addToReady(removeHead);		//add PCB back to the end of the ready queue
        	}
        }else{
        	interruptStatusFlag = runCPU(InstructionsToExecute);
//        	if(interruptStatusFlag==1)
//        		resolvePageFault(removeHead);
            for(int i=0; i<10; i++){
            	if(removeHead->pageTable[i]!=-1){
            		int index = removeHead->pageTable[i];
                    ram[index] = NULL;
                    ram[index+1] = NULL;
                    ram[index+2] = NULL;
                    ram[index+3] = NULL;
            	}
            }
            free(oldhead);
        }
        head = head->next;
    }
    head = NULL;
    tail = head;
}


