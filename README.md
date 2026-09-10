# Hillpoint

A hobby x86-64 operating system written in C++23 and assembly, booted with Limine.
Built to explore how a kernel handles input, interrupts, memory, and files.

## Demo

https://github.com/user-attachments/assets/eeb95076-8a0a-457b-b649-a0bbc03c0677

## Features

- **Interactive console:** framebuffer text output with scrolling, a PS/2 keyboard
  driver, and a 15-command shell.
- **Memory management:** 4 KiB physical-page allocation and a kernel heap with
  block splitting, coalescing, and memory reclamation.
- **Interrupts and diagnostics:** CPU exception handling, a dedicated double-fault
  stack, a 100 Hz PIT timer, and serial debug output.
- **RAM filesystem:** create, read, overwrite, list, and delete up to 32 files,
  backed by dynamically allocated memory.

Hillpoint is a single-core kernel with all commands running in kernel mode, files are lost on reboot.

## Build and run

Requires an `x86_64-elf` GCC toolchain with C++23 support, CMake, Ninja, Make,
a host C compiler, xorriso, and QEMU. The run script uses Linux KVM and a GTK
display, so both must be available. Initial configuration downloads the Limine header to the project folder.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=/usr/local/osdev-cross/bin/x86_64-elf-g++
cmake --build build --target iso
./run_qemu.sh
```

Adjust the compiler path for your installation. To boot paused for debugging,
run `./run_qemu.sh debug` and connect GDB on port `1234`.

## Try it

Click the QEMU window and type `help` for the command list.

```text
hillpoint> help
help  List commands
about  Kernel features and editing keys
clear  Clear the screen
echo  Print text
uptime  Time since the PIT started
irq  Interrupt and input counters
mem  Physical page and heap statistics
alloc  alloc <bytes>: allocate a demo heap block
free  free <slot>: release a demo heap block
ls  List RAM files
write  write <name> [text]: create or replace a RAM file
cat  cat <name>: print a RAM file
rm  rm <name>: delete a RAM file
fault  fault invalid|page|double: demonstrate a fatal exception
halt  Stop the CPU
```
