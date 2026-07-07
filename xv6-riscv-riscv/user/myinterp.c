#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    printf("myinterp running!\n");
    printf("argc=%d\n" , argc);
    for(int i = 0; i < argc; i++)
       printf("argv[%d]=%s\n", i, argv[i]);
    exit(0);
}
