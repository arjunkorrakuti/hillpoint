.section .text
.global loadGdt
.type loadGdt, @function
loadGdt:
  lgdt (%rdi)
  pushq $0x08
  leaq 1f(%rip), %rax
  pushq %rax
  lretq
1:
  movw $0x10, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  xorw %ax, %ax
  movw %ax, %fs
  movw %ax, %gs
  ret
.size loadGdt, .-loadGdt

.altmacro
.macro stub number
.type interrupt\number, @function
interrupt\number:
  .if (\number != 8) && (\number != 10) && (\number != 11) && (\number != 12) && (\number != 13) && (\number != 14) && (\number != 17) && (\number != 21) && (\number != 29) && (\number != 30)
    pushq $0
  .endif
  pushq $\number
  jmp interruptCommon
.endm

.set vector, 0
.rept 32
  stub %vector
  .set vector, vector + 1
.endr

.type interruptCommon, @function
interruptCommon:
  cld
  pushq %rax
  pushq %rbx
  pushq %rcx
  pushq %rdx
  pushq %rbp
  pushq %rsi
  pushq %rdi
  pushq %r8
  pushq %r9
  pushq %r10
  pushq %r11
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15
  # Preserve the frame while aligning the C++ call to the System V ABI.
  movq %rsp, %rbx
  movq %rsp, %rdi
  andq $-16, %rsp
  call interruptDispatch
  movq %rbx, %rsp
  popq %r15
  popq %r14
  popq %r13
  popq %r12
  popq %r11
  popq %r10
  popq %r9
  popq %r8
  popq %rdi
  popq %rsi
  popq %rbp
  popq %rdx
  popq %rcx
  popq %rbx
  popq %rax
  addq $16, %rsp
  iretq
.size interruptCommon, .-interruptCommon

.section .rodata
.balign 8
.global interruptStubs
interruptStubs:
.macro address number
  .quad interrupt\number
.endm
.set vector, 0
.rept 32
  address %vector
  .set vector, vector + 1
.endr

.section .note.GNU-stack,"",@progbits
