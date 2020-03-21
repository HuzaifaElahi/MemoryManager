/*
 * 	ram.c
 *
 *  Created on: Feb. 23, 2020
 *      Author: ahmedelehwany
 *      Id:260707540
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"

int addToRAM(FILE *p, int *start, int *end){
    int currentLine = 0;
    while(!feof(p)){
        char *line = NULL;
        size_t linecap = 0;
        getline(&line, &linecap, p);

        while(ram[currentLine] != NULL){
            currentLine++;
            *start = currentLine;
            if(currentLine == 1000){
                printf("exec: RAM overflow");
                return 1;
            }
        }
        ram[currentLine] = strdup(line);
        *end = currentLine;
        currentLine++;
        if(currentLine == 1000){
                printf("exec: RAM overflow");
                return 1;
        }
    }
    return 0;
}
