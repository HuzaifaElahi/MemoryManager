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

QUEUE_NODE *oldhead = NULL;
QUEUE_NODE *head = NULL;
QUEUE_NODE *tail = NULL;

CPU* cpu;
char *ram[40];
int main(int argc, const char *argv[])
{
	int error=0;
	boot();
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
//    printf("myinit call launcher\n");
    errCode = launcher(file);
    if(errCode != 0)	return errCode;

    return errCode;
}

void addToReady(PCB *pcb){
//	printf("start add to ready\n");
//	printf("///////\n");
//	printf("pid:%d\n", pcb->pid);
//	printf("max_lines:%d\n", pcb->lines_max);
//	printf("max_pages:%d\n", pcb->pages_max);
//	printf("page:%d\n", pcb->PC_page);
//	printf("offset:%d\n", pcb->PC_offset);
//	printf("finish add to ready\n");
//	printf("///////\n");
    QUEUE_NODE *newPCB = malloc(sizeof(QUEUE_NODE));
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
//    printf("start scheduler\n");

    while(head != NULL && head!=tail->next){
//   	printf("while loop\n");
        PCB* removeHead = head->thisPCB;
//        printf("%d\n", removeHead->pageTable[removeHead->PC_page]);
        if(removeHead->pageTable[removeHead->PC_page]==-1)
        	resolvePageFault(removeHead);
        cpu->IP = removeHead->pageTable[removeHead->PC_page];	    // Copy PC from PCB into IP of CPU
        cpu->offset = removeHead->PC_offset;
        oldhead = head;
        int InstructionsToExecute = removeHead->lines_max - ((removeHead->PC_page)*4+removeHead->PC_offset);
//        printf("pid=%d\n", removeHead->pid);
//        printf("page=%d\n", removeHead->PC_page);
//        printf("offset=%d\n", removeHead->PC_offset);
//        printf("max lines=%d\n", removeHead->lines_max);
//        printf("instructions to exec=%d\n", InstructionsToExecute);
        int interruptStatusFlag=0;
        if(cpu->quanta < InstructionsToExecute){
//        	printf("run cpu1\n");
        	interruptStatusFlag = runCPU(cpu->quanta);
//        	printf("interrupt status=%d\n", interruptStatusFlag);
        	if(interruptStatusFlag==1){
        		removeHead->PC_offset=0;
        		(removeHead->PC_page)++;
        		if(resolvePageFault(removeHead)==0)
        			addToReady(removeHead);
        	}
        	else{
        		removeHead->PC_offset = cpu->offset;
        		removeHead->PC = (cpu->IP)*4 + cpu->offset;
                addToReady(removeHead);		//add PCB back to the end of the ready queue
        	}
        }else{
//        	printf("run cpu2\n");
        	interruptStatusFlag = runCPU(InstructionsToExecute);
//        	printf("pid:%d\n", removeHead->pid);
//        	printf("interrupt status=%d\n", interruptStatusFlag);
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
    for(int i=0; i<40;i++){
    	ram[i]=NULL;
    }
}


