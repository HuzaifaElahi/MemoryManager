/*
 * 	pcb.c
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "pcb.h"

PCB* makePCB(int pid, int pages_max, int lines_max){
    PCB* newPCB = malloc(sizeof(struct PCB_t));
    newPCB->pid=pid;
    newPCB->pages_max=pages_max;
    newPCB->lines_max=lines_max;
    newPCB->PC_page=0;
    newPCB->PC_offset=0;
    for(int i=0; i<10;i++){
    	newPCB->pageTable[i]=-1;
    }
    return newPCB;
}
