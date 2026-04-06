# GDB Cheat Sheet

Quick reference for common debugging tasks.

## Start / Attach

```bash
gdb ./program
gdb ./program core
```

- `run` or `r`: Start program
- `run arg1 arg2`: Start with args
- `attach <pid>`: Attach to running process
- `detach`: Detach without killing target
- `quit` or `q`: Exit GDB

## Breakpoints

- `break main`: Break at function
- `break file.cpp:42`: Break at line
- `break foo if x > 10`: Conditional breakpoint
- `info breakpoints`: List breakpoints
- `disable <n>` / `enable <n>`: Toggle breakpoint
- `delete <n>`: Remove breakpoint
- `clear file.cpp:42`: Remove breakpoint at line

## Stepping / Flow

- `continue` or `c`: Continue execution
- `next` or `n`: Step over (do not enter called function)
- `step` or `s`: Step into function
- `finish`: Run until current function returns
- `until`: Run until next line in current frame

## Stack / Frames

- `backtrace` or `bt`: Show call stack
- `bt full`: Stack + locals
- `frame <n>`: Jump to frame
- `up`: Move to caller frame
- `down`: Move to callee frame
- `info frame`: Current frame details

## Variables / Types

- `print expr` or `p expr`: Evaluate expression
- `ptype var_or_type`: Show type layout
- `info locals`: Show locals
- `info args`: Show function args
- `display expr`: Auto-print each stop
- `undisplay <n>`: Remove display expression

## Modify State

- `set var x = 42`: Set variable
- `set var obj.field = 7`: Set object field
- `call func(args)`: Call function in target
- `set {int}0xADDR = 123`: Write typed value to memory

## Memory / Registers

- `x/NFU ADDRESS`: Examine memory
	- `N` = count
	- `F` = format: `x` hex, `d` signed, `u` unsigned, `c` char, `s` string, `i` instruction
	- `U` = unit size: `b` 1B, `h` 2B, `w` 4B, `g` 8B

Examples:

- `x/16xb 0xADDR`: 16 raw bytes (hex)
- `x/8xh 0xADDR`: 8 halfwords (16-bit)
- `x/4xw 0xADDR`: 4 words (32-bit)
- `x/2xg 0xADDR`: 2 giant words (64-bit)
- `x/s 0xADDR`: Interpret as C string
- `x/10i $rip`: Show next 10 instructions

Registers:

- `info registers`: Show all registers
- `p $rip`, `p $rsp`, `p $rax`: Show specific register
- `disassemble` or `disas`: Disassemble current function
- `disas /m`: Mixed source + assembly

## Threads

- `info threads`: List all threads
- `thread <n>`: Switch current thread
- `thread apply all bt`: Backtrace for all threads (great for deadlocks)
- `thread`: Print current thread

## Signals

- `info signals`: Show signal handling config
- `handle SIGSEGV stop print pass`: Stop + print on segfault
- `handle SIGPIPE nostop noprint pass`: Ignore noisy SIGPIPE stops

## Core Dump Workflow

```bash
ulimit -c unlimited
./program
gdb ./program core
```

Inside GDB:

- `bt`
- `frame <n>`
- `info locals`
- `p var`

## Fast Deadlock Triage

1. `info threads`
2. `thread apply all bt`
3. `thread <n>`
4. `bt full`
5. `info locals`
6. `p mutex_or_state_var`
