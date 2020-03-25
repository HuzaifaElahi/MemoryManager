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

PCB* makePCB(){
    PCB* newPCB = malloc(sizeof(struct PCB_t));
    newPCB->PC_page=0;
    newPCB->PC_offset=0;
    return newPCB;
}
