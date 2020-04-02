/*
    Author: Muhammad Huzaifa Elahi
    ID: 260726386
*/

#include "interpreter.h"
#include "shellmemory.h"

#include <stdio.h>
#include <stdlib.h>

int shellUI(){
    printf("Kernel 1.0 loaded!\n"
    	   "Welcome to the Muhammad Huzaifa Elahi shell!\n"
           "Version 3.0 Updated March 2020\n");

    shell_memory_initialize();

    while (!feof(stdin)){
        printf("$ ");
        fflush(stdout);
        size_t linecap = 0;
        char *line = NULL;
        if (getline(&line, &linecap, stdin) == -1) break;
        (void)interpret(line);
        free(line);
    }
    shell_memory_destory();

    return 0;
}
