Working through the xv6 operating system textbook excercises.

### Completed
-Chapter 1 - ping-pong IPC Benchmark (user/pingpong.c)
  - Two processes passing a byte through pipes
  - Measures kernel IPC performance in exchanges/sec
  - Concepts: fork, pipe, file descriptors, uptime

### Completed
- Chapter 2 - freemem system call (kernel/kalloc.c, kernel/sysproc.c)
   - Added a syscall_freemem syscall to the xv6 kernel
   - Walks the kernel free page list to count available memory
   - Returns free memory in bytes, testable via user/freetest.c
   - Concept: syscalls, kernel memory, linked lists, defs.h
