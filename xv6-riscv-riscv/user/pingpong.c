#include  "kernel/types.h"
#include  "user/user.h"

int main (void) {
      // Two pipes: one for each direction
      //p1: parent writes 'n child reads
      //p2: child writes 'n parents reads
      int p1[2], p2[2];
      pipe(p1);
      pipe(p2);
      
      int N = 100000; // number of ping pong exchange to measure

      int pid = fork();

      if (pid == 0) {
        // -CHILD-
        // Child doesn't need the writes end of p1 or reads end of p2
        close(p1[1]);
        close(p2[0]);

        char buf;
        // keep recieving a byte on p1 and sending it back on p2
        while (read(p1[0], &buf, 1)==1) {
            write(p2[1], &buf, 1);
        }

        //When parents closes p1[1]. read return 0 - child exist cleanly
        close(p1[0]);
        close(p2[1]);
        exit(0);
      } else { 
        // -parents -
        // parents doesn't need the read end of p1 or writes end of p2
        close(p1[0]);
        close(p2[1]);

        char buf = 42;  // the byte we ping pong (value doesn't matter)

        // uptime () returns xv6 ticks (each ticks = 10ms by default)
        int start = uptime();

        for (int i = 0; i < N; i++) {
            write(p1[1], &buf, 1);  //send byte to child
            read(p2[0], &buf, 1);  //wait for child to send it back
        }

        int elapsed = uptime () - start; //total ticks taken

        printf("exchanges:        %d\n", N);
        printf("ticks elapsed: %d\n", elapsed);

        if (elapsed > 0)
            printf("exchange/sec:  %d\n", N * 100 / elapsed);
        else
            printf("too fast to measure, increase N\n");


        // Closing p1[1] signals childs's read to return 0 'n child exits
        close (p1[1]);
        close (p2[0]);

        wait(0);
        exit(0);
    }
}
        
        
            
        
        
      
