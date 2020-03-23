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
#include "pcb.h"
#include "ram.h"
#include "cpu.h"
#include "memorymanager.h"

struct QUEUE_NODE {
    PCB *thisPCB;
    struct QUEUE_NODE *next;
} *head = NULL, *oldhead = NULL, *tail = NULL;

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

//    int pid=0;

//	char filePath[100] = "BackingStore/";
//	char extension[10];
//	char fileName[100];
//	sprintf(fileName, "%d", pid);
//	strcat(filePath, fileName);
//	strcat(filePath, ".txt");
//	char touch_cmd[100];
//	strcpy(touch_cmd, "touch ");
//	strcat(touch_cmd, filePath);
//	system(touch_cmd);
//	printf("%s\n", touch_cmd);

	return 0;
}

int kernel(){
	return shellUI();
}

int myinit(char *fileName){
    int start = 0;
    int end = 0;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("exec: Script '%s' not found.\n", fileName);
        return 1;
    }
    int errCode = addToRAM(file, &start, &end);
    if(errCode != 0)	return errCode;
    PCB* thisPCB = makePCB(start, end);
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
        cpu->IP = removeHead->PC;	    // Copy PC from PCB into IP of CPU
        oldhead = head;
        int InstructionsToExecute = (removeHead->end - removeHead->PC) + 1;

        if(cpu->quanta < InstructionsToExecute){
            runCPU(cpu->quanta);
            removeHead->PC = cpu->IP;
            addToReady(removeHead);		//add PCB back to the end of the ready queue
        }else{
            runCPU(InstructionsToExecute);
            for(int i = removeHead->start; i <= removeHead->end ; i++){
                ram[i] = NULL;
            }
            free(oldhead);
        }
        head = head->next;
    }
    head = NULL;
    tail = head;
}


