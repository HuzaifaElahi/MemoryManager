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

void runCPU(int quanta){
    while(quanta > 0){
        strcpy(cpu->IR, ram[cpu->IP]);
        interpret(cpu->IR);
        cpu->IP++;
        quanta--;
    }
}
