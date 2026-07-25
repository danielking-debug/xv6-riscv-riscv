# xv6-riscv Exercise Solutions

A collection of completed exercises and kernel modifications from the xv6 RISC-V operating systems textbook by Cox, Kaashoek, and Morris.

Built as a systems programming portfolio project demonstrating knowledge of C, RISC-V architecture, kernel development, memory management, process management, device drivers, synchronization, and filesystem internals.

The goal of this project was not only to complete exercises, but to understand how operating systems work by modifying, debugging, and extending a real teaching kernel.

---

# Project Highlights

- Implemented new xv6 system calls from user interface to kernel execution
- Modified early RISC-V boot code and debugged register corruption issues
- Implemented Device Tree parsing during kernel initialization
- Added Unix-style shebang (`#!`) script execution support
- Converted UART from interrupt-driven I/O to polling-based operation
- Redesigned kernel memory allocation using per-CPU free lists
- Fixed process sleep/wakeup race conditions
- Implemented `lseek()` and sparse file support
- Used GDB, objdump, assembly analysis, and fault addresses to debug kernel failures

---

# Completed Exercises

# Chapter 1 — Unix Utilities

## Exercise 1 — Ping-Pong IPC Benchmark

**File:**
```
user/pingpong.c
```

Implemented a benchmark where two processes communicate by passing a byte through pipes 100,000 times.

The program measures kernel IPC performance in exchanges per second.

### Concepts:
- `fork()`
- Pipes
- File descriptors
- Process communication
- Kernel IPC overhead

---

# Chapter 2 — Operating System Organization

## Exercise 1 — freemem System Call

**Files:**

```
kernel/kalloc.c
kernel/sysproc.c
```

Added a system call that returns the amount of free physical memory in bytes.

Implementation:

- Traverses the kernel allocator free list(s)
- Counts available physical pages
- Converts pages into bytes using `PGSIZE`

### Concepts:
- System call implementation
- Kernel memory allocator
- Spinlocks
- Physical memory management

---

# Chapter 3 — Page Tables

## Exercise 1 — Device Tree Parser

**File:**

```
kernel/dtb.c
```

Implemented a parser for the RISC-V firmware Device Tree Blob (FDT/DTB).

The kernel now:

- Reads the firmware-provided device tree
- Locates the memory node
- Extracts physical memory size from the `reg` property

### Debugging Achievement

Found and fixed an early boot register corruption bug.

The DTB address was passed through the RISC-V `a1` register, but the stack setup code in `entry.S` overwrote the value before it was used.

Fixed by preserving the register value before stack initialization.

### Concepts:
- Device Tree Binary format
- RISC-V boot sequence
- Assembly debugging
- Calling conventions
- Register preservation

---

## Exercise 4 — Shebang (`#!`) Support

**File:**

```
kernel/exec.c
```

Added Unix-style interpreter support.

Example:

```bash
#!/bin/sh
```

When executing a script, xv6 now:

1. Detects the `#!` prefix
2. Extracts the interpreter path
3. Creates a new argument list
4. Executes the interpreter with the script path

Example:

```
./script.sh
```

becomes:

```
/bin/sh ./script.sh
```

### Debugging Achievement

Tracked down a script execution failure caused by a single incorrect byte.

A test script created incorrectly with:

```
echo "#!/interp" > file
```

contained literal quote characters, causing the interpreter path parser to fail.

The issue was discovered through low-level debugging of the loaded bytes.

### Concepts:
- ELF loading
- Process execution
- Argument manipulation
- Unix interpreter behavior

---

# Chapter 4 — Traps and System Calls

## Exercises 1–3 — Trampoline Analysis

Completed written analysis of xv6 trap architecture.

---

## Exercise 1

Analyzed why trap handling cannot completely move from assembly to C.

`kernelvec.S` requires assembly because:

- Registers must be saved before C execution
- The kernel stack must be established first

`trampoline.S` cannot be replaced by C because it executes during the page-table switch before a valid C execution environment exists.

---

## Exercise 2

Analyzed why the trapframe cannot simply be removed.

Before switching page tables, the CPU needs somewhere accessible to save critical information:

- Kernel stack pointer
- `satp` value
- Register state

Different designs are possible, but an equivalent mechanism is required.

---

## Exercise 3

Analyzed why the trampoline page is necessary.

After changing page tables, the CPU must continue executing instructions from an address mapped in both the old and new address spaces.

The trampoline page provides this shared execution location.

### Concepts:
- Trap handling
- Virtual memory
- Page table switching
- RISC-V privilege levels

---

# Chapter 6 — Interrupts and Device Drivers

## Exercise 1 — UART Polling Mode

**Files:**

```
kernel/uart.c
kernel/trap.c
```

Removed interrupt-driven UART handling and replaced it with polling.

Changes:

- Disabled UART interrupt-enable register (IER)
- Removed UART branch from PLIC interrupt handling
- Modified `uartwrite()` to busy-wait on transmitter readiness
- Added `uartpoll()` called from timer interrupts
- Processed incoming keyboard input through `consoleintr()`

### Debugging Achievement

Found a silent bug where:

```c
void uartpoll(void);
```

was declared but never actually called.

The kernel compiled successfully, but UART input stopped working because the function was never executed.

### Concepts:
- Device drivers
- UART hardware
- Interrupt handling
- Polling
- Hardware communication

---

# Chapter 7 — Locking

## Exercise 3 — Per-CPU Kernel Allocator

**File:**

```
kernel/kalloc.c
```

Replaced the single global memory free list with per-CPU allocators.

Implementation:

- Each CPU has its own free list
- Each free list has its own spinlock
- CPUs allocate locally first
- Empty CPUs steal pages from other CPUs

Updated `freemem()` to count memory across all CPU lists.

### Concepts:
- Lock contention
- Parallel allocation
- Per-CPU data structures
- Work stealing

---

# Chapter 9 — Sleep and Wakeup

## Exercise 3 — Fix Kill/Sleep Race

**File:**

```
kernel/proc.c
```

Fixed a race condition where a process could be killed between checking a sleep condition and entering the sleeping state.

Before the fix:

1. Process checks condition
2. Process is killed
3. Wakeup signal is missed
4. Process sleeps forever

Solution:

`sleep()` now checks:

```c
p->killed
```

while holding the process lock immediately before entering the sleeping state.

---

## Additional Kernel Debugging

Found and fixed a separate filesystem crash.

Problem:

`filewrite()` accessed:

```c
f->ip
```

without checking the file type.

Pipe file descriptors do not contain inode pointers, causing a kernel crash.

### Debugging process:

- Examined kernel fault information
- Used `stval` fault address
- Used `objdump` disassembly
- Matched instruction offset `0x4c` to the `ip->size` field inside `struct file`

Fixed by restricting inode-specific operations:

```c
if(f->type == FD_INODE)
```

---

# Chapter 10 — File System

## Exercise 7 — Implement lseek()

**Files:**

```
kernel/sysfile.c
kernel/file.c
```

Implemented the Unix `lseek()` system call.

Supported:

```
SEEK_SET
SEEK_CUR
SEEK_END
```

Features:

- Changes file offset without modifying file contents
- Supports seeking beyond EOF
- Creates sparse files when writing after gaps

Modified `filewrite()` to zero-fill gaps when writing past the end of a file.

### Verification

Created a test program that:

- Seeks to the beginning
- Reads existing data
- Seeks to the end
- Appends new data
- Confirms final file contents

---

# Testing

Implemented and verified features using xv6 user-space test programs.

Tests include:

- Ping-pong IPC benchmark
- Shebang script execution
- `freemem` system call verification
- `lseek()` file offset testing
- Sparse file behavior
- Kernel allocator changes

Run:

```bash
make grade
```

---

# Environment

## Operating System

```
Ubuntu (WSL)
```

## Emulator

```
QEMU RISC-V
qemu-system-riscv64
virt machine
3 harts
```

## Compiler

```
riscv64-unknown-elf-gcc
```

## Editor

```
VS Code
```

## Version Control

```
Git / GitHub
```

---

# How to Run

Clone:

```bash
git clone https://github.com/danielking-debug/xv6-riscv-riscv.git

cd xv6-riscv-riscv
```

Build and start xv6:

```bash
make qemu
```

Run tests:

```bash
make grade
```

---

# Demo

Screenshots and terminal demonstrations:

---

# Key Learnings

## RISC-V Boot Process

`entry.S` executes before any C code exists.

Early boot code can silently overwrite argument registers such as `a0` and `a1`, creating failures that appear much later as kernel crashes.

---

## Debugging Kernel Failures

A hang and a panic are different problems.

A system that boots but stops accepting input often indicates:

- A function was never called
- A wakeup was missed
- A device event is not being processed

A panic usually indicates:

- Invalid memory access
- Corrupted state
- Incorrect assumptions in kernel code

---

## Assembly and Memory Debugging

`objdump -d -S` can transform an unclear kernel crash into a specific source-level issue.

Matching a panic address with structure layout can reveal exactly which field caused the failure.

---

# Skills Demonstrated

- C programming
- RISC-V architecture
- Assembly debugging
- Operating system development
- Kernel programming
- System calls
- Memory management
- Virtual memory
- Device drivers
- Interrupt handling
- Synchronization
- Filesystem implementation
- Debugging with GDB and objdump

---

# License

This repository contains modifications and exercise solutions built on top of xv6-riscv from MIT PDOS.

The original xv6 source code remains under its original license.

My own modifications and exercise solutions are provided under the MIT License.

See:

```
LICENSE
```

for details.

---

GitHub:
https://github.com/danielking-debug
