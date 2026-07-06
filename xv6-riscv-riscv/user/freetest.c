#include "kernel/types.h"
#include "user/user.h"

int main (void) {
    // call our new syscall and print the result
    uint64 mem = freemem();
    printf("free memory: %ld bytes\n", mem);
    printf("free memory: %ld KB\n", mem / 1024);
    printf("free memory: %ld MB\n" , mem / (1024 * 1024));
    exit(0);
}
