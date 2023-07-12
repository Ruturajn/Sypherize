.section .data
a: .space 8
.section .text
jmp afteradd_int
add_int:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
sub $8, %rsp
add $40, %rsp
add $32, %rsp
pop %rbp
ret
afteradd_int:
.global main
main:
push %rbp
mov %rsp, %rbp
sub $32, %rsp
movq $-2, %rax
movq $3, %r10
imul %rax, %r10
movq $1, %rax
add %r10, %rax
movq $2, %r10
sub %rax, %r10
sub $8, %rsp
mov $a(%rip), %r10
mov %r10, %rax
add $40, %rsp
add $32, %rsp
pop %rbp
ret
