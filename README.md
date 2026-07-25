# xv6-riscv Exercise Solutions

A collection of completed exercises and kernel modifications from the xv6 RISC-V operating systems textbook by Cox, Kaashoek, and Morris.

This repository extends xv6 with new system features and kernel improvements while exploring core operating system concepts including system calls, memory management, virtual memory, process scheduling, interrupts, device drivers, synchronization, and filesystems.

Built as a systems programming portfolio project demonstrating C programming, RISC-V architecture, kernel debugging, and low-level operating system development.

---

# Project Highlights

- Implemented new xv6 system calls from user interface through kernel execution path
- Modified RISC-V boot assembly and debugged early boot register corruption issues
- Implemented device tree parsing during kernel initialization
- Added Unix-compatible shebang (`#!`) script execution support
- Redesigned kernel memory allocation using per-CPU free lists
- Modified UART driver architecture from interrupt-driven I/O to polling-based operation
- Debugged kernel crashes using assembly analysis, objdump, and fault address tracing
- Implemented filesystem improvements including `lseek()` and sparse file support

---

# Completed Exercises

# Chapter 1 — Unix Utilities

## Exercise 1 — Ping-Pong IPC Benchmark

**File:** `user/pingpong.c`

Implemented a benchmark where two processes exchange a byte through pipes 100,000 times.

### Concepts explored:
- Process creation (`fork`)
- Pipes and file descriptors
- Inter-process communication
- Kernel IPC performance measurement
- System uptime measurement

Measures IPC exchanges per second between processes.

---

# Chapter 2 — Operating System Organization

## Exercise 1 — freemem System Call

**Files:**
- `kernel/kalloc.c`
- `kernel/sysproc.c`

Added a new system call that returns the amount of free physical memory available in bytes.

Implementation:
- Traverses kernel memory allocator free lists
- Counts available pages
- Converts page count into bytes using `PGSIZE`

### Concepts explored:
- System call implementation
- Kernel memory allocator
- Spinlocks
- Physical memory management

---

# Chapter 3 — Page Tables

## Exercise 1 — Device Tree Parser

**File:** `kernel/dtb.c`

Implemented a parser for the RISC-V firmware Device Tree Blob (DTB/FDT).

The kernel now:
- Reads the firmware-provided device tree
- Searches for the memory node
- Extracts physical memory size from the `reg` property

### Debugging achievement

Found and fixed an early boot register corruption bug.

The DTB address was passed through the `a1` register, but stack initialization overwrote it before the kernel could use it.

Fixed by preserving the register value inside `entry.S`.

### Concepts explored:
- Device Tree Binary format
- RISC-V boot sequence
- Assembly debugging
- Calling conventions
- Register preservation

---

## Exercise 4 — Shebang (`#!`) Support

**File:** `kernel/exec.c`

Implemented Unix-style script execution.

Example:

```bash
#!/bin/sh
```

When executing a script, xv6 now:

1. Detects the `#!` prefix
2. Extracts the interpreter path
3. Builds a new argument list
4. Executes the interpreter with the script path as an argument

Example:

```
./script.sh
```

becomes:

```
/bin/sh ./script.sh
```

### Debugging achievement

Tracked down a script execution failure caused by a single incorrect byte.

The issue was caused by literal quote characters being written into the script due to incorrect shell usage.

### Concepts explored:
- ELF loading
- Process execution
- Argument manipulation
- Unix process conventions

---

# Chapter 4 — Traps and System Calls

## Exercises 1–3 — Trap Architecture Analysis

Completed written analysis of xv6 trap mechanisms.

### Exercise 1

Explored why trap handling cannot be completely rewritten in C.

Conclusion:

- `kernelvec.S` requires assembly for initial register handling and stack setup.
- `trampoline.S` cannot be replaced by C because it executes during page-table switching before a valid kernel environment exists.

---

### Exercise 2

Analyzed why the trapframe cannot simply be removed.

A memory location accessible before and after the page-table switch is required to preserve critical state such as:

- Kernel stack pointer
- `satp` value
- Saved registers

Alternative designs exist, but an equivalent mechanism is unavoidable.

---

### Exercise 3

Analyzed why the trampoline page is required.

After switching page tables, the CPU must continue executing instructions from an address mapped in both address spaces.

The trampoline page provides this shared execution location.

### Concepts explored:
- Trap handling
- Virtual memory switching
- Page table isolation
- RISC-V privilege transitions

---

# Chapter 6 — Interrupts and Device Drivers

## Exercise 1 — UART Polling Mode

**Files:**
- `kernel/uart.c`
- `kernel/trap.c`

Converted the UART driver from interrupt-driven I/O to polling-based operation.

Changes:
- Disabled UART interrupt generation
- Removed UART interrupt handling from PLIC dispatch
- Reworked `uartwrite()` to busy-wait on transmitter readiness
- Added `uartpoll()` to process incoming characters during timer interrupts

### Debugging achievement

Found a silent logic bug where `uartpoll()` was declared but never called.

A declaration existed:

```c
void uartpoll(void);
```

but the actual function invocation was missing.

### Concepts explored:
- Device drivers
- UART hardware
- Interrupt handling
- Busy waiting
- Hardware polling

---

# Chapter 7 — Locking

## Exercise 3 — Per-CPU Kernel Allocator

**File:** `kernel/kalloc.c`

Replaced the global kernel free list with independent per-CPU allocators.

Implementation:

- Each CPU owns a separate free list
- Each free list has its own spinlock
- CPUs allocate locally first
- Empty CPUs steal pages from other CPUs

Updated `freemem()` to count memory across all CPU lists.

### Concepts explored:
- Lock contention reduction
- Parallel memory allocation
- Per-CPU data structures
- Work stealing

---

# Chapter 9 — Sleep and Wakeup

## Exercise 3 — Fix Kill/Sleep Race

**File:** `kernel/proc.c`

Fixed a race condition where a process could be killed between:

1. Checking a sleep condition
2. Entering the sleeping state

This caused the wakeup signal from `kill()` to be missed, leaving the process sleeping forever.

Solution:

`sleep()` now checks `p->killed` while holding the process lock immediately before sleeping.

---

## Additional Kernel Debugging

Found and fixed an unrelated filesystem crash.

Problem:

`filewrite()` dereferenced `f->ip` for pipe file descriptors.

Pipes do not contain inode pointers, causing a kernel crash.

Debugging process:

- Used fault address from `stval`
- Examined generated assembly using `objdump`
- Matched the failing instruction offset to `struct file`

Fixed by restricting inode-specific logic:

```c
if(f->type == FD_INODE)
```

---

# Chapter 10 — File System

## Exercise 7 — Implement lseek()

**Files:**
- `kernel/sysfile.c`
- `kernel/file.c`

Implemented the Unix `lseek()` system call.

Supported operations:

```
SEEK_SET
SEEK_CUR
SEEK_END
```

Features:

- Changes file offset without modifying file contents
- Allows seeking beyond end-of-file
- Supports sparse file creation

Modified `filewrite()` so writing after seeking past EOF creates a zero-filled gap.

Testing verified:

- Seek to file start
- Read existing data
- Seek to end
- Append data
- Verify complete file contents

### Concepts explored:
- File descriptors
- File offsets
- Sparse files
- Filesystem internals

---

# Build and Run

## Requirements

- RISC-V GCC toolchain
- QEMU emulator
- Make

## Clone Repository

```bash
git clone https://github.com/danielking-debug/xv6-riscv-riscv.git

cd xv6-riscv-riscv
```

## Build and Run xv6

```bash
make qemu
```

## Run Tests

```bash
make grade
```

---

# Demo

Screenshots and terminal demonstrations will be added here.

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

# Skills Demonstrated

- C programming
- RISC-V architecture
- Assembly debugging
- Operating system design
- Kernel development
- Memory management
- Virtual memory
- Device drivers
- Synchronization
- Filesystem implementation
- Debugging with GDB and objdump
