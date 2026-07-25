### XV6-riscv - Exercise solutions
Working through the xv6-RISCV operating system
textbook exercises by Cox, Kaashoek, and Morris,

Built as a portfolio project to demonstrate system
programming knowledge in C and low-level OS
concept-memory management,IPC,device trees,exec 
internals,device drivers,concurrency,and filesysystems.

### Completed Exercises
- Chapter 1 - Ping-Oong IPC Benchmark (user/pingpong.c)
  - Two processes passing a byte through pipes 100,000 times
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
  Exercise 4 - Shebang(#!) Support(kernel/exec.c)
   - exec() checks whether a file starts with #! before treating it as an ELF binary
   - Parses the interpreter path, builds a new argv (interpreter, original scripts path, remaining args), and recurively calls exec with the interpreter as the new program
   - Matches standard Unix shebang behavior (like #!/bin/sh)
   - Debugged down to a single misread byte: a script created via echo "#!/interp" > file had literal quote characters written into it, which echo without quoting resolved
  - Chapter 6 - Interrupts and Device Drivers
    Exercise 1-Polling UART(np interrupts)(kernel/uarts.c,Kernel/trap.c)
    - Removed interrupt driven UART entirely in favour of busy-wait polling
    - Disabled the UART's own interrupt-enable register(IER) and removed the UART branch from the PLIC interrupt dispatch in devintr()
    - Rewrote uartwrite() to busy-wait on the transmitter-idle status bit instead of sleeping on a wakeup that would never arrive, since no interrupt exists anymore to send it
    - Added uartpoll(), called every timer tick from clockintr(),to check for and process incoming keystrokes via consoleintr()
    - Root-caused a hang where uartpoll() had been declared but never actually called (void uartpool(void); sitting where the call should have been)-the exact kind of one-character-class bug that sillently does nothing rather than erroring
  - Chapter 7-Locking
    Exercise 3 - Per-CPU kalloc (kernel/kalloc.c)
    - Replaced a single global freelist (and its one connected lock) with one free list and one spinlock per CPU
    - kalloc() checks it own CPU's list first;if empty,it steals one page from another CPU's list rather than blocking
    - kfree() always return pages to the freeing CPU's own list
    - freemem()(Ch2Ex1) updated to sum across all per-CPU lists
    - Concepts:lock contention,per-CPU data structures,work-stealing
  - Chapter 9-Sleep and Wakeup
    Exercise 3-Fix the kill/sleep race (kernel/proc.c)
    - Closed a race where a process could be killed in the gap between checking a wait condition and actually calling sleep(), causing kill's wakeup to be missed and to process to sleep forever
    - sleep() now checks p->killed directly (while already holding p->lock) immediatly before marking the process SLEEPING, eliminating the window entirely
    - Also surfaced and fixed a seperate, urelated null-pointer bug in filewrite():the gap-filling logic added for the lseek exercise deferenced f->ip unconditionally, which crashed on pipe file descriptor (where ip is never set). Root-caused via objdump disassembly, matching the faulting instruction's stval offset (0x4c) to the ip->size field inside struct file. Fixed by guarding the gap-fill check with f->type ==FD_INODE
  - Chapter 10 - file system
    Exercise 7-implement lseek(kernel/sysfile.c,kernel/file.c)
    - New lseek(fd, offset, whence) syscall supporting SEEK_SET, SEEK_CUR, and SEEK_END
    - Update the file's offset without touching the contents
    - Modified filewrite() to zero-fill gaps when writing past current end of file, so seeking past EOF and writing creates a proper hole rather than exposing garbage data
    - Verified with a test program that seeks to start, reads back written data, seeks to end, appends more, and confirms full file contents match
    
 


## Environment
- OS: Ubuntu
- Emulator: QEMU RiSC-V
(qemu-system-riscv64, virt machine, 3harts)
- Compiler: riscv64-unknown-elf-gcc
   
 ### How to run
 '''Bash
 make qemu
 '''
 
 At the xv6 shell:
 '''
 pingpong    # Chapter 1 Exercise 1
 freetest    # Chapter 2 Exercise 2
 dtbtest     # Chapter 3 Exercise 1
 lseektest   # Chapter 10 Exercise 7
 usertest    # full regression suite

 Test the shebang feature:
 '''
 echo #!/myinterp > myscript
 myscript
 '''



 
 
