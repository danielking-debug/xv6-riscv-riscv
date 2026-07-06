#include "kernel/types.h"
#include "user/user.h"

int main (void){
    // call our new syscall that return memory size
    //we'll add this syscall next
    uint64 mem = memsize();
    printf("physical memory: %ld MB\n", mem / (1024 * 1024));
    exit(0);
}