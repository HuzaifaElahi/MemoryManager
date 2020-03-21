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

PCB* makePCB(int start, int end){
    PCB* newPCB = malloc(sizeof(struct PCB_t));
    newPCB->PC = start;
    newPCB->start = start;
    newPCB->end = end;
    return newPCB;
}
