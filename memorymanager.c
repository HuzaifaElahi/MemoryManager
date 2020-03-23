/*
 * memorymanager.c
 *
 *  Created on: Mar. 22, 2020
 *      Author: ahmedelehwany
 */


int launcher(FILE *p){
	return 0;
}

int countTotalPages(FILE *f){
	int lines=0;
	int pageSize = 4;
    while(!feof(f)){
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

}

int findVictim(PCB *p){

}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame){

}

