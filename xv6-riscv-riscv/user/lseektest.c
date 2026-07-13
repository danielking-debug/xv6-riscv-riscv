#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main (void){
    int fd = open ("lseekfile", O_CREATE | O_RDWR);
    write(fd, "hello", 5);

    int pos = lseek(fd, 0, SEEK_SET);
    printf("after SEEK_SET: pos=%d\n", pos);

    char buf [6];
    buf[5] = 0;
    read(fd, buf, 5);
    printf("read back: %s\n", buf);

    pos = lseek(fd, 0, SEEK_END);
    printf("after SEEK_END: pos=%d\n", pos);
    write(fd, "world", 5);

    close(fd);

    fd = open("lseekfile", O_RDONLY);
    char buf2[11];
    buf2[10] = 0;
    read(fd, buf2, 10);
    printf("full file; %s\n", buf2);
    close(fd);

    exit(0);
}