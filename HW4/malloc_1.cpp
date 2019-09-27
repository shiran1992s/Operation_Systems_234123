#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100000000

void *malloc(size_t size){
    if (size > MAX_SIZE || size == 0) {
        return NULL;
    }
   // printf("hihihihihi\n");
  
    void * n_break = sbrk((intptr_t)size);
            if ((*(int *)n_break) == -1) {
            return NULL;
        }
        else{
            return n_break;
        }
}

void* calloc(size_t num, size_t size){}

void free(void* p){}

void* realloc(void* oldp, size_t size){}


