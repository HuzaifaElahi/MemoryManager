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

#include "shell.h"
#include "pcb.h"
#include "ram.h"
#include "cpu.h"

struct QUEUE_NODE {
    PCB *thisPCB;
    struct QUEUE_NODE *next;
} *head = NULL, *oldhead = NULL, *tail = NULL;

void addToReady(PCB *pcb);

CPU* cpu;
char *ram[1000];

int main(int argc, const char *argv[])
{
    shellUI();
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


