/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "ram.h"
#include "cpu.h"

const int MAX_OFFSET = 4;

// Takes Quanta Size & Runs Instructions
int runCPU(int quanta){
    while(quanta > 0){
        // Find & Run Next Instruction
    	int address = (cpu->IP) * MAX_OFFSET +(cpu->offset);
        strcpy(cpu->IR, ram[address]);
        interpret(cpu->IR);
        
        // Increase Offset
        cpu->offset++;
        if(cpu->offset >= MAX_OFFSET) return 1;
        quanta--;
    }
    return 0;
}
