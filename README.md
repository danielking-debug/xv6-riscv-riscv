Working through the xv6 operating system textbook excercises.

### Completed Exercises
- Chapter 1 - ping-pong IPC Benchmark (user/pingpong.c)
  - Two processes passing a byte through pipes
  - Measures kernel IPC performance in exchanges/sec
  - Concepts: fork, pipe, file descriptors, uptime


- Chapter 2 - freemem system call (kernel/kalloc.c, kernel/sysproc.c)
   - Added a syscall_freemem syscall to the xv6 kernel
   - Walks the kernel free page list to count available memory
   - Returns free memory in bytes, testable via user/freetest.c
   - Concept: syscalls, kernel memory, linked lists, defs.h
 
- Chapter 3 - Page table
   - Parse Riscv firmware device tree (FDT/DTB) at boot
   - Walks FDT structure looking for memory@ node
   - Fixed entry.s to preserve a1 register(DTB address) before stack setup overwrites it
   - Concepts: device tree, RISCV boot sequence, register conventions, FDT binary format
 




## Environment
- OS: Ubuntu
- Emulator: QEMU Riscv
   
 ### How to run
 bash
 make qemu
