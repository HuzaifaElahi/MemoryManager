/*
 * 	cpu.c
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cpu.h"
#include "interpreter.h"
#include "ram.h"

int runCPU(int quanta){
    while(quanta > 0){
    	int address = (cpu->IP)*4 +(cpu->offset);
        strcpy(cpu->IR, ram[address]);
        interpret(cpu->IR);
        cpu->offset++;
        if(cpu->offset>=4)
        	return 1;
        quanta--;
    }
    return 0;
}
