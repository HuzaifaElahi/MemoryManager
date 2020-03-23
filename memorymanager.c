/*
 * memorymanager.c
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "memorymanager.h"
#include "pcb.h"

int pid=0;

int createBackingStorageFile(char** file){
	char filePath[100] = "BackingStore/";
	char fileName[100];
	sprintf(fileName, "%d", pid);
	strcat(filePath, fileName);
	strcat(filePath, ".txt");
	char touch_cmd[100];
	strcpy(touch_cmd, "touch ");
	strcat(touch_cmd, filePath);
	system(touch_cmd);
	*file = strdup(filePath);
}

int copyIntoBackingFile(FILE* fptr1, FILE* fptr2){
    char c = fgetc(fptr1);
    while (c != EOF)
    {
        fputc(c, fptr2);
        c = fgetc(fptr1);
    }
    fputc('\n', fptr2);
    fputc('\n', fptr2);
    fclose(fptr1);
    fclose(fptr2);
}

int launcher(FILE *fptr1){
	char* file;
	createBackingStorageFile(&file);
	FILE *fptr2 = fopen(file, "w");
	copyIntoBackingFile(fptr1, fptr2);
	FILE *f = fopen(file, "r");
	int totalPages = countTotalPages(f);
//	printf("%d", totalPages);


	return 0;
}

int countTotalPages(FILE *f){
	int lines=0;
	int pageSize = 4;
    while(!feof(f)){
    	printf("dsnfjds\n");
    	lines++;
    }
    int pageCount = lines/pageSize;
    if(lines%pageSize != 0)
    	pageCount++;

    return pageCount;
}

void loadPage(int pageNumber, FILE *f, int frameNumber){

}

int findFrame(){
	return 0;
}

int findVictim(PCB *p){
	return 0;

}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){
	return 0;

}

